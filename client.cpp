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

std::string serverRequest(std::string address, std::string port, std::string request){
	
	//Definindo o socket que foi passado como argumento 
	int socketfd = socket(AF_INET,SOCK_STREAM,0);
	std::cout << port << std::endl;
	int portInt = std::stoi(port);


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
	    	return UM;
	}
	
	
	//Enviando o request para o cliente
	if(send(socketfd,request.c_str(),request.size(),0) == -1){
		std::cerr << "ERRO: Nao foi possivel enviar o request ao socket" << std::endl;
		return UM;
	}

	//Recebendo o request do cliente
	std::string response;
	std::stringstream responseStream;
	char buffer[20] = {0};
	bool receivedMessage = false;

	//Recebendo mensagem 
	while(!receivedMessage){
	
		memset(buffer,'\0',sizeof(buffer));

		//Tentando receber a mensagem do servidor
		if(recv(socketfd,buffer,20,0)==-1){
			std::cerr << "ERRO: Nao recebi a mensagem do servidor" << std::endl;
			return UM;
		}

		//Pega o conteudo que foi recebido e colocando na resposta
		responseStream << buffer;
		response = buffer;

		if(response.length() == 0)
			break;
	}

	response = responseStream.str();
	return response;
}

int main (int argc, char* argv[]){
	//Definindo a string para ler os argumnetos
	std::string UM = "1";
	std::string requestUrl; 
	std::string HTTP = "http://";
	HTTPReq request;
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
		
		std::cout << hostname << std::endl;

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


		//Achando o endereço IP do hostname
		std::string addressString = getAddress(hostname);
		if(addressString == UM)
			return 1;


		//Vamos setar os parametros do request que será feito
		request.setMethod("GET");
		request.setURL(requestUrl);
		request.setHost(addressString);


		//Achando a string que, dados os parametros, faz o request
		std::string requestString = request.makeRequest();

		//Realizando o request
		std::cout << port << std::endl;
		std::string response = serverRequest(addressString,port,requestString);
		if(response == UM)
			return 1;


		std::string statusResponse = response.substr(9,3);
		std::string statusResponseDigit = response.substr(9,1);
		intDigit = std::stoi(statusResponseDigit);
		if(intDigit == 4){
			std::cout << "ERRO CÓDIGO: " << statusResponse << std::endl;
			return 4;
		}

		if(intDigit == 3{
			std::cout << "ARQUIVO MOVIDO CÓDIGO: " << statusResponse << std::endl;
			return 3;
		}

		if(initDigit == 2){
			std::cout << "Arquivo Encontrado" << std::endl;
			std::ofstream out(file);
			out << response;
			out.close;
			
		}
	}

	return 0;
}

