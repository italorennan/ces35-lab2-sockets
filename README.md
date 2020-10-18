# ces35-lab2-sockets

Instrucoes de uso:

- Os arquivos podem ser compilados pelo comando:

```js
$ make
```

- Para rodar o servidor, deve ser executado:

```js
$ ./web-server
```

- Para rodar o cliente (com possibilidade de multithread em múltiplos terminais, deve ser executado:

```js
$ ./web-client [URL] [URL]...
```
 
- Para baixar arquivos do servidor, a URL deve ser iniciada com http://localhost:40000/, seguida do nome do arquivo.
- Os arquivos acessados pelo servidor ficam na pasta /temp, enquanto que os arquivos baixados pelo cliente vao para a pasta /received.
