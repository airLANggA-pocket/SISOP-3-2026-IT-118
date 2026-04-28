#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "protocol.h"

int sock;
void *receive_msg() {
    char buffer[BUFFER_SIZE];
    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}
int main() {
    struct sockaddr_in serv_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    char name[50];
    printf("Enter your name: ");
    scanf("%s", name);
    send(sock, name, strlen(name), 0);
    pthread_t tid;
    pthread_create(&tid, NULL, receive_msg, NULL);
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
        if (strncmp(buffer, "/exit", 5) == 0) {
            break;
        }
    }
    close(sock);
    return 0;
}
