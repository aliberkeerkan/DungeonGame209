#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "game.h"

#define DEFAULT_HEALTH 100
#define DEFAULT_STRENGTH 10
#define DEFAULT_ROOM 5

// Items
Item knife = {"Knife", 20, 5};
Item shield = {"Shield", 5, 30};
Item cloak = {"Cloak", 40, 10};
Item aidKit = {"Aid Kit", 10, 50};
Item trove = {"Trove", 0, 0};
Item helmet = {"Helmet", 15, 45};
Item armor = {"Armor", 15, 60};
Item gun = {"Gun", 45, 10};
Item nothingI = {"NothingI", 0, 0};

// Obstacles
Obstacle skeletons = {"Skeletons", 30, 5};
Obstacle vampire = {"Vampires", 35, 10};
Obstacle wizard = {"Wizard", 50, 15};
Obstacle ghost = {"Ghost", 55, 20};
Obstacle morbivore = {"Morbivore", 80, 25};
Obstacle golem = {"Golem", 65, 12};
Obstacle archer = {"Archer", 60, 8};
Obstacle giant = {"Giant", 70, 20};
Obstacle nothingO = {"NothingO", 0, 0};



void allocateRooms(Room ***rooms) {
    *rooms = (Room **)malloc(3 * sizeof(Room *));
    for (int i = 0; i < 3; i++) {
        (*rooms)[i] = (Room *)malloc(3 * sizeof(Room));
    }
}

void freeRooms(Room **rooms) {
    for (int i = 0; i < 3; i++) {
        free(rooms[i]);
    }
    free(rooms);
}

void listSavedGames() {
    struct dirent *entry;
    DIR *dp = opendir(".");

    if (dp == NULL) {
        printf("Could not open the directory to list saved games.\n");
        return;
    }

    printf("Saved games:\n");
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, "_savegame.txt")) {
            printf("- %s\n", entry->d_name);
        }
    }

    closedir(dp);
}

void inventory(Player *p) {
    printf("Your inventory:\n");
    for (int i = 0; i < 9; i++) {
        if (p->inventoryCap[i].name[0] != '\0') {
            printf("- %s (Damage: %d, Heal: %d)\n", p->inventoryCap[i].name, p->inventoryCap[i].damage, p->inventoryCap[i].heal);
        }
    }
}

void move(Player *p, Room **rooms) {
    int newRoomNo;

    while (1) {
        char direction;
        printf("Enter the direction you want to go (d)own, (u)p, (l)eft, (r)ight: \n");
        scanf(" %c", &direction);

        switch (direction) {
            case 'u':
                newRoomNo = p->room - 3;
                break;
            case 'd':
                newRoomNo = p->room + 3;
                break;
            case 'l':
                newRoomNo = p->room - 1;
                break;
            case 'r':
                newRoomNo = p->room + 1;
                break;
            default:
                printf("Invalid direction. (d)own, (u)p, (l)eft, (r)ight: \n");
                continue;
        }

        if (newRoomNo < 1 || newRoomNo > 9) {
            printf("Invalid room number. Please choose a valid direction.\n");
            continue;
        }

        p->room = newRoomNo;
        printf("You moved to room %d.\n", newRoomNo);
        break;
    }
}

void look(Player *p, Room **rooms) {
    int newRoomNo;
    printf("Enter the number of the room you want to look: \n");
    scanf("%d", &newRoomNo);

    if (newRoomNo < 1 || newRoomNo > 9) {
        printf("Invalid room number.\n");
        return;
    }

    int row = (newRoomNo - 1) / 3;
    int col = (newRoomNo - 1) % 3;

    printf("Obstacle: %s, Reward: %s\n", rooms[row][col].o.name, rooms[row][col].reward.name);
}

void pickUp(Player *p, Room **rooms) {
    int row = (p->room - 1) / 3;
    int col = (p->room - 1) % 3;

    p->inventoryCap[p->room - 1] = rooms[row][col].reward;
    p->health += rooms[row][col].reward.heal;
    p->strength += rooms[row][col].reward.damage;

    printf("Item picked up: %s\n", rooms[row][col].reward.name);
}

void attack(Player *p, Room **rooms) {
    int row = (p->room - 1) / 3;
    int col = (p->room - 1) % 3;

    if (rooms[row][col].o.health > 0) {
        char choice;
        do {
            printf("Press (h) to hit or (e) to escape:\n");
            scanf(" %c", &choice);
            while (choice != 'h' && choice != 'e') {
                printf("Invalid choice! Press (h) to hit or (e) to escape:\n");
                scanf(" %c", &choice);
            }

            if (choice == 'h') {
                p->health -= rooms[row][col].o.damage;
                rooms[row][col].o.health -= p->strength;

                printf("You attacked! Your health: %d, Enemy health: %d\n", p->health, rooms[row][col].o.health);
            }
        } while (choice == 'h' && p->health > 0 && rooms[row][col].o.health > 0);

        if (choice == 'e') {
            printf("You escaped!\n");
            return;
        }
    } else {
        printf("There is no obstacle in this room.\n");
    }

    if (p->health <= 0) {
        printf("You are defeated!\n");
        exitGame(p, rooms);
    } else if (rooms[row][col].o.health <= 0) {
        printf("Enemy defeated! Now you can move to the next room.\n");
        pickUp(p, rooms);
    }
}

