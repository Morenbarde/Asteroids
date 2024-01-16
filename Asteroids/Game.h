#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

//Define the structure for the ship the player will control
struct Ship {
	sf::Vector2f position;
	sf::Vector2f velocity;
	//Used to calculate directional acceleration
	float rotation;
	sf::CircleShape body = sf::CircleShape(10.f, 3);
};

//Ship Weapon, fired with Spacebar
struct Blast {
	sf::Vector2f position;
	float angle;
	sf::CircleShape body = sf::CircleShape(4.f);
	Blast *next = NULL;
};

//Obstacles
struct Asteroid {
	sf::Vector2f position;
	sf::Vector2f velocity;
	float rotation;
	float size;
	sf::CircleShape body;
	Asteroid *next = NULL;
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
	Blast* blast_ptr;
	Blast* current_blast;

	//Linked List containing asteroids
	Asteroid *asteroid_ptr;
	Asteroid *current_asteroid;

	void initVariables();
	void initWindow();

public:
	Game();
	~Game();

	bool isRunning();

	void pollEvents();
	void update();
	void render();
};

