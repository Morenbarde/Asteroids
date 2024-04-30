#include "Game.h"
#include <iostream>

//Pi constant used for calculations
float PI = 3.141592653;

void Game::initVariables()
{
	std::srand(time(NULL));
	player.position.x = WINDOW_WIDTH/2;
	player.position.y = WINDOW_HEIGHT/2;
	player.velocity.x = 0;
	player.velocity.y = 0;
	player.body = sf::CircleShape(10.f, 3);
	player.body.setOrigin(player.body.getRadius(), player.body.getRadius()); // Will need to change when changed to sprite

	blast_ptr = NULL;
	blast_end_ptr = blast_ptr;
	asteroid_ptr = NULL;
}

void Game::initWindow()
{
	video_mode.width = WINDOW_WIDTH;
	video_mode.height = WINDOW_HEIGHT;
	window = new sf::RenderWindow(video_mode, "Asteroids", sf::Style::Fullscreen);
	window->setFramerateLimit(60);
}


//Constant speed for the blasts
float BLAST_SPEED = 20;

void Game::createBlast()
{
	new_blast = new Blast();
	new_blast->body = sf::CircleShape(blast_size);
	new_blast->position = player.position;
	new_blast->lifespan = blast_lifespan;

	//Since blast speed is constant, can normalize and set constat x and y velocity values
	new_blast->velocity.x = BLAST_SPEED * sin(player.body.getRotation() * (PI / 180));
	new_blast->velocity.y = BLAST_SPEED * cos(player.body.getRotation() * (PI / 180));

	new_blast->body.setOrigin(new_blast->body.getRadius(), new_blast->body.getRadius());
	new_blast->body.setPosition(new_blast->position);

	//Make new blast the end of the list
	if (blast_ptr) {
		blast_end_ptr->next = new_blast;
		new_blast->prev = blast_end_ptr;
		blast_end_ptr = new_blast;
	}
	else {
		new_blast->prev = NULL;
		blast_ptr = new_blast;
		blast_end_ptr = new_blast;
	}
	new_blast->next = NULL;
}

//Constant for initial asteroid speed, may be faster when smaller
float ASTEROID_SPEED = 2.5; 

void Game::createAsteroid()
{
	new_asteroid = new Asteroid();

	new_asteroid->size = 3;
	new_asteroid->body = sf::CircleShape(40.f);

	new_asteroid->body.setFillColor(sf::Color(0, 0, 0, 0));
	new_asteroid->body.setOutlineThickness(2);
	new_asteroid->body.setOutlineColor(sf::Color(255, 255, 255));

	//randomly place asteroid near edge of screen
	if (rand() % 2) {

		new_asteroid->position.x = rand() % WINDOW_WIDTH;

		if (rand() % 2) {
			new_asteroid->position.y = rand() % 50 + 50;
		}
		else {
			new_asteroid->position.y = rand() % 50 + (WINDOW_HEIGHT - 100);
		}
	}
	else {

		new_asteroid->position.y = rand() % WINDOW_HEIGHT;

		if (rand() % 2) {
			new_asteroid->position.x = rand() % 50 + 50;
		}
		else {
			new_asteroid->position.x = rand() % 50 + (WINDOW_WIDTH - 100);
		}
	}
	//std::cout << new_asteroid->position.x << " " << new_asteroid->position.y << std::endl;

	//Gives asteroid random direction to travel
	int asteroid_angle = rand() % 360;
	new_asteroid->velocity.x = ASTEROID_SPEED * cos(asteroid_angle * (PI / 180));
	new_asteroid->velocity.y = ASTEROID_SPEED * sin(asteroid_angle * (PI / 180));

	new_asteroid->body.setOrigin(new_asteroid->body.getRadius(), new_asteroid->body.getRadius());
	new_asteroid->body.setPosition(new_asteroid->position);

	//store asteroids in linked list
	if (asteroid_ptr) {
		new_asteroid->next = asteroid_ptr;
		asteroid_ptr = new_asteroid;
	}
	else {
		new_asteroid->next = NULL;
		asteroid_ptr = new_asteroid;
	}
}

