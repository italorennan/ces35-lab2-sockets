#include "HTTPReq.hpp"
#include "HTTPRes.hpp"

#include <iostream>
#include <string.h>

int main() {
    // Teste de request

    HTTPReq request;
    request.setMethod("GET");
    request.setHost("localhost:21000");
    request.setURL("/index.html");

    std::cout << request.makeRequest();

    unsigned char* buf;
    
    buf = request.encode();

    HTTPReq aux;
    aux.parse(buf);

    std::cout << aux.getMethod() << std::endl << aux.getURL() << std::endl << aux.getHost() << std::endl << std::endl;

    // Teste de response

    HTTPRes response;
    response.setStatus(404);
    response.setServer("Leite");
    response.setLength(50);

    std::cout << response.makeResponse();

    unsigned char* buf2;

    buf2 = response.encode();

    std::cout << buf2;

    HTTPRes aux2;
    aux2.parse(buf2);

    std::cout << aux2.getStatus() << '\n' << aux2.getServer() << '\n' << aux2.getLength() << std::endl << std::endl;

    return 0;
}