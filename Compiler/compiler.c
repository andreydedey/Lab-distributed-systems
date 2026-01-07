#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void *handle_client(void *arg) {
    int newsockfd = *(int *)arg;
    free(arg);
    char buffer[4096];
    int n;

    // Recebe o código PROLOG do cliente
    bzero(buffer, sizeof(buffer));
    n = read(newsockfd, buffer, sizeof(buffer)-1);
    if (n <= 0) {
        close(newsockfd);
        return NULL;
    }

    // Salva o código em um arquivo temporário
    char filename[64];
    snprintf(filename, sizeof(filename), "client_code_%d.pl", newsockfd);
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        write(newsockfd, "ERRO: Falha ao criar arquivo.\n", 30);
        close(newsockfd);
        return NULL;
    }
    fwrite(buffer, 1, strlen(buffer), fp);
    fclose(fp);

    // Executa o código usando SWI-Prolog e captura saída/erros
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "swipl -q -f %s -g halt 2>&1", filename);
    FILE *out = popen(cmd, "r");
    if (!out) {
        write(newsockfd, "ERRO: Falha ao executar Prolog.\n", 32);
        close(newsockfd);
        return NULL;
    }
    char result[4096];
    bzero(result, sizeof(result));
    fread(result, 1, sizeof(result)-1, out);
    pclose(out);

    // Envia a saída/erros de volta ao cliente
    write(newsockfd, result, strlen(result));
    close(newsockfd);

    // Remove o arquivo temporário
    remove(filename);
    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <porta>\n", argv[0]);
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Servidor Prolog ouvindo na porta %d...\n", portno);

    while (1) {
        int *newsockfd = malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (*newsockfd < 0) error("ERROR on accept");
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, newsockfd);
        pthread_detach(tid);
    }
    close(sockfd);
    return 0;
}