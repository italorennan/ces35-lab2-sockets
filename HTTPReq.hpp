#include <iostream>
#include <string>

#ifndef HTTPREQ_H
#define HTTPREQ_H

// Classe para requisicoes HTTP

class HTTPReq {
    // Atributos da requisicao
    private:
        std::string t_method;
        std::string t_URL;
        std::string t_host;

    // Metodos da requisicao
    public:
        std::string getMethod() { return t_method; };
        std::string getURL() { return t_URL; };
        std::string getHost() { return t_host; };

        void setMethod(std::string method);
        void setURL(std::string URL);
        void setHost(std::string host);
        
        std::string makeRequest();
        unsigned char* encode();
        void parse(unsigned char* bytecode);
};

// Funcoes set

void HTTPReq::setMethod(std::string method) {
    // O unico metodo implementado eh GET
    if(method.compare("GET") != 0)
        std::cout << "Metodo nao implementado!" << std::endl;
    else 
        t_method = "GET";
}

void HTTPReq::setURL(std::string URL) {
    t_URL = URL;
}

void HTTPReq::setHost(std::string host) {
    t_host = host;
}

// Funcao para montar requisicao como string

std::string HTTPReq::makeRequest() {
    std::string request;

    request = "GET " + t_URL + " HTTP/1.0\r\n"
            + "Host: " + t_host + "\r\n"
            + "\r\n";

    return request;
}

// Funcao para montar requisicao como vetor de bytes

unsigned char* HTTPReq::encode() {
    std::string text = makeRequest();

    int size = text.size();
    unsigned char* request = new unsigned char[size];

    int i;
    for (i = 0; i < size; i++) {
        request[i] = (unsigned char) text[i];
    }
    request[i] = '\0';

    return request;
}

// Funcao para gerar objeto request a partir de bytecode

void HTTPReq::parse(unsigned char* bytecode) {
    std::string request(reinterpret_cast<char const*>(bytecode));
    
    int i = 0;
    std::string aux = "";

    // Obter metodo
    while(request[i] != ' ') {
        aux += request[i];
        i++;
    }
    setMethod(aux);
    i++;
    aux = "";

    // Obter URL
    while(request[i] != ' ') {
        aux += request[i];
        i++;
    }
    setURL(aux);
    aux = "";

    // Obter host
    i += 17;
    while(request[i] != '\r') {
        aux += request[i];
        i++;
    }
    setHost(aux);
}

#endif