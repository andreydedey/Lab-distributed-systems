#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <hostname> <porta> <num_clientes>\n", argv[0]);
        exit(1);
    }
    char *hostname = argv[1];
    char *porta = argv[2];
    int num_clients = atoi(argv[3]);
    pid_t pid;
    for (int i = 0; i < num_clients; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            execl("./client", "client", hostname, porta, (char *)NULL);
            perror("execl");
            exit(1);
        }
        usleep(10000);
    }
    for (int i = 0; i < num_clients; i++) {
        wait(NULL);
    }
    printf("Teste de stress concluído com %d clientes.\n", num_clients);
    return 0;
}