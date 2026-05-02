#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define PORT 8080
#define BUFFER_SIZE 1024

int sock;

void *receive_msg(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) break;
        buffer[len] = 0;
        printf("%s\n", buffer);
    }
    return NULL;
}
int main() {
    struct sockaddr_in serv;
    char buffer[BUFFER_SIZE];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr*)&serv, sizeof(serv));
    printf("Enter your name: ");
    fgets(buffer, 50, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send(sock, buffer, strlen(buffer), 0);
    pthread_t t;
    pthread_create(&t, NULL, receive_msg, NULL);

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(sock, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) break;
    }
    close(sock);
}