void Game::endGame() {
	running = false;
	if (window->isOpen()) window->close();

}

//Update Functions


float ACCELERATION_CONST = .2;
float DECCELERATION_CONST = .98;

void Game::updatePlayer()
{
	//Rotates the player if the keys have been pressed and not released
	if (player_rotating_right) player.body.rotate(2.f);
	if (player_rotating_left) player.body.rotate(-2.f);

	//Gets the angle of the player in radians
	float player_rotation = player.body.getRotation();

	//Moves the player if the key has been pressed and not released
	if (player_moving_forward) {
		//Normalizes the acceleration and adds to both the x and y directiong
		player.velocity.x += ACCELERATION_CONST * sin(player_rotation * (PI / 180));
		player.velocity.y += ACCELERATION_CONST * cos(player_rotation * (PI / 180));
	}
	else {
		player.velocity.x *= DECCELERATION_CONST;
		player.velocity.y *= DECCELERATION_CONST;
	}

	//Adds velocity to position, y negative due to sfml window having y positive downwards
	player.position.x += player.velocity.x;
	player.position.y -= player.velocity.y;

	//Loops player around the window upon reaching the edge
	if (player.position.x > WINDOW_WIDTH) player.position.x -= WINDOW_WIDTH;
	else if (player.position.x < 0) player.position.x += WINDOW_WIDTH;

	if (player.position.y > WINDOW_HEIGHT) player.position.y -= WINDOW_HEIGHT;
	else if (player.position.y < 0) player.position.y += WINDOW_HEIGHT;

	player.body.setPosition(player.position);
}

//TODO add asteroid collission
void Game::updateBlasts()
{
	if (blast_ptr && !blast_ptr->lifespan) {
		current_blast = blast_ptr->next;
		delete blast_ptr;
		blast_ptr = current_blast;
	}
	current_blast = blast_ptr;

	while (current_blast) {
		//Adds velocity to position, y negative due to sfml window having y positive downwards
		current_blast->position.x += current_blast->velocity.x;
		current_blast->position.y -= current_blast->velocity.y;

		if (current_blast->position.x > WINDOW_WIDTH) current_blast->position.x -= WINDOW_WIDTH;
		else if (current_blast->position.x < 0) current_blast->position.x += WINDOW_WIDTH;

		if (current_blast->position.y > WINDOW_HEIGHT) current_blast->position.y -= WINDOW_HEIGHT;
		else if (current_blast->position.y < 0) current_blast->position.y += WINDOW_HEIGHT;

		current_blast->body.setPosition(current_blast->position);

		current_blast->lifespan -= 1;

		current_blast = current_blast->next;
	}
}

void Game::updateAsteroids()
{
	current_asteroid = asteroid_ptr;

	while (current_asteroid) {
		//Adds velocity to position, y negative due to sfml window having y positive downwards
		current_asteroid->position.x += current_asteroid->velocity.x;
		current_asteroid->position.y -= current_asteroid->velocity.y;

		if (current_asteroid->position.x > WINDOW_WIDTH) current_asteroid->position.x -= WINDOW_WIDTH;
		else if (current_asteroid->position.x < 0) current_asteroid->position.x += WINDOW_WIDTH;

		if (current_asteroid->position.y > WINDOW_HEIGHT) current_asteroid->position.y -= WINDOW_HEIGHT;
		else if (current_asteroid->position.y < 0) current_asteroid->position.y += WINDOW_HEIGHT;

		current_asteroid->body.setPosition(current_asteroid->position);

		current_asteroid = current_asteroid->next;
	}
}

