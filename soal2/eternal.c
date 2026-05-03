#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include "arena.h"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"

SharedData *data;
int login_idx = -1;

void hp_bar(int hp) {
    int total = 20;
    int fill = (hp * total) / 100;
    printf("[");
    for(int i=0;i<total;i++){
        if(i<fill) printf("█");
        else printf(" ");
    }
    printf("] %d\n", hp);
}
void profile(Player p){
    printf(CYAN "\n====== PROFILE ======\n" RESET);
    printf("Name  : %s\n", p.username);
    printf("Lv    : %d\n", p.level);
    printf("Gold  : %d\n", p.gold);
    printf("XP    : %d\n", p.xp);
}
void register_user(){
    Player p;
    printf("Username: ");
    scanf("%s", p.username);
    pthread_mutex_lock(&data->lock);
    for(int i=0;i<data->count;i++){
        if(strcmp(data->players[i].username,p.username)==0){
            printf("Username sudah ada!\n");
            pthread_mutex_unlock(&data->lock);
            return;
        }
    }
    printf("Password: ");
    scanf("%s", p.password);
    p.level = 1;
    p.gold = 150;
    p.xp = 0;
    p.base_damage = 10;
    p.weapon_damage = 0;
    p.health = 100;
    p.active = 0;
    p.in_battle = 0;
    p.hcount = 0;
    data->players[data->count++] = p;
    pthread_mutex_unlock(&data->lock);
    printf(GREEN "Register berhasil!\n" RESET);
}
void login_user(){
    char u[50], p[50];
    printf("Username: ");
    scanf("%s", u);
    printf("Password: ");
    scanf("%s", p);

    for(int i=0;i<data->count;i++){
        if(strcmp(data->players[i].username,u)==0 &&
           strcmp(data->players[i].password,p)==0){
            login_idx = i;
            data->players[i].active = 1;
            printf(GREEN "Login berhasil!\n" RESET);
            return;
        }
    }
    printf(RED "Login gagal!\n" RESET);
}
void armory(){
    Player *p = &data->players[login_idx];
    printf("\n===== ARMORY =====\n");
    printf("1. Wood Sword (+10) - 100\n");
    printf("2. Iron Sword (+15) - 200\n");
    printf("3. Steel Axe (+30) - 300\n");
    printf("0. Back\n");
    int c;
    scanf("%d",&c);
    if(c==1 && p->gold>=100){ p->weapon_damage=10; p->gold-=100;}
    else if(c==2 && p->gold>=200){ p->weapon_damage=15; p->gold-=200;}
    else if(c==3 && p->gold>=300){ p->weapon_damage=30; p->gold-=300;}
}
void show_history(){
    Player p = data->players[login_idx];
    printf("\n===== HISTORY =====\n");
    for(int i=0;i<p.hcount;i++){
        printf("%s - %s +%d XP\n",
            p.history[i].opponent,
            p.history[i].result,
            p.history[i].xp);
    }
}
void battle(){
    Player *p = &data->players[login_idx];
    int enemy_hp = 100;
    int player_hp = 100;
    int total_dmg = p->base_damage + p->weapon_damage;

    while(player_hp>0 && enemy_hp>0){
        system("clear");
        printf("\n===== ARENA =====\n");
        printf("%s\n", p->username);
        hp_bar(player_hp);
        printf("\nVS\n\n");
        printf("Enemy\n");
        hp_bar(enemy_hp);
        printf("\n[A] Attack  [U] Ultimate\n");
        char cmd;
        scanf(" %c",&cmd);

        if(cmd=='a' || cmd=='A'){
            enemy_hp -= total_dmg;
        }
        else if(cmd=='u' || cmd=='U'){
            enemy_hp -= total_dmg*3;
        }
        if(enemy_hp<=0) break;
        player_hp -= 8;
        sleep(1);
    }
    int win = player_hp>0;

    if(win){
        printf(GREEN "\nVICTORY\n" RESET);
        p->xp += 50;
        p->gold += 120;
        strcpy(p->history[p->hcount].result,"WIN");
        p->history[p->hcount].xp = 50;
    }else{
        printf(RED "\nDEFEAT\n" RESET);
        p->xp += 15;
        p->gold += 30;
        strcpy(p->history[p->hcount].result,"LOSS");
        p->history[p->hcount].xp = 15;
    }
    strcpy(p->history[p->hcount].opponent,"musuko");
    p->hcount++;
    if(p->xp>=100){
        p->level++;
        p->xp = 0;
    }
    printf("Press ENTER...");
    getchar(); getchar();
}

int main(){
    key_t key = 1234;
    int shmid = shmget(key,sizeof(SharedData),0666);
    data = (SharedData*) shmat(shmid,NULL,0);
    int c;

    while(1){
        printf("\n1.Register\n2.Login\n3.Exit\nChoice: ");
        scanf("%d",&c);
        if(c==1) register_user();
        else if(c==2){
            login_user();
            if(login_idx!=-1){
                int m;
                while(1){
                    profile(data->players[login_idx]);
                    printf("\n1.Battle\n2.Armory\n3.History\n4.Logout\nChoice: ");
                    scanf("%d",&m);
                    if(m==1) battle();
                    else if(m==2) armory();
                    else if(m==3) show_history();
                    else break;
                }
            }
        }
        else break;
    }
}
