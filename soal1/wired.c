#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
char names[MAX_CLIENTS][50];
int total_clients = 0;
time_t start_time;

void log_message(char *type, char *msg);

void broadcast(char *msg, int sender);

void *handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    char name[50];
    recv(sock, name, sizeof(name), 0);
    int idx = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == 0) {
            int dup = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) {
                if (strcmp(names[j], name) == 0) dup = 1;
            }
            if (dup) {
                send(sock, "[System] Name used\n", 20, 0);
                close(sock);
                return NULL;
            }
            clients[i] = sock;
            strcpy(names[i], name);
            idx = i;
            total_clients++;
            break;
        }
    }
    if (strcmp(name, "The_Knights") == 0) {
        send(sock, "[Admin Mode]\n", 13, 0);
        while (1) {
            char menu[] =
            "\n1.Users\n2.Uptime\n3.Shutdown\n4.Exit\n>> ";
            send(sock, menu, strlen(menu), 0);
            int len = recv(sock, buffer, BUFFER_SIZE, 0);
            if (len <= 0) break;
            int cmd = atoi(buffer);

            if (cmd == 1) {
                char out[50];
                sprintf(out, "Users: %d\n", total_clients);
                send(sock, out, strlen(out), 0);
                log_message("Admin", "GET_USERS");
            }
            else if (cmd == 2) {
                int up = time(NULL) - start_time;
                char out[50];
                sprintf(out, "Uptime: %d\n", up);
                send(sock, out, strlen(out), 0);
                log_message("Admin", "GET_UPTIME");
            }
            else if (cmd == 3) {
                log_message("Admin", "SHUTDOWN");
                exit(0);
            }
            else break;
        }
        close(sock);
        return NULL;
    }
    char join[100];
    sprintf(join, "User '%s' connected", name);
    log_message("System", join);
    broadcast(join, sock);

    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) break;
        buffer[len] = 0;
        if (strcmp(buffer, "/exit") == 0) break;
        char msg[1100];
        sprintf(msg, "[%s]: %s", name, buffer);
        log_message("User", msg);
        broadcast(msg, sock);
    }
    char leave[100];
    sprintf(leave, "User '%s' disconnected", name);
    log_message("System", leave);
    clients[idx] = 0;
    total_clients--;
    close(sock);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    start_time = time(NULL);
    log_message("System", "SERVER ONLINE");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);
    printf("Server berjalan...\n");

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr*)&addr, (socklen_t*)&addrlen);
        pthread_t t;
        int *p = malloc(sizeof(int));
        *p = new_socket;
        pthread_create(&t, NULL, handle_client, p);
    }
}
