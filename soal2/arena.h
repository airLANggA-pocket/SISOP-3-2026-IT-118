#ifndef ARENA_H
#define ARENA_H
#include <pthread.h>
#define MAX_PLAYER 10
#define MAX_HISTORY 10

typedef struct {
    char opponent[50];
    char result[10];
    int xp;
} 
History;
typedef struct {
    char username[50];
    char password[50];
    int level;
    int gold;
    int xp;
    int base_damage;
    int weapon_damage;
    int health;
    int in_battle;
    int active;
    History history[MAX_HISTORY];
    int hcount;
} 
Player;
typedef struct {
    Player players[MAX_PLAYER];
    int count;
    int waiting_player;
    pthread_mutex_t lock;
} SharedData;
#endif