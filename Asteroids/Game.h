#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <stdlib.h>
#include <time.h>

//Define the structure for the ship the player will control
struct Ship {
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::CircleShape body;
};

//Ship Weapon, fired with Spacebar
struct Blast {
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::CircleShape body;
	//Blast lasts for 2 seconds, locked to 60fps
	int lifespan;
	Blast *next;
	Blast* prev;
};

//Obstacles
struct Asteroid {
	sf::Vector2f position;
	sf::Vector2f velocity;
	float size;
	sf::CircleShape body;
	Asteroid *next;
};

class Game
{
private:
	sf::VideoMode video_mode;
	sf::RenderWindow *window;
	int WINDOW_WIDTH = 1920;
	int WINDOW_HEIGHT = 1080;
	sf::Event event;

	bool running;

	Ship player;
	bool player_rotating_left = false;
	bool player_rotating_right = false;
	bool player_moving_forward = false;

	//Linked List containing blasts
	const float blast_size = 1.f;
	const int blast_lifespan = 80;
	Blast *blast_ptr;
	Blast *blast_end_ptr;
	Blast *current_blast;
	Blast *new_blast;
	Blast *dead_blast;
	bool blaster_locked = false;

	//Linked List containing asteroids
	Asteroid *asteroid_ptr;
	Asteroid *current_asteroid;
	Asteroid *new_asteroid;
	Asteroid *dead_asteroid;

	void initVariables();
	void initWindow();

	void createBlast();
	void createAsteroid();


	void updatePlayer();
	void updateBlasts();
	void updateAsteroids();

	//Collision functions
	void checkAsteroidCollision();
	void breakBlast();

public:
	Game();
	~Game();

	bool isRunning();

	void pollEvents();

	void update();

	void render();
};

