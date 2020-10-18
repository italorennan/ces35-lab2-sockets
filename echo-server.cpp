#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <thread>

#include "HTTPReq.hpp"
#include "HTTPRes.hpp"


#include <iostream>
#include <sstream>

const int MAX_BYTES=200;

// Estabelecer conexão em multithread
int connection(int clientSockfd) {
    // faz leitura e escrita dos dados da conexao
    // utiliza um buffer de 20 bytes (char)
    bool isEnd = false;
    char buf[MAX_BYTES] = {0};
    unsigned char* unsigned_buf= new unsigned char[sizeof(buf)];

    std::ifstream arquivo;
    std::stringstream  leitura_arquivo;

    while (!isEnd)
    {
        // zera a memoria do buffer
        memset(buf, '\0', sizeof(buf));

        // recebe ate MAX_BYTES bytes do cliente remoto
        if (recv(clientSockfd, unsigned_buf, MAX_BYTES, 0) == -1)
        {
            perror("recv");
            return 5;
        }
        /*for (int i = 0; i < sizeof(buf); i++)
        {
            unsigned_buf[i] = (unsigned char) buf[i];
        }*/


        HTTPReq request;
        request.parse(unsigned_buf);

        HTTPRes response;
        response.setServer(request.getHost());

        arquivo.open(request.getURL());

        if(arquivo)
            response.setStatus(200);
        else
            response.setStatus(404);

        std::cout<<response.getStatus()<<std::endl;

        unsigned_buf= response.encode();

        for (int i = 0; i < sizeof(unsigned_buf); i++)
        {
            buf[i] = (char) unsigned_buf[i];
        }

        if (send(clientSockfd, unsigned_buf, MAX_BYTES, 0) == -1)
        {
            perror("send");
            return 6;
        }


        if(arquivo)
        {
            leitura_arquivo << arquivo.rdbuf(); //read the file
            std::string mensagem = leitura_arquivo.str(); //str holds the content of the file
            std::cout<<"O QUE ELE VAI ENVIAR: ";
            std::cout<<mensagem<<std::endl;

            if (send(clientSockfd, mensagem.c_str(), mensagem.size(), 0) == -1) {
            perror("send");
            return 7;
            }

        }


    }
    close(clientSockfd);
}

int main()
{
    // cria um socket para IPv4 e usando protocolo de transporte TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Opções de configuração do SOCKETs
    // No sistema Unix um socket local TCP fica preso e indisponível
    // por algum tempo após close, a não ser que configurado SO_REUSEADDR
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        return 1;
    }

    // struct sockaddr_in {
    //  short            sin_family;   // e.g. AF_INET, AF_INET6
    //  unsigned short   sin_port;     // e.g. htons(3490)
    //  struct in_addr   sin_addr;     // see struct in_addr, below
    //  char             sin_zero[8];  // zero this if you want to
    // };

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(40000);     // porta tem 16 bits, logo short, network byte order
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    // realizar o bind (registrar a porta para uso com o SO) para o socket
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return 2;
    }

    // colocar o socket em modo de escuta, ouvindo a porta
    if (listen(sockfd, 1) == -1)
    {
        perror("listen");
        return 3;
    }

    // Estabelecer conexoes TCP em loop infinito, ate o processo ser encerrado
    while(true) {
        // aceitar a conexao TCP
        // verificar que sockfd e clientSockfd sao sockets diferentes
        // sockfd eh a "socket de boas vindas"
        // clientSockfd eh a "socket diretamente com o cliente"
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);    

        if (clientSockfd == -1) {
            perror("accept");
            return 4;
        }

        // usa um vetor de caracteres para preencher o endereço IP do cliente
        char ipstr[INET_ADDRSTRLEN] = {'\0'};
        inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
        std::cout << "Accept a connection from: " << ipstr << ":" <<
                ntohs(clientAddr.sin_port) << std::endl;    

        // Criar thread para nova conexão TCP
        std::thread(connection, clientSockfd).detach();
    }

    return 0;
}
