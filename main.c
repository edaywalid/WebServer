#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  // define the port number as what is after -p flag
  int PORT = 8080;

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-p") == 0) {
        if (i + 1 >= argc) {
          printf("Please provide a port number\n");
          return 1;
        }
        if (atoi(argv[i + 1]) == 0) {
          printf("Please provide a valid port number\n");
          return 1;
        } else {
          PORT = atoi(argv[i + 1]);
        }
      }
    }
  }

  // create two socket for server and client_socket
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t addr_size = sizeof(struct sockaddr_in);

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    printf("Error creating socket\n");
    return 1;
  }

  server_address.sin_addr.s_addr =
      INADDR_ANY; // this will bind the socket to all available interfaces
  server_address.sin_family = AF_INET;   // IP protocol family (IPv4)
  server_address.sin_port = htons(PORT); // port number

  // bind the socket to the address and port number
  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    printf("Error binding socket\n");
    perror("Bind error");
    close(server_socket);
    return 1;
  }

  // listen for incoming connections
  if (listen(server_socket, MAX_CONNECTIONS) == -1) {
    printf("Error listening\n");
    close(server_socket);
    return 1;
  }

  printf("Server is listening on port %d\n", PORT);

  // accept incoming connections
  while (1) {
    client_socket =
        accept(server_socket, (struct sockaddr *)&client_address, &addr_size);
    if (client_socket == -1) {
      printf("Error accepting connection\n");
      continue;
    }

    // read data from the client
    char buffer[BUFFER_SIZE];
    int read_size = read(client_socket, buffer, BUFFER_SIZE);
    if (read_size == -1) {
      printf("Error reading from client\n");
      close(client_socket);
      continue;
    }

    buffer[read_size] = '\0';
    printf("Client says: %s\n", buffer);

    // send data to the client
    // PARSE THE HTTP REQUEST AND DETERMINE THE REQUESTED FILE
    char method[BUFFER_SIZE], path[BUFFER_SIZE], protocol[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    printf("Method: %s\n", method);
    printf("Path: %s\n", path);
    printf("Protocol: %s\n", protocol);

    if (strcmp(path, "/") == 0) {
      strcpy(path, "/index.html");
    } else {
      memmove(path, path + 1, strlen(path));
    }

    // determine the file type
    char *extension = strrchr(path, '.');
    char *content_type;

    if (extension != NULL) {
      if (strcmp(extension, ".html") == 0) {
        content_type = "text/html";
      } else if (strcmp(extension, ".css") == 0) {
        content_type = "text/css";
      } else if (strcmp(extension, ".js") == 0) {
        content_type = "text/javascript";
      } else if (strcmp(extension, ".jpg") == 0) {
        content_type = "image/jpeg";
      } else if (strcmp(extension, ".png") == 0) {
        content_type = "image/png";
      } else if (strcmp(extension, ".gif") == 0) {
        content_type = "image/gif";
      } else if (strcmp(extension, ".ico") == 0) {
        content_type = "image/x-icon";
      } else {
        content_type = "text/plain";
      }
    } else {
      content_type = "text/plain";
    }

    // open the file
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
      printf("Error opening file\n");
      const char *not_found_response =
          "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
      write(client_socket, not_found_response, strlen(not_found_response));
      close(client_socket);
      continue;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // send the response header
    char response_header[BUFFER_SIZE];
    sprintf(response_header,
            "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
            content_type, file_size);
    write(client_socket, response_header, strlen(response_header));

    // send the file content
    char file_buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(file_buffer, 1, BUFFER_SIZE, file)) > 0) {
      write(client_socket, file_buffer, bytes);
    }

    // close the file and the client socket
    fclose(file);
    close(client_socket);
  }

  // close the server socket
  close(server_socket);

  return 0;
}
