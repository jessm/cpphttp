#include "server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>

void Server::start() {
    // Create the server socket with IPv4, byte stream instead of datagrams, and default protocol
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Server: Socket creation failed\n";
        return;
    }

    // Set socket options
    int opt_value = 1; // yes, we do want to reuse address and port
    int setsockopt_ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt_value, sizeof(opt_value));
    if (setsockopt_ret == -1) {
        std::cerr << "Server: Setting socket option failed\n";
        return;
    }

    // Bind the socket to the given port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    int bind_ret = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (bind_ret < 0) {
        std::cerr << "Couldn't bind to socket\n";
        return;
    }

    // Listen for incoming connections
    constexpr int max_pending_connections = 256;
    int listen_ret = listen(server_fd, max_pending_connections);
    if (listen_ret < 0) {
        std::cerr << "Couldn't listen on socket\n";
        return;
    }

    int requests_handled = 0;
    // Accept incoming connections and handle requests
    while (true) {
        socklen_t socklen;
        int new_socket = accept(server_fd, (struct sockaddr *)&address, &socklen);
        if (new_socket < 0) {
            std::cerr << "Failed to accept connection\n";
            return;
        }

        // Read the HTTP request from the socket
        std::string request_raw = "";
        size_t content_length = 0;
        size_t body_break_pos = 0;
        HttpMethod method = HttpMethod::INVALID;
        while (request_raw.length() < body_break_pos + strlen("\r\n\r\n") + content_length) {
            constexpr int buffer_size = 4096;
            char buffer[buffer_size] = { 0 };
            ssize_t num_bytes = recv(new_socket, buffer, buffer_size-1, 0);
            buffer[num_bytes] = '\0';
            if (num_bytes <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                if (num_bytes == 0) {
                    break;
                }
                std::cout << "Error reading from socket, errno: " << errno << "\n";
                return;
            }
            request_raw.append(buffer, num_bytes);
            // If the buffer size is more than 4, check if it's a get or post request
            if (request_raw.length() < 4) {
                continue;
            }
            if (method == HttpMethod::INVALID) {
                method = request_raw.substr(0, 4) == "POST" ? HttpMethod::POST : HttpMethod::GET;
            }

            // If it's a get request, break when \r\n\r\n is found
            body_break_pos = request_raw.find("\r\n\r\n");
            if (method == HttpMethod::GET) {
                if (body_break_pos != std::string::npos) {
                    break;
                }
            } else if (method == HttpMethod::POST) {
                // If it's a post request, find \r\n\r\n and find "Content-Length: "
                size_t content_length_pos = request_raw.find("Content-Length: ");
                // If Content-Length: is before \r\n\r\n, parse the number, else fail
                if (body_break_pos != std::string::npos && content_length_pos < body_break_pos) {
                    size_t end_of_line = request_raw.find("\r\n", content_length_pos);
                    size_t value_start = content_length_pos + strlen("Content-Length: ");
                    std::string length_str = request_raw.substr(value_start, end_of_line - value_start);
                    content_length = std::stoi(length_str);
                }
            } else {
                continue; // haven't read enough of the message yet
            }
        }

        // Parse the HTTP request
        HttpRequest request(request_raw);
        if (request.method == HttpMethod::INVALID) {
            close(new_socket);
            continue;
        }

        // std::cout << method_names.find(request.method)->second << " " << request.path << "\n";

        handle_request(new_socket, request);

        // Close the connection
        close(new_socket);
    }
}

void Server::handle_request(int new_socket, HttpRequest request) {
    // Find the appropriate handler for the request method
    Handler handler;
    auto it = handlers_.find(request.method);
    if (it != handlers_.end()) {
        handler = it->second;
    } else {
        std::cerr << "Invalid method received: " << request.raw << "\n";
        return;
    }

    // Call the request handler and send the response
    HttpResponse response = handler(request);
    std::string response_str = response.to_string();
    int send_idx = 0;
    while (true) {
        int num_sent = send(new_socket, response_str.c_str() + send_idx, response_str.length() - send_idx, 0);
        if (num_sent <= 0) {
            break;
        }
        send_idx += num_sent;
    }
}

Server::Server(int port) {
    port_ = port;
    handlers_.clear();
}

void Server::get(Handler handler) {
    handlers_[HttpMethod::GET] = handler;
}

void Server::post(Handler handler) {
    handlers_[HttpMethod::POST] = handler;
}
