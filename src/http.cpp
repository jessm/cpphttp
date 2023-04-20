#include "http.h"

#include <sstream>

HttpRequest::HttpRequest(std::string request) {
    raw = request;
    std::istringstream iss(request);
    std::string line;
    std::getline(iss, line);

    // Parse method and path
    std::istringstream method_line(line);
    std::string method_str, path;
    method_line >> method_str >> path;
    if (method_str == "GET") {
        method = HttpMethod::GET;
    } else if (method_str == "POST") {
        method = HttpMethod::POST;
    } else {
        method = HttpMethod::INVALID;
    }
    this->path = path;

    // Parse headers
    while (std::getline(iss, line) && line != "\r") {
        std::istringstream header_line(line);
        std::string key, value;
        std::getline(header_line, key, ':');
        header_line.ignore(1);  // ignore the space after the colon
        std::getline(header_line, value);
        headers[key] = value;
    }

    // Parse body
    std::stringstream body_stream;
    body_stream << iss.rdbuf();
    body = body_stream.str();
}

std::string HttpResponse::to_string(void) {
    std::string message = status_code / 100 == 2 ? "OK" : "ERROR";
    std::string resp = "HTTP/1.1 " + std::to_string(status_code) + " " + message + "\r\n";

    headers["Content-Length"] = std::to_string(body.length());

    for (auto& header : headers) {
        resp += header.first + ": " + header.second + "\r\n";
    }
    resp += "\r\n";
    resp += body;

    return resp;
}
