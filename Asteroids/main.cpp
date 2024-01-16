#include "Game.h"

int main() {
	Game game;

	while (game.isRunning()) {
		game.pollEvents();
		game.update();
		game.render();
	}
}