#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[]) {
  const unsigned int IP = (127 << 24) + 1; // for ip 127.0.0.1
  const unsigned short int PORT = 80;
  const unsigned short int MAX_CONNECTIONS = 10;

  // structure that holds server's IP, port number and address family
  // (it can be used either to create a server with these values or
  // either to connect to the server with these values)
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(IP);

  int sock; // for socket

  // initialize socket for either client or server
  sock = socket(addr.sin_family, SOCK_STREAM, 0);

  if (sock < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  if (argc > 1) {
    // if program is used as server
    if (strcmp(argv[1], "-s") == 0) {
      int conns[MAX_CONNECTIONS]; // for connections
      unsigned short int N = 0; // N is number of connections
      char message[] = "HELLO FROM SERVER";

      // used only for accept(...) function
      unsigned int addr_len = sizeof (addr);

      // mark unexisting connections as NULL
      for (unsigned short int i = 0; i < MAX_CONNECTIONS; i++) {
        conns[i] = NULL;
      }

      // bind server socket with server's IP address, port number and address family
      if (bind(sock, (struct sockaddr*) &addr, sizeof (addr)) < 0) {
        close(sock);
        perror("bind()");
        exit(EXIT_FAILURE);
      }

      // make the server listen to new connections until they reach MAX_CONNECTIONS
      if (listen(sock, MAX_CONNECTIONS) < 0) {
        close(sock);
        perror("listen()");
        exit(EXIT_FAILURE);
      }

      printf("server is listening on port %hu\n", PORT);

      while (1) {
        // add new connections to the array that holds all connections (as they occur)
        if (N < MAX_CONNECTIONS) {
          int conn; // for connection
          conn = accept(sock, (struct sockaddr*) &addr, &addr_len);

          if (conn >= 0) {
            conns[N] = conn;
            N += 1;
          }
        }

        // go through all connections (skipping unexisting ones) to send server's message
        for (unsigned short int i = 0; i < MAX_CONNECTIONS; i++) {
          if (conns[i] == NULL) {
            continue;
          }

          if (send(conns[i], message, sizeof (message), 0) < 0) {
            perror("send()");
            exit(EXIT_FAILURE);
          }

          // close current connection after sending server's message
          // (because in this implementation server sends only one message
          // and doesn't receive any messages from the client)
          close(conns[i]);
          conns[i] = NULL;
          N -= 1;
        }
      }
    }

    // if program is used as client
    else if (strcmp(argv[1], "-c") == 0) {
      const unsigned int BUFF_SIZE = 512; // max size of bytes of receive buffer
      char buff[BUFF_SIZE]; // for receive buffer
      unsigned short int bytes_received = 0;

      // connect to the server using its port number and IP
      if (connect(sock, (struct sockaddr*) &addr, sizeof (addr)) < 0) {
        close(sock);
        perror("connect()");
        exit(EXIT_FAILURE);
      }

      // try to recevie the first portion of bytes from the server
      bytes_received = recv(sock, buff, BUFF_SIZE, 0);

      // keep printing all bytes of data that client recevies from the server
      while (bytes_received > 0) {
        for (unsigned short int i = 0; i < bytes_received; i++) {
          putchar(buff[i]);
        }

        bytes_received = recv(sock, buff, BUFF_SIZE, 0);
      }

      // check if any error occured while trying to receive data from server
      // (in other case the client successfully received all the data)
      if (bytes_received < 0) {
        close(sock);
        perror("recv()");
        exit(EXIT_FAILURE);
      }
    }

    // otherwise wrong arguments were passed to program
    else {
      close(sock);
      perror("command-line arguments");
      exit(EXIT_FAILURE);
    }
  }

  close(sock);

  return 0;
}
