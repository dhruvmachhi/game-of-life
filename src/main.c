#include "game.h"

int main() {
    setupGame();
    
    while (gameRunning()) {
        handleInput();
    }
    endGame();

    return 0;
}
