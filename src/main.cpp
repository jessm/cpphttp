#include <iostream>
#include <string>
#include <signal.h>
#include "lru.h"
#include "server.h"

void signal_callback_handler(int signum) {
   std::cout << "main: server exiting\n";
   exit(0);
}

int main(int argc, char* argv[]) {
    // Initialize server and cache
    Server server(8080);
    LRUCache cache(100);

    signal(SIGINT, signal_callback_handler);

    server.get([&](HttpRequest req) {
        std::string value = "";
        // value = cache.get(req.path);
        if (!value.empty()) {
            return HttpResponse(200, value);
        } else {
            return HttpResponse(404);
        }
    });

    server.post([&](HttpRequest req) {
        bool created = false;
        // created = cache.set(req.path, req.body);
        if (created) {
            return HttpResponse(201);
        } else {
            return HttpResponse(200);
        }
    });

    std::cout << "main: now listening\n";
    server.start();

    return 0;
}
