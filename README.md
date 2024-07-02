# Simple HTTP Server in C

This project is a simple HTTP server implemented in C. The server listens on a specified port and serves static files to clients. It supports basic content types such as HTML, CSS, JavaScript, and common image formats.

## Features

- Handles multiple connections using a single-threaded loop.
- Serves static files such as HTML, CSS, JavaScript, and images.
- Parses HTTP GET requests and responds with the requested file.
- Supports customizable port number via command-line arguments.

## Getting Started

### Prerequisites

- A C compiler (e.g., `gcc`)
- Basic knowledge of C programming and socket programming

### Installation

1. Clone the repository:

```sh
git clone https://github.com/edaywalid/WebServer
cd WebServer
```

2. Compile the source code:

```sh
gcc -o main main.c
```

### Usage

Run the server with the default port (8080):

```sh
./main
```

Or specify a custom port using the `-p` flag:

```sh
./main -p 3000
```

### Accessing the Server

Open a web browser and navigate to:

```
http://localhost:8080/index.html
```

Replace `8080` with your specified port if using a custom port.

## How It Works

1. The server initializes a socket and binds it to the specified port.
2. It listens for incoming connections and enters an infinite loop to handle them.
3. For each connection:
   - Reads the HTTP request from the client.
   - Parses the request to determine the requested file.
   - Determines the content type based on the file extension.
   - Opens and reads the requested file.
   - Sends the HTTP response header and the file content to the client.
   - Closes the client connection.

### Supported Content Types

- HTML: `.html`
- CSS: `.css`
- JavaScript: `.js`
- JPEG images: `.jpg`
- PNG images: `.png`
- GIF images: `.gif`
- ICO images: `.ico`
- Plain text: Other file types

## Example

Here's an example of how the server responds to a request for `index.html`:

```
GET /index.html HTTP/1.1
Host: localhost:8080
```

Response:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

<!DOCTYPE html>
<html>
<head>
    <title>Welcome</title>
</head>
<body>
    <h1>Hello, World!</h1>
</body>
</html>
```

## Acknowledgements

- Inspired by the simplicity and educational value of creating a basic HTTP server in C.
