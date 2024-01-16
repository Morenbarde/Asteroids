#include "Game.h"

void Game::initVariables()
{
	player.position.x = WINDOW_WIDTH/2;
	player.position.y = WINDOW_HEIGHT/2;
	player.velocity.x = 0;
	player.velocity.y = 0;
	player.rotation = 0;
	player.body.setOrigin(player.body.getRadius(), player.body.getRadius()); // Will need to change when changed to sprite

	blast_ptr = NULL;
	asteroid_ptr = NULL;
}

void Game::initWindow()
{
	video_mode.width = WINDOW_WIDTH;
	video_mode.height = WINDOW_HEIGHT;
	window = new sf::RenderWindow(video_mode, "Asteroids", sf::Style::Fullscreen);
	window->setFramerateLimit(60);
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
			window->close();
			break;

		case sf::Event::KeyPressed:

			switch (event.key.code) {

			//Escape Key to Close the Game
			case sf::Keyboard::Escape:
				window->close();
				running = false;

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

			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}

float ACCELERATION_CONST = .2;
float DECCELERATION_CONST = .98;
float PI = 3.141592653;

void Game::update()
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
	if (player.position.x < 0) player.position.x += WINDOW_WIDTH;
	if (player.position.y > WINDOW_HEIGHT) player.position.y -= WINDOW_HEIGHT;
	if (player.position.y < 0) player.position.y += WINDOW_HEIGHT;
	player.body.setPosition(player.position);
}

void Game::render()
{
	window->clear();
	window->draw(player.body);

	current_blast = blast_ptr;
	while (current_blast) window->draw(current_blast->body);
	
	current_asteroid = asteroid_ptr;
	while (current_asteroid) window->draw(current_asteroid->body);

	window->display();
}
