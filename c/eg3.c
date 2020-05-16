#include "blog.h"
#include "k.h"
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cb.h"

int clientSocket;
int runclient;
int fd[2];
int callbacks;

static cb queue;
pthread_t client_thread;
char buffer[1024];
char msgbuff[1024];
char mybuff[1024];
struct sockaddr_in serverAddr;
socklen_t addr_size;

K callback(I d) {
  char c;
  long qsize, msize;
  recv(d, &c, 1, 0);
  qsize = cb_size(&queue);
  K msg, dmsgs;
  if (qsize > 0) {

    dmsgs = ktn(0, 0);
    cb_read(&queue, buffer, qsize, 0);
    long offset = 0;
    while (offset < qsize) {

      memcpy(&msize, buffer + offset, sizeof(long));
      assert((msize > 0) && (msize < 500));

      memcpy(&mybuff, buffer + offset + sizeof(long), msize);
      msg = ks(mybuff); // msize);
      js(&dmsgs, msg);
      r0(msg);
      offset += msize + sizeof(long);
      processed = processed + 1;
    }
    callbacks = callbacks + 1;
    K rtn = k(0, ss(callbackname), kj(processed), dmsgs, (K)0);
  }

  return (K)0;
}

// message added to buffer
void *start_client(void *input) {

  while (runclient > 0) {

    bzero(buffer, sizeof(buffer));
    int y = recv(clientSocket, buffer, 29, 0);
    long msize = 29;
    memcpy(msgbuff, &msize, sizeof(long));
    memcpy(msgbuff + sizeof(long), buffer, msize);
    cb_write(&queue, msgbuff, msize + sizeof(long), 1);
    char dot = '.';
    write(fd[0], &dot, 1);
    received = received + 1;
  }
}

K startclient(K x) {

  logmessage(__FUNCTION__, "starting to connect to server");
  int port = x->j;
  runclient = 1;
  received = 0;
  processed = 0;
  callbacks = 0;
  cb_init(&queue, 10000000);
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(port);

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  addr_size = sizeof serverAddr;

  connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);

  socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);

  if (pthread_create(&client_thread, NULL, &start_client, &runclient)) {
    logmessage(__FUNCTION__, "threadstart. Error");
    return krr("thread");
  }
  sd1(fd[1], callback);
  return (K)0;
}

K getstats(K x) {

  K keys;
  K values;
  K dict;

  keys = knk(4, ks((S) "sent"), ks((S) "received"), ks((S) "processed"),
             ks((S) "callbacks"));

  values = knk(4, kj(sent), kj(received), kj(processed), kj(callbacks));
  return xD(keys, values);
}

K stopclient() {

  runclient = 0;
  return ki(0);
}
