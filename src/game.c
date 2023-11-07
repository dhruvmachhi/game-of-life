#include "game.h"

#include <stdbool.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_HEIGHT 20

enum Keys {
    QUIT = 113, // Q
    UP = 119, // W
    DOWN = 115, // S
    LEFT = 97, // A
    RIGHT = 100, // D
    TOGGLE = 32, // SPACE
    STEP = 98, // B
};

enum Keys previousKey;

int WIDTH = 40;
int HEIGHT;
size_t DATA_SIZE;

int playerX;
int playerY;

WINDOW *gameWindow;
bool *gameMap;
bool *gameMapCopy;

void render() {
    for (int mapX = 1; mapX < WIDTH; mapX++) {
        for (int mapY = 1; mapY < HEIGHT; mapY++) {
            if (mapY == playerY && mapX == playerX) {
                if (gameMap[mapY * WIDTH + mapX]) {
                    wattron(gameWindow, COLOR_PAIR(2));
                    mvwaddch(gameWindow, mapY, mapX, '@');
                    wattroff(gameWindow, COLOR_PAIR(2));
                } else {
                    mvwaddch(gameWindow, mapY, mapX, 'O');
                }
            } else if (gameMap[mapY * WIDTH + mapX]) {
                wattron(gameWindow, COLOR_PAIR(1));
                mvwaddch(gameWindow, mapY, mapX, '#');
                wattroff(gameWindow, COLOR_PAIR(1));
            } else {
                // or maybe for performance only do when nessecary
                mvwaddch(gameWindow, mapY, mapX, ' ');
            }
        }
    }
    wrefresh(gameWindow);
}

void fitBox() {
    // this is a mess (ignore this)
    if (COLS > MAX_HEIGHT*2 && LINES > MAX_HEIGHT) {
        HEIGHT = MAX_HEIGHT;
        WIDTH = HEIGHT*2;
        printf("MAX\n");
    } else if (COLS > LINES) {
        HEIGHT = LINES - 2;
        WIDTH = HEIGHT*2-2;
        printf("LINES\n");
    } else {
        HEIGHT = COLS/2 - 2;
        WIDTH = COLS-2;
        printf("COLS\n");
    }
    printf("WIDTH: %d\nHEIGHT: %d\n", WIDTH, HEIGHT);
    DATA_SIZE = WIDTH * HEIGHT * sizeof(bool);
}

void setupGame() {
    initscr();

    fitBox();
    start_color();

    raw();
    noecho();
    refresh();
    curs_set(0);

    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_YELLOW);

    playerX = 1;
    playerY = 1;

    gameMap = malloc(DATA_SIZE);
    memset(gameMap, false, DATA_SIZE);

    gameMapCopy = malloc(DATA_SIZE);

    gameWindow = newwin(HEIGHT + 1, WIDTH + 1, (LINES - HEIGHT + 1) / 2, (COLS - WIDTH + 1) / 2);

    box(gameWindow, 0, 0);

    render();
}

int gameRunning() {
    return (previousKey != QUIT);
}

bool isValid(int x, int y) {
    return x < WIDTH && y < HEIGHT && x > 0 && y > 0;
}

void movePosition(int changeX, int changeY) {
    if (!isValid(changeX + playerX, changeY + playerY)) return;
    playerX += changeX;
    playerY += changeY;
    render();
}

bool getPosition(int x, int y) {
    if (isValid(x, y)) {
        return gameMap[y * WIDTH + x];
    }
    return false;
}

void setPosition(int x, int y, bool value) {
    gameMap[y * WIDTH + x] = value;
}

void setPositionCopy(int x, int y, bool value) {
    gameMapCopy[y * WIDTH + x] = value;
}

int countNeighbors(int x, int y) {
    int temp = 0;
    for (int xOffset = -1; xOffset <= 1; xOffset++) {
        for (int yOffset = -1; yOffset <= 1; yOffset++) {
            if (xOffset == 0 && yOffset == 0) continue;
            temp += getPosition(x + xOffset, y + yOffset);    
        }
    }
    return temp;
}

void step() {
    memcpy(gameMapCopy, gameMap, DATA_SIZE);
    for (int mapX = 1; mapX < WIDTH; mapX++) {
        for (int mapY = 1; mapY < HEIGHT; mapY++) {
            int neighborCount = countNeighbors(mapX, mapY);
            if (neighborCount > 3 || neighborCount < 2) {
                setPositionCopy(mapX, mapY, false);
            } else if (neighborCount == 3) {
                setPositionCopy(mapX, mapY, true);
            }
        }
    }
    memcpy(gameMap, gameMapCopy, DATA_SIZE);
    render();
}

void toggle(int x, int y) {
    setPosition(x, y, !getPosition(x, y));
    render();
}

void handleInput() { // change to step and do the sequence with doubl buffering and clean up code
    previousKey = getch();
    switch (previousKey) {
    case UP:
        movePosition(0, -1);
        break;
    case DOWN:
        movePosition(0, 1);
        break;
    case LEFT:
        movePosition(-1, 0);
        break;
    case RIGHT:
        movePosition(1, 0);
        break;
    case TOGGLE:
        toggle(playerX, playerY);
        break;
    case STEP:
        step();
        break;
    case QUIT:
        break;
    }
}

void endGame() {
    delwin(gameWindow);
    endwin();
}