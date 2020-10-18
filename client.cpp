#include <HTTPReq.hpp>
#include <HTTPRes.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

//Função que retorna o endereço de um dado hostname
std::string getAddress(std::string hostname, std::string port){
	
	struct addrinfo hints;
	struct addrinfo* res;

	std::string ipString

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int status = getaddrinfo(hostname,"80",&hints,&res)
	if(status != 0){
		std::cerr << "ERRO: hostname nao encontrado" << std::endl;
		return 1;
	}

	for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
	    //Typecast para a estrutura de dados padrão
	    struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
	    
	    char ipstr[INET_ADDRSTRLEN] = {'\0'};
	    inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
	    ipString = ipstr
	}
	
	freeaddrinfo(res); 
	return ipString;
}

int main (int argc, char* argv[]){
	//Definindo a string para ler os argumnetos
	std::string requestUrl 
	std::string HTTP = "http://"
	HTTPReq request
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
		std::string tagInicial = requestUrl.substr(0,6);
		if(not(tagIncial == HTTP)){
			std:cerr << "ERRO: O request nao comeca com a tag http://" << std::endl;
			return 1;
		}
		
		//Se comeca com a tag HTTP, vamos pegar o hostname (valor da string até o :)
		int j
		std::string hostname = "";
		for(j = 7; requestUrl[j] != ":"; j++)
			hostname.append(requestUrl[j]);
		
		//Agora, vamos pegar o valor da porta
		std::string port = "";
		for(;requestUrl[j] != "/";j++)
			port.append(requestUrl[j]);

		//O final da url é o arquivo que temos que fazer a requisição para o servidor
		std::string file = hostname.substr(j,requestUrl.length()-1);

		//Achando o endereço IP do hostname
		std::string ipString = getAddress(hostname);
	
		//Vamos setar os parametros do request que será feito
		request.setMethod("URL");
		request.setURL(requestUrl);
		request.setHost(ipString);

		std::string requestString = request.makeRequest();


	}
}

