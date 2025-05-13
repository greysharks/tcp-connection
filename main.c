#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[]) {
  const unsigned int IP = (127 << 24) + 1; // for ip 127.0.0.1
  const unsigned short int PORT = 80;
  const int MAX_CONNECTIONS = 10;

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(IP);

  int sock; // for socket

  sock = socket(addr.sin_family, SOCK_STREAM, 0);

  if (sock < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  if (argc > 1) {
    if (strcmp(argv[1], "-s") == 0) {
      int conns[MAX_CONNECTIONS]; // for connections

      unsigned short int N = 0; // N is number of connections

      if (bind(sock, (struct sockaddr*) &addr, sizeof (addr)) < 0) {
        perror("bind()");
        exit(EXIT_FAILURE);
      }

      if (listen(sock, MAX_CONNECTIONS) < 0) {
        perror("listen()");
        exit(EXIT_FAILURE);
      }

      printf("server is listening on port %hu\n", PORT);

      while (1) {
        if (N < MAX_CONNECTIONS) {
          int conn; // for connection

          conn = accept(sock, (struct sockaddr*) &addr, sizeof (&addr));

          conns[N] = conn;

          N += 1;
        }

      }
    }

    else if (strcmp(argv[1], "-c") == 0) {
      printf("client is running..\n");
    }

    else {
      perror("command-line arguments");
      exit(EXIT_FAILURE);
    }
  }

  close(sock);

  return 0;
}
