#include "HTTPReq.hpp"
#include "HTTPRes.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 200

struct httpFullResponse{

	HTTPRes httpResponse;
	unsigned char* file;
};

//Função que retorna o endereço de um dado hostname
std::string getAddress(std::string hostname){
	
	struct addrinfo hints;
	struct addrinfo* res;

	std::string ipString;
	std::string UM;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname.c_str(),"80",&hints,&res);
	if(status != 0){
		std::cerr << "ERRO: hostname nao encontrado" << std::endl;
		return UM;
	}

	for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
	    //Typecast para a estrutura de dados padrão
	    struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
	    
	    char ipstr[INET_ADDRSTRLEN] = {'\0'};
	    inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
	    ipString = ipstr;
	}
	
	freeaddrinfo(res); 
	return ipString;
}

struct httpFullResponse serverRequest(std::string address, std::string port, std::string request){
	
	//Definindo o socket que foi passado como argumento 
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	std::cout << port << std::endl;
	int portInt = std::stoi(port);

	struct httpFullResponse fullResponse;

	struct sockaddr_in serverAddr;

	//Conectando o socket do servidor
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portInt);
	serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
	memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

	std::string UM = "1";

	//Testando se a conexão é bem sucedida
	if (connect(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "ERRO: Nao foi possivel conectar no socket" << std::endl;
	    	return fullResponse;
	}
	
	
	//Enviando o request para o cliente
	if(send(socketfd,request.c_str(),request.size(),0) == -1){
		std::cerr << "ERRO: Nao foi possivel enviar o request ao socket" << std::endl;
		return fullResponse;
	}

	//Recebendo o request do cliente
	std::string response;
	std::stringstream responseStream;
	std::string fileString;
	std::stringstream fileStream;
	unsigned char * mensagem;
	unsigned char buffer[BUFFER_SIZE] = {0};
	bool receivedMessage = false;
	HTTPRes httpRes;
	
	//Recebendo mensagem HTTP
	while(!receivedMessage){
	
		memset(buffer,'\0',sizeof(buffer));

		//Tentando receber a mensagem do servidor
		if(recv(socketfd,buffer,BUFFER_SIZE,0)==-1){
			std::cerr << "ERRO: Nao recebi a mensagem do servidor" << std::endl;
			return fullResponse;
		}

		//Pega o conteudo que foi recebido e colocando na resposta
		responseStream << buffer;
		response = responseStream.str();

		if(response.find("\r\n\r\n") != std::string::npos){
			httpRes.parse((unsigned char*)response.c_str());
			break;	
		}
	}

	int numeroChar = httpRes.getLength();

	mensagem = new unsigned char[numeroChar];
	int read = 0;
	
	while(read < numeroChar){
		memset(buffer,'\0',sizeof(buffer));

		//Tentando receber a mensagem do servidor
		int received = recv(socketfd, buffer, BUFFER_SIZE, 0);

		//if(recv(socketfd,buffer,BUFFER_SIZE,0)==-1){
		if (received == -1) {
			std::cerr << "ERRO: Nao recebi a mensagem do servidor" << std::endl;
			return fullResponse;
		}

		else{
			for (int i = 0; i < received; i++) {
				if (read < numeroChar) {
					mensagem[read] = buffer[i];
					read++;
				}
			}
		}
	}

	fullResponse.file = mensagem;
	fullResponse.httpResponse = httpRes;

	std::cout << httpRes.makeResponse() << std::endl;
	
	return fullResponse;
}

int main (int argc, char* argv[]){
	//Definindo a string para ler os argumnetos
	std::string UM = "1";
	std::string requestUrl; 
	std::string HTTP = "http://";
	HTTPReq request;
	struct httpFullResponse fullResponse;
	//Vamos checar se algum argumento foi passado como argumento
	//Se nenhum argumento foi passado, vamos retornar erro

	if(argc < 2){
		std::cerr << "ERRO: Voce deve passar algum endereco como argumento" << std::endl;
		return 1;
	}
	
	//Loop que faz a requisição para cada um dos servidores que foram passados com argumentos
	for (int i = 1; i < argc; i++){
		requestUrl = argv[i];

		//Vamos checar se o request começa com a tag http
		std::string tagInicial = requestUrl.substr(0,7);
		if(not(tagInicial == HTTP)){
			std::cerr << "ERRO: O request nao comeca com a tag http://" << std::endl;
			return 1;
		}
		
		//Se comeca com a tag HTTP, vamos pegar o hostname (valor da string até o :)
		int j;
		std::string hostname = "";
		for(j = 7; requestUrl[j] != ':'; j++)
			hostname+= requestUrl[j];

		j++;
		//Agora, vamos pegar o valor da porta
		std::string port = "";
		while(requestUrl[j] != '/'){
			port+=requestUrl[j];
			j++;
		}

		j++;
		//O final da url é o arquivo que temos que fazer a requisição para o servidor
		std::string file = requestUrl.substr(j);
		if (file.length() == 0) file = "index.html";

		//Achando o endereço IP do hostname
		std::string addressString = getAddress(hostname);
		if(addressString == UM)
			return 1;


		//Vamos setar os parametros do request que será feito
		request.setMethod("GET");
		request.setURL(file);
		request.setHost(addressString);


		//Achando a string que, dados os parametros, faz o request
		std::string requestString = request.makeRequest();

		//Realizando o request
		fullResponse = serverRequest(addressString,port,requestString);

		std::string status = fullResponse.httpResponse.getStatus();
		std::string DUZENTOS = "200 OK";
		if(status == DUZENTOS){
			std::cout << "Arquivo Encontrado" << std::endl;

			std::string name = "receive/" + file;
			int nameSize = name.length();
			char* fileName = new char[name.length()];
            for (int i = 0; i < nameSize; i++)
                fileName[i] = name[i];

			FILE * arquivo = fopen(fileName, "wb");
			fwrite(fullResponse.file, sizeof(unsigned char), fullResponse.httpResponse.getLength(), arquivo);
			fclose(arquivo);

			delete[] fileName;
		}
		
		else{
			std::cerr << "ERRO: Arquivo nao encontrado";
			return 1;
		}

	}

	return 0;
}

