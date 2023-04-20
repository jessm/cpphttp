#include <functional>
#include <unordered_map>
#include <string>

#include "http.h"

class Server {
public:
    // Define the HTTP request handler type
    using Handler = std::function<HttpResponse(HttpRequest)>;
    Server(int port);

    // Set the request handler for GET requests
    void get(Handler handler);
    // Set the request handler for POST requests
    void post(Handler handler);

    // Start the server and listen for incoming requests
    void start();
private:
    int port_;

    // Map of methods to handlers
    std::unordered_map<HttpMethod, Handler> handlers_;

    void handle_request(int new_socket, HttpRequest request);
};
