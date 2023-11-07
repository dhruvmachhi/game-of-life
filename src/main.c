#include "game.h"

int main() {
    //  refresh();

    // new
    setupGame();
    while (gameRunning()) {
        handleInput();
    }
    endGame();

    return 0;
}