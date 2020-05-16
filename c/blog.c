#include "blog.h"
#include "k.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

pthread_t listen_thread;

int socket_desc, c, client_sock;
struct sockaddr_in server;
struct sockaddr_in client;

int keep_listening = 0;
int no_connections = 0;
int connected = 0;
int waittime = 0;
int sent = 0;
char client_message[2000];

void logmessage(const char *whom, char *message) {
   printf(" Info: %s %s\n", whom, message);
  return;
}

void *start_listen(void *);

void mysleep(int millis) {

  usleep(millis * 1000);

  return;
}

K startserver(K port, K freq) {

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  waittime = freq->j;
  int enable = 1;
  sent = 0;
  callbackname = "f";
  if (socket_desc == -1) {
    logmessage(__FUNCTION__, "Could not create socket");
    return krr("noinit");
  }
  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons((int)port->j);
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    // print the error message
    logmessage(__FUNCTION__, "bind failed. Error");
    return krr("bind");
  }
  keep_listening = 1;
  if (pthread_create(&listen_thread, NULL, &start_listen, &freq->i)) {
    logmessage(__FUNCTION__, "threadstart. Error");
    return krr("thread");
  }

  return ki(0);
}

void *start_listen(void *freq)

{
  int write_success = 0;

  logmessage(__FUNCTION__, "threadstarted.");
  listen(socket_desc, 3);
  logmessage(__FUNCTION__, "Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  while (keep_listening > 0) {
    client_sock =
        accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
    int sz = 100 * 1024;
    setsockopt(client_sock, SOL_SOCKET, SO_SNDBUF, &sz, sizeof(sz));
    logmessage(__FUNCTION__, "connection received");
    if (client_sock < 0) {
      logmessage(__FUNCTION__, "accept failed");
      return NULL;
    }
    connected = 1;
    while ((keep_listening > 0) && c > 0) {
      mysleep(waittime);
      sprintf(client_message, "#### Hello world %6i  ####", sent);
      // Send the message back to client
      write_success =
          write(client_sock, client_message, strlen(client_message));
      if (write_success < 0) {
        printf(" Hello \n");
        logmessage(__FUNCTION__, "Not successful write");
        c = 0;
      }
      sent = sent + 1;
    }
  }

  connected = 0;
  logmessage(__FUNCTION__,
             "listen thread closing as the listen value has changed.");
  return NULL;
}

K stopserver(K x) {
  logmessage(__FUNCTION__, "stopping thread listening");
  keep_listening = 0;
  close(socket_desc);
  return ki(0);
}

