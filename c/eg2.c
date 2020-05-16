#include "blog.h"
#include "k.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int clientSocket;
int runclient;
int fd[2];
pthread_t client_thread;
char buffer[1024];
char kbuffer[1024];
struct sockaddr_in serverAddr;
socklen_t addr_size;

K callback(I d) {
  bzero(buffer, sizeof(kbuffer));
  int y = recv(fd[1], &kbuffer, 29, 0);
  k(0, ss(callbackname), kj(processed), kp(kbuffer), (K)0);
  processed += 1;
  return (K)0;
}

// message added to buffer
void *start_client(void *input) {

  while (runclient > 0) {

    bzero(buffer, sizeof(buffer));
    int y = recv(clientSocket, buffer, 29, 0);
    write(fd[0], &buffer, 29);
    received = received + 1;
  }
}

K startclient(K x) {

  logmessage(__FUNCTION__, "starting to connect to server");
  int port = x->j;
  runclient = 1;
  received = 0;
  processed = 0;

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

  keys = knk(3, ks((S) "sent"), ks((S) "received"), ks((S) "processed"));

  values = knk(3, kj(sent), kj(received), kj(processed));
  return xD(keys, values);
}

K stopclient() {

  runclient = 0;
  return ki(0);
}
