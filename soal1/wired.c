#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "protocol.h"

int clients[MAX_CLIENTS];
char names[MAX_CLIENTS][50];

void log_message(char *type, char *msg) {
    FILE *f = fopen("history.log", "a");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [%s]\n",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        type,
	msg
    );
    fclose(f);
}
void broadcast(char *msg, int sender) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != 0 && clients[i] != sender) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
}
void *handle_client(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    char name[50];
    int bytes = recv(sock, name, sizeof(name) - 1, 0);
    if (bytes <= 0) {
        close(sock);
        return NULL;
    }
    name[bytes] = '\0';
    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == 0) {
            int duplicate = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) {
                if (clients[j] != 0 && strcmp(names[j], name) == 0) {
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate) {
                char *msg = "[System] Nama sudah digunakan\n";
                send(sock, msg, strlen(msg), 0);
                close(sock);
                return NULL;
            }
            clients[i] = sock;
            strcpy(names[i], name);
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        char *msg = "[System] Server penuh\n";
        send(sock, msg, strlen(msg), 0);
        close(sock);
        return NULL;
    }
    char join_msg[120];
    snprintf(join_msg, sizeof(join_msg), "[System] User '%s' connected\n", name);
    log_message("System", join_msg);
    broadcast(join_msg, sock);
    while (1) {
        int len = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (len <= 0) break;
        buffer[len] = '\0';
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        if (strncmp(buffer, "/exit", 5) == 0) {
            break;
        }
        char msg[BUFFER_SIZE + 60];
        int msg_len = snprintf(msg, sizeof(msg), "[%s]: %s\n", name, buffer);
        if (msg_len > 0 && msg_len < sizeof(msg)) {
            log_message("User", msg);
            broadcast(msg, sock);
        }
    }
    char leave_msg[120];
    snprintf(leave_msg, sizeof(leave_msg), "[System] User '%s' disconnected\n", name);
    log_message("System", leave_msg);
    broadcast(leave_msg, sock);
    clients[idx] = 0;
    memset(names[idx], 0, sizeof(names[idx]));
    close(sock);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Initialize arrays
    memset(clients, 0, sizeof(clients));
    memset(names, 0, sizeof(names));
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        return 1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }
    
    printf("Server berjalan di port %d...\n", PORT);
    
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }
        pthread_t tid;
        int *client_sock = malloc(sizeof(int));
        if (client_sock == NULL) {
            close(new_socket);
            continue;
        }
        *client_sock = new_socket;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid); 
    }
    close(server_fd);
    return 0;
}
