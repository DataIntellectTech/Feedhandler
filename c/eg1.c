#include "blog.h"
#include "k.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int clientSocket;
struct sockaddr_in serverAddr;
socklen_t addr_size;
int callbackcounter;

K callback(I d) {

  char buffer[1024];
  int tt = recv(clientSocket, &buffer, 29, 0);

  k(0, ss(callbackname), kp(buffer), kj(callbackcounter), (K)0);
  callbackcounter += 1;
  return (K)0;
}

// start client listening to server
K startclient(K x) {

  logmessage(__FUNCTION__, "starting to connect to server");
  int port = x->j;
  callbackcounter = 0;
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(port);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size);
  int sz = 10*1024;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz));
  sd1(clientSocket, callback);
  return (K)0;
}

// return dictionary of messages sent and received
K getstats(K x) {
  K keys;
  K values;
  K dict;
  keys = knk(2, ks((S) "sent"), ks((S) "processed"));
  values = knk(2, kj(sent), kj(callbackcounter));
  return xD(keys, values);
}