Player createPlayer(char name[]) {
    Player p;
    strcpy(p.name, name);
    p.health = DEFAULT_HEALTH;
    p.strength = DEFAULT_STRENGTH;
    p.room = DEFAULT_ROOM;
    p.inventoryCap = (Item *)malloc(9 * sizeof(Item));
    for (int i = 0; i < 9; i++) {
        p.inventoryCap[i].name[0] = '\0';
    }
    return p;
}

void saveGame(Player *p, Room **rooms) {
    char filename[50];
    sprintf(filename, "%s_savegame.txt", p->name);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error saving the game.\n");
        return;
    }

    // save the player datas
    fprintf(file, "%s\n%d\n%d\n%d\n", p->name, p->health, p->strength, p->room);
    
    for (int i = 0; i < 9; i++) { 
        fprintf(file, "%s\n%d\n%d\n", p->inventoryCap[i].name, p->inventoryCap[i].damage, p->inventoryCap[i].heal); 
    }

    // save the room datas
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            fprintf(file, "%d\n", rooms[i][j].o.health);
        }
    }

    fclose(file);
    printf("Game saved successfully!\n");
}

void loadGame(Player *p, Room **rooms) {
    char filename[50];
    sprintf(filename, "%s_savegame.txt", p->name);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No saved game found for %s. Starting a new game.\n", p->name);
        return;
    }

    // load the player datas
    fscanf(file, "%s\n%d\n%d\n%d\n", p->name, &p->health, &p->strength, &p->room);
    
    for (int i = 0; i < 9; i++) { 
        fscanf(file, "%s\n%d\n%d\n", p->inventoryCap[i].name, &p->inventoryCap[i].damage, &p->inventoryCap[i].heal); 
    }

        // load the room datas
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            fscanf(file, "%d\n", &rooms[i][j].o.health);
            if (rooms[i][j].o.health <= 0) {
                rooms[i][j].o.health = 0; 
            }
        }
    }

    fclose(file);
    printf("Game loaded successfully for %s!\n", p->name);
}

void exitGame(Player *p, Room **rooms) {
    char choice;
    printf("Would you like to save your game before exiting? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {
        saveGame(p, rooms);
    }

    // Freeing the rooms array and inventoryCap
    free(p->inventoryCap);
    freeRooms(rooms);
    printf("Exiting the game. Goodbye!\n");
    exit(0);
}

void printMenu(Player *p, Room **rooms) {
    int choice;

    while (1) {
        printf("------ MENU ------\n");
        printf("1- Move\n");
        printf("2- Attack\n");
        printf("3- Look\n");
        printf("4- Save the game\n");
        printf("5- Exit\n");

        if (scanf("%d", &choice) != 1) {
            
            while (getchar() != '\n');
            printf("Invalid choice. Please enter a number between 1 and 5.\n");
            continue;
        }

        if (choice >= 1 && choice <= 5) {
            break;  
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    switch (choice) {
        case 1:
            move(p, rooms);
            break;
        case 2:
            attack(p, rooms);
            break;
        case 3:
            look(p, rooms);
            break;
        case 4:
            saveGame(p, rooms);
            break;
        case 5:
            exitGame(p, rooms);
            break;
    }
}

void gameStart() {
    Player player1;
    Room **rooms;
    allocateRooms(&rooms);

    // Filling room with the obstacles and the items
    rooms[0][0] = (Room){1, skeletons, knife};
    rooms[0][1] = (Room){2, vampire, shield};
    rooms[0][2] = (Room){3, wizard, cloak};
    rooms[1][0] = (Room){4, ghost, aidKit};
    rooms[1][1] = (Room){5, nothingO, nothingI};
    rooms[1][2] = (Room){6, morbivore, trove};
    rooms[2][0] = (Room){7, golem, helmet};
    rooms[2][1] = (Room){8, archer, armor};
    rooms[2][2] = (Room){9, giant, gun};

    printf("----------WELCOME TO THE DUNGEON GAME----------\n");

    printf("Would you like to load your saved game? (y/n): ");
    char choice;
    scanf(" %c", &choice);
    getchar();  // Clear newline

    if (choice == 'y' || choice == 'Y') {
        printf("Enter your name: ");
        char name[20];
        scanf("%s", name);
        getchar();
        player1 = createPlayer(name);
        loadGame(&player1, rooms);
    } else {
        printf("Enter your name: ");
        char name[20];
        scanf("%s", name);
        getchar();
        player1 = createPlayer(name);
        printf("Starting a new game for %s.\n", name);
    }
    
    // Print room details to help player to find correct way to win the game
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("Room number: %d, Obstacle: %s, Obstacle's Health: %d, Obstacle's Damage: %d\n",
                   rooms[i][j].number, rooms[i][j].o.name, rooms[i][j].o.health, rooms[i][j].o.damage);
            printf("Reward in the room: %s, Reward's Heal: %d, Reward's Damage: %d\n",
                   rooms[i][j].reward.name, rooms[i][j].reward.heal, rooms[i][j].reward.damage);
            printf("\n");
        }
        printf("\n");
    }

    printf("!!You have to find the correct path to win the game, good luck!!\n");

    while (1) {
        printMenu(&player1, rooms);
    }
}

int main(int argc, char *argv[]) {
    gameStart();
    return 0;
}
