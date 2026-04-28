#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "arena.h"

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    while (1) {
        printf("\n=== ETERNAL MENU ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Choice: ");
        int choice;
        scanf("%d", &choice);
        if (choice == 3) {
            printf("Keluar...\n");
            break;
        }
        sock = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (choice == 1) {
            strcpy(buffer, "REGISTER");
        } else if (choice == 2) {
            strcpy(buffer, "LOGIN");
        }
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        printf("Dari server: %s\n", buffer);
        close(sock);
    }
    return 0;
}
