#include <string>
#include <unordered_map>

enum class HttpMethod {
    INVALID,
    GET,
    POST,
};

const std::unordered_map<HttpMethod, std::string> method_names = {
    {HttpMethod::INVALID, "INVALID"},
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
};

struct HttpRequest {
    std::string raw;
    HttpMethod method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    HttpRequest(std::string request);
};

struct HttpResponse {
    int status_code;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    HttpResponse(int status_code) :
        status_code(status_code) {}
    HttpResponse(int status_code, std::string body) :
        status_code(status_code),
        body(body) {}
    
    std::string to_string(void);
};
