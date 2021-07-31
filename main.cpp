#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
#include "cerrno"

#define PORT 17

// log just prints some stuff
//
// Example usage: log("pog", "someone is pogging"); // [pog] someone is pogging
void log(const std::string& who, const std::string& event) {
    printf("[%s] %s\n", who.c_str(), event.c_str());
}

// handle_error handles the error and pretty prints it
//
// Example usage:  handle_error("socket", true); // [socket] An error occurred: Protocol not available. Code: 42
void handle_error(const std::string &at, bool die) {
    int saved_err = errno;
    if (saved_err != 0) {
        std::cout << "[" << at << "] " << "An error occurred: " << strerror(saved_err) << ". Code: " << saved_err
                  << std::endl;
        if (die) exit(saved_err);
    }
}

class Quote {
    std::vector<std::string> quote_of_the_day = {"Play Team Fortress 2, it's free!\r\n",
                                                 "Buy Team Fortress Classic, it's not free!\r\n"};
    public:
        std::string get() {
            return quote_of_the_day[rand() % quote_of_the_day.size()]; // limited randomness
        };
};

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    int opt = 1;
    int address_length = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    handle_error("socket", true);
    log("socket", "creating socket");

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    handle_error("setsockopt", true);
    log("setsockopt", "attaching socket to the port: ");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *) &address, sizeof(address));
    handle_error("bind", true);
    log("bind", "binding socket");

    listen(server_fd, 3);
    handle_error("listen", true);
    log("listen", "listening on the socket and now accepting new connections!");

    while (errno == 0) {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &address_length);
        handle_error("accept", true);
        log("accept", "new socket accepted");

        Quote quote;
        auto quote_to_send = quote.get();
        send(new_socket, quote_to_send.c_str(), quote_to_send.size(), 0);
        handle_error("send", true);
        log("send", "sending random quote of the day message");

        close(new_socket);
        handle_error("close", true);
        log("close", "closing new socket");
    }

    return 0;
}
