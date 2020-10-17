#include "HTTPReq.hpp"
#include "HTTPRes.hpp"

#include <iostream>
#include <string.h>

int main() {
    HTTPReq request;
    request.setMethod("GET");
    request.setHost("localhost:21000");
    request.setURL("/index.html");

    unsigned char* buf;
    
    buf = request.encode();

    HTTPReq aux;
    aux.parse(buf);

    std::cout << aux.getMethod() << '\n' << aux.getURL() << '\n' << aux.getHost();

    return 0;
}