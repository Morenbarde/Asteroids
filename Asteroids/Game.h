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
	sf::Sprite body;
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
	int level;
	float rotate_speed;
	sf::CircleShape body;
	Asteroid *next;
};

struct Star {
	sf::CircleShape body;
	Star* next;
};

class Game
{
private:
	//Pi constant used for calculations
	float PI = 3.141592653;

	//Constant speed for the blasts
	float BLAST_SPEED = 20;

	//Asteroid constants
	float ASTEROID_SPEED_1 = 2.0;
	float ASTEROID_SPEED_2 = 4.0;
	float ASTEROID_SPEED_3 = 6.0;

	float ASTEROID_SIZE_1 = 40.f;
	float ASTEROID_SIZE_2 = 25.f;
	float ASTEROID_SIZE_3 = 15.f;

	int MAX_ROTATION = 5;

	//Player Constants
	float ACCELERATION_CONST = .2;
	float DECCELERATION_CONST = .98;

	sf::VideoMode video_mode;
	sf::RenderWindow *window;
	int WINDOW_WIDTH = 1920;
	int WINDOW_HEIGHT = 1080;
	sf::Event event;

	sf::Music music;

	bool running;

	bool game_is_over;
	sf::Text gameEndedText;
	sf::Font font;

	bool between_levels;
	bool end_level;
	bool level_active;
	sf::Text levelText;
	sf::Text endLevelText;

	int current_level;

	int score;
	sf::Text scoreText;

	Ship player;
	sf::Texture ship_texture;
	int SPRITE_WIDTH = 25;
	int SPRITE_HEIGHT = 25;
	int Texture_pos = 0;
	bool player_rotating_left = false;
	bool player_rotating_right = false;
	bool player_moving_forward = false;
	sf::SoundBuffer ship_explosion_sound_buffer;
	sf::Sound ship_explosion_sound;

	//Linked List containing blasts
	const float blast_size = 2.f;
	const int blast_lifespan = 80;
	Blast *blast_ptr;
	Blast *blast_end_ptr;
	bool blaster_locked = false;
	sf::SoundBuffer blast_sound_buffer;
	sf::Sound blast_sound;

	//Linked List containing asteroids
	Asteroid *asteroid_ptr;
	sf::Texture asteroid_texture;
	sf::SoundBuffer asteroid_explosion_sound_buffer;
	sf::Sound asteroid_explosion_sound;

	Star* star_ptr;
	int STAR_COUNT = 1000;

	void initVariables();
	void initStars();
	void initWindow();

	void createBlast();
	void createAsteroid();
	void createAsteroid(float x_pos, float y_pos, int size);
	void splitAsteroid(Asteroid* current_asteroid);

	void initiateLevel();
	void endGame();


	void updatePlayer();
	void updateBlasts();
	void updateAsteroids();
	void updateScore();
	void updateLevel();

	//Collision functions
	void checkAsteroidCollision();
	void checkPlayerCollision();
	void breakBlast();

	void resetPlayer();
	void resetAsteroids();
	void resetStars();
	void resetScore();

public:
	Game();
	~Game();

	bool isRunning();

	void pollEvents();

	void update();

	void render();
};

