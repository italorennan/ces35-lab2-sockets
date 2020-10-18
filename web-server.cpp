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
#include <iostream>
#include <sstream>

#include "HTTPReq.hpp"
#include "HTTPRes.hpp"

const int MAX_BYTES = 200;
std::string nome_host_aux = "localhost";
int port = 40000;
std::string nome_host = nome_host_aux + ":" + std::to_string(port);
std::string pasta_temporaria = "temp/";

void enviar_mensagem(int clientSockfd, unsigned char* mensagem, long int size) {
    int num_partes = size/MAX_BYTES;
    int modulo = size - num_partes*MAX_BYTES;
    int minimo, maximo;
    unsigned char* mensagem_unsigned = new unsigned char[MAX_BYTES];

    for (int i = 0; i < num_partes+1; i++) {
        memset(mensagem_unsigned,'\0',MAX_BYTES);
        
        std::cout << "Enviando " << i+1 << " de " << num_partes+1 << " partes do arquivo..." << std::endl;
    
        if (modulo != 0 || i != num_partes) {
            minimo = i*MAX_BYTES;
            if(i != num_partes) 
                maximo = (i+1)*MAX_BYTES-1;
            else
                maximo = size-1;

            for (int j = 0; j <= (maximo - minimo); j++)
                mensagem_unsigned[j] = mensagem[minimo + j];

            // envia de volta o buffer recebido como um echo
            if (send(clientSockfd, mensagem_unsigned, MAX_BYTES, 0) == -1) {
                perror("send");
            }
        }

        std::cout << "Arquivo enviado com sucesso." << std::endl << std::endl;
    }

    delete[] mensagem_unsigned;
}

int connection(int clientSockfd) {
    // faz leitura e escrita dos dados da conexao
    // utiliza um buffer de 20 bytes (char)
    bool isEnd = false;
    char buf[MAX_BYTES] = {0};
    unsigned char* unsigned_buf = new unsigned char[sizeof(buf)];

    FILE * arquivo;

    while (!isEnd) {
        // zera a memoria do buffer
        memset(unsigned_buf, '\0', sizeof(unsigned_buf));

        // recebe ate MAX_BYTES bytes do cliente remoto
        if (recv(clientSockfd, unsigned_buf, MAX_BYTES, 0) == -1)
        {
            perror("recv");
            return 5;
        }

        if (unsigned_buf[0] != '\0') {
            HTTPReq request;
            request.parse(unsigned_buf);

            HTTPRes response;
            response.setServer(nome_host);

            std::string name;

            if(request.getURL() == "") {
                name = pasta_temporaria + "index.html";
            }
            else {
                name = pasta_temporaria + request.getURL();
            }

            // Abrir arquivo
            int nameSize = name.length();
            char* fileName = new char[name.length()];
            for (int i = 0; i < nameSize; i++)
                fileName[i] = name[i];
            arquivo = fopen(fileName, "rb");

            unsigned char * mensagem;
            long int size = 0;

            if(arquivo != NULL) {
                response.setStatus(200);
                
                fseek(arquivo, 0, SEEK_END);
                size = ftell(arquivo);

                response.setLength(size);
                fclose(arquivo);

                arquivo = fopen(fileName, "rb");
                mensagem = new unsigned char [size];
                fread(mensagem, sizeof(unsigned char), size, arquivo);
            }
            else {
                response.setStatus(404);
            }

            unsigned_buf = response.encode();
            std::cout << "Response:" << std::endl;
            std::cout << unsigned_buf << std::endl;

            if (send(clientSockfd, unsigned_buf, MAX_BYTES, 0) == -1) {
                perror("send");
                return 6;
            }

            if(arquivo != NULL) {
                enviar_mensagem(clientSockfd, mensagem, size);
                delete[] mensagem;
                fclose(arquivo);
            }
        }
    }

    close(clientSockfd);
    return 0;
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

    // aceitar a conexao TCP
    // verificar que sockfd e clientSockfd sao sockets diferentes
    // sockfd eh a "socket de boas vindas"
    // clientSockfd eh a "socket diretamente com o cliente"
    while (true)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSockfd == -1)
        {
            perror("accept");
            return 4;
        }

        // usa um vetor de caracteres para preencher o endereço IP do cliente
        char ipstr[INET_ADDRSTRLEN] = {'\0'};
        inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
        std::cout << "Accept a connection from: " << ipstr << ":" <<
                  ntohs(clientAddr.sin_port) << std::endl;

        // Criar conexões em multithread
        std::thread(connection, clientSockfd).detach();
    }
    return 0;
}
