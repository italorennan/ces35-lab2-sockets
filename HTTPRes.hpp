#include <iostream>
#include <string>

#ifndef HTTPRES_H
#define HTTPRES_H

// Classe para respostas HTTP

class HTTPRes {
    // Atributos da resposta
    private:
        std::string t_statusCode;
        std::string t_server;
        int t_contentLength;

    // Metodos da resposta
    public:
        std::string getStatus() { return t_statusCode; };
        std::string getServer() { return t_server; };
        int getLength() { return t_contentLength; };

        void setStatus(int statusCode);
        void setServer(std::string server);
        void setLength(int contentLength);

        std::string makeResponse();
        unsigned char* encode();
        void parse(unsigned char* bytecode);
};

// Funcoes set

void HTTPRes::setStatus(int statusCode) {
    if(statusCode == 200)
        t_statusCode = "200 OK";
    else if(statusCode == 400)
        t_statusCode = "400 Bad Request";
    else if(statusCode == 404)
        t_statusCode = "404 Not Found";
    else
        std::cout << "Status nao implementado!" << std::endl;
}

void HTTPRes::setServer(std::string server) {
    t_server = server;
}

void HTTPRes::setLength(int contentLength) {
    t_contentLength = contentLength;
}

// Funcao para montar requisicao como string

std::string HTTPRes::makeResponse() {
    std::string response;

    response = "HTTP/1.0 " + t_statusCode + "\r\n"
            + "Server: " + t_server + "\r\n";

    if(t_statusCode == "200 OK")
        response += "Content-Length: " + std::to_string(t_contentLength) + "\r\n";
    
    response += "\r\n";
    
    return response;
}

// Funcao para montar resposta como vetor de bytes

unsigned char* HTTPRes::encode() {
    std::string text = makeResponse();

    int size = text.size();
    unsigned char* response = new unsigned char[size];

    int i;
    for (i = 0; i < size; i++) {
        response[i] = (unsigned char) text[i];
    }
    response[i] = '\0';

    return response;
}

// Funcao para gerar objeto response a partir de bytecode

void HTTPRes::parse(unsigned char* bytecode) {
    std::string response(reinterpret_cast<char const*>(bytecode));

    int i = 9;
    std::string aux = "";

    // Obter status
    while(response[i] != ' ') {
        aux += response[i];
        i++;
    }
    setStatus(std::stoi(aux));
    aux = "";

    // Obter servidor
    aux = "";
    while(response[i] != '\n') {
        i++;
    }
    i += 9;
    while(response[i] != '\r') {
        aux += response[i];
        i++;
    }
    setServer(aux);
    aux = "";

    if(t_statusCode == "200 OK") {
        // Obter tamanho
        i += 18;
        while(response[i] != '\r') {
            aux += response[i];
            i++;
        }
        setLength(std::stoi(aux));
    }
    else {
        setLength(0);
    }
}

#endif