void Game::checkAsteroidCollision()
{
	//Asteroid-Blast Collision
	Blast* prev_blast = NULL;
	current_blast = blast_ptr;
	bool blast_deleted = false;

	while (current_blast) {
		Asteroid* prev_asteroid = NULL;
		current_asteroid = asteroid_ptr;
		blast_deleted = false;
		while (current_asteroid && !blast_deleted) {
			if (current_asteroid->body.getGlobalBounds().intersects(current_blast->body.getGlobalBounds())) {
				std::cout << "Struct Pos:" << current_asteroid->position.x << ", " << current_asteroid->position.y << std::endl;
				//Removes Asteroid from linked list
				if (current_asteroid == asteroid_ptr) {
					asteroid_ptr = current_asteroid->next;
					delete current_asteroid;
					current_asteroid = asteroid_ptr;
				}
				else {
					prev_asteroid->next = current_asteroid->next;
					delete current_asteroid;
					current_asteroid = prev_asteroid->next;
				}
				//Removes Blast from linked list
				if (current_blast == blast_ptr) {
					blast_ptr = current_blast->next;
					delete current_blast;
					current_blast = blast_ptr;
				}
				else {
					prev_blast->next = current_blast->next;
					delete current_blast;
					current_blast = prev_blast->next;
				}
				blast_deleted = true;
			}
			else {
				prev_asteroid = current_asteroid;
				current_asteroid = current_asteroid->next;
			}

		}
		if (!blast_deleted) {
			prev_blast = current_blast;
			current_blast = current_blast->next;
		}
	}
}

void Game::checkPlayerCollision()
{
	current_asteroid = asteroid_ptr;
	while (current_asteroid) {
		if (player.body.getGlobalBounds().intersects(current_asteroid->body.getGlobalBounds())) {
			endGame();
		}
		current_asteroid = current_asteroid->next;
	}
}

void Game::breakBlast()
{
	//Removes Asteroid from linked list
	if (!current_blast->prev && !current_blast->next) blast_ptr = NULL;
	else if (current_blast->prev && current_blast->next) current_blast->prev->next = current_blast->next;
	else if (current_blast->prev) current_blast->prev->next = NULL;
	else if (current_blast->next) blast_ptr = current_blast->next;
}

Game::Game()
{
	running = true;
	initVariables();
	initWindow();
}

Game::~Game()
{
	running = false;
	window->close();
}

bool Game::isRunning()
{
	return running;
}

void Game::pollEvents()
{
	while (window->pollEvent(event)) {
		switch (event.type) {

		case sf::Event::Closed:
			endGame();
			break;

		case sf::Event::KeyPressed:

			switch (event.key.code) {

			//Escape Key to Close the Game
			case sf::Keyboard::Escape:
				endGame();

			//Rotate Counterclockwise
			case sf::Keyboard::Left:
			case sf::Keyboard::A:
				player_rotating_left = true;
				break;

			//Rotate Clockwise
			case sf::Keyboard::Right:
			case sf::Keyboard::D:
				player_rotating_right = true;
				break;

			//Move Forward
			case sf::Keyboard::Up:
			case sf::Keyboard::W:
				player_moving_forward = true;
				break;

			//Fire Blast
			case sf::Keyboard::Space:
				//restrict fire rate to 1 blast per button press
				if (!blaster_locked) {
					createBlast();
					blaster_locked = true;
				}
				break;

			//Create Asteroid For Testing
			case sf::Keyboard::Z:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) createAsteroid();

			default:
				break;
			}
			break;

		case sf::Event::KeyReleased:

			switch (event.key.code) {

			//Keeps Rotating until the key is released
			case sf::Keyboard::Left:
			case sf::Keyboard::A:
				player_rotating_left = false;
				break;

			case sf::Keyboard::Right:
			case sf::Keyboard::D:
				player_rotating_right = false;
				break;

			//Keeps accelerating until the key is released
			case sf::Keyboard::Up:
			case sf::Keyboard::W:
				player_moving_forward = false;
				break;

			case sf::Keyboard::Space:
				blaster_locked = false;
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}

void Game::update()
{
	updatePlayer();
	updateBlasts();
	updateAsteroids();

	checkAsteroidCollision();
	checkPlayerCollision();
}

void Game::render()
{
	window->clear();
	window->draw(player.body);

	current_blast = blast_ptr;
	while (current_blast) {
		window->draw(current_blast->body);
		current_blast = current_blast->next;
	}

	current_asteroid = asteroid_ptr;
	while (current_asteroid) {
		window->draw(current_asteroid->body);
		current_asteroid = current_asteroid->next;
	}
	window->display();
}
