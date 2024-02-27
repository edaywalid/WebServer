#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define RESPONSE_HEADER_SIZE 1000
#define FILENAME "index.html"

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  // Open index.html file
  FILE *html_file = fopen(FILENAME, "r");
  if (html_file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Get file size
  fseek(html_file, 0, SEEK_END);
  long file_size = ftell(html_file);
  fseek(html_file, 0, SEEK_SET);

  char *html_content = (char *)malloc(file_size + 1);
  if (html_content == NULL) {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  // Read content from index.html
  fread(html_content, 1, file_size, html_file);
  fclose(html_file);
  html_content[file_size] = '\0'; // Null-terminate the string

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  // Construct response header
  char response_header[RESPONSE_HEADER_SIZE];
  sprintf(response_header,
          "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
          "%ld\r\n\r\n",
          file_size);

  printf("Response Header:\n%s\n", response_header);

  // Send response header
  send(new_socket, response_header, strlen(response_header), 0);

  // Send HTML content
  send(new_socket, html_content, file_size, 0);

  printf("HTML content sent\n");

  // Free dynamically allocated memory
  free(html_content);

  return 0;
}
