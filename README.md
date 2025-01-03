# Dungeon Game

## Gameplay
This is a dungeon adventure game where players navigate through a grid of rooms, encountering obstacles and collecting items. The player can move between rooms, look into rooms to see the obstacles and rewards, attack obstacles, and pick up rewards.

## Code Structure
- **main.c**: Contains the implementation of the game functions.
- **game.h**: Contains structure definitions and function prototypes.

## Game Logic
- **Move**: The player can move to any room using directional commands.
- **Looking**: The player can look into rooms to see the current obstacle and reward.
- **Attacking**: The player can attack obstacles in the current room. If successful, they can pick up the reward.
- **Saving and Loading**: The player can save their progress, storing their current health, strength, room number, and inventory. When returning, they can load their saved game to continue from where they left off.

## Compilation
To compile the game, use the provided Makefile:
```sh
# Makefile for Dungeon Game

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable
TARGET = dungeon_game

# Source files
SRCS = main.c game.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)


