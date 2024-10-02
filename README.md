# Overview

## HTTP Server Implementation in C++

This project involves writing a custom HTTP server, inspired by Nginx, using C++98. It's an exploration into the intricacies of the HTTP protocol, one of the most widely used protocols on the internet.

### Key Features

- **Language**: C++98
- **I/O Multiplexing**: Non-blocking I/O using `poll()`
- **HTTP Methods**: Supports GET, POST, and DELETE
- **Configuration**: Server configuration via a custom configuration file
- **Static Content**: Ability to serve static websites
- **File Uploads**: Support for client file uploads
- **Error Handling**: Custom error pages

### Technical Specifications

- Compliant with C++98 standard
- Non-blocking operation, handling multiple clients simultaneously
- Custom configuration file for server setup (similar to Nginx)
- No external libraries used (except standard C++98 libraries)

### Building and Running

```bash
make
./NginxLite [configuration_file]
```

### Configuration File

The configuration file allows you to:

- Set port and host for each "server"
- Configure server names
- Set up routes with specific rules
- Define error pages
- Limit client body size
- Configure directory listings
- Set up CGI execution based on file extensions

### Testing

- Compatible with standard web browsers
- Tested against Nginx for HTTP 1.1 compliance
- Includes stress tests to ensure server availability

