#ifndef GAME_H
#define GAME_H

#define DEFAULT_HEALTH 100
#define DEFAULT_STRENGTH 10
#define DEFAULT_ROOM 5

typedef struct {
    char name[30];
    int damage;
    int heal;
} Item;

typedef struct {
    char name[30];
    int health;
    int damage;
} Obstacle;

typedef struct {
    char name[20];
    int health;
    int strength;
    Item *inventoryCap;
    int room;
} Player;

typedef struct {
    int number;
    Obstacle o;
    Item reward;
} Room;

void move(Player *p, Room **rooms);
void look(Player *p, Room **rooms);
void pickUp(Player *p, Room **rooms);
void attack(Player *p, Room **rooms);
Player createPlayer(char name[]);
void saveGame(Player *p, Room **rooms);
void loadGame(Player *p, Room **rooms);
void exitGame(Player *p, Room **rooms);
void printMenu(Player *p, Room **rooms);
void gameStart();
void allocateRooms(Room ***rooms);
void freeRooms(Room **rooms);

#endif // GAME_H

