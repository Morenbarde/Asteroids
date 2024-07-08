#include "Game.h"
#include <iostream>

void Game::initVariables()
{
	std::srand(time(NULL));

	current_level = 1;
	
	ship_texture.loadFromFile("Images/Ship_sheet.png");
	asteroid_texture.loadFromFile("Images/Asteroid.png");
	resetPlayer();

	star_ptr = NULL;
	initStars();

	blast_ptr = NULL;
	blast_end_ptr = blast_ptr;
	asteroid_ptr = NULL;

	music.openFromFile("Sound/background_music.wav");
	music.setLoop(true);
	music.play();

	blast_sound_buffer.loadFromFile("Sound/laser_fire.wav");
	blast_sound.setBuffer(blast_sound_buffer);

	asteroid_explosion_sound_buffer.loadFromFile("Sound/asteroid_explosion.wav");
	asteroid_explosion_sound.setBuffer(asteroid_explosion_sound_buffer);

	ship_explosion_sound_buffer.loadFromFile("Sound/ship_explosion.wav");
	ship_explosion_sound.setBuffer(ship_explosion_sound_buffer);

	game_is_over = false;
	this->font.loadFromFile("Fonts/CloisterBlack.ttf");

	this->gameEndedText.setFont(font);
	this->gameEndedText.setString("Game Over");
	this->gameEndedText.setCharacterSize(150);
	this->gameEndedText.setOrigin(gameEndedText.getCharacterSize() * gameEndedText.getString().getSize() / 4, 
		gameEndedText.getCharacterSize() / 2);
	this->gameEndedText.setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

	between_levels = true;
	this->levelText.setFont(font);
	this->levelText.setString("Level " + std::to_string(current_level));
	this->levelText.setCharacterSize(150);
	this->levelText.setOrigin(levelText.getCharacterSize() * levelText.getString().getSize() / 4,
		levelText.getCharacterSize() / 2);
	this->levelText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	end_level = false;
	this->endLevelText.setFont(font);
	this->endLevelText.setString("Level Complete");
	this->endLevelText.setCharacterSize(150);
	this->endLevelText.setOrigin(endLevelText.getCharacterSize() * endLevelText.getString().getSize() / 4,
		endLevelText.getCharacterSize() / 2);
	this->endLevelText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	score = 0;
	this->scoreText.setFont(font);
	this->scoreText.setCharacterSize(30);
	this->scoreText.setPosition(10, 10);
}

void Game::initStars() {
	Star* new_star;
	for (int i = 0; i < STAR_COUNT; i++) {
		new_star = new Star();
		new_star->body = sf::CircleShape(float(rand() % 3)/2);
		new_star->body.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
		new_star->body.setFillColor(sf::Color::White);
		new_star->next = star_ptr;
		star_ptr = new_star;
	}
}

void Game::initWindow()
{
	video_mode.width = WINDOW_WIDTH;
	video_mode.height = WINDOW_HEIGHT;
	window = new sf::RenderWindow(video_mode, "Asteroids", sf::Style::Fullscreen);
	window->setFramerateLimit(60);
}

void Game::createBlast()
{
	Blast* new_blast = new Blast();
	new_blast->body = sf::CircleShape(blast_size);
	new_blast->body.setFillColor(sf::Color(0, 255, 255));
	new_blast->position = player.position;
	new_blast->lifespan = blast_lifespan;

	//Since blast speed is constant, can normalize and set constat x and y velocity values
	new_blast->velocity.x = BLAST_SPEED * sin(player.body.getRotation() * (PI / 180)) + player.velocity.x;
	new_blast->velocity.y = BLAST_SPEED * cos(player.body.getRotation() * (PI / 180)) + player.velocity.y;

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

void Game::createAsteroid()
{
	Asteroid* new_asteroid = new Asteroid();

	new_asteroid->level = 1;
	new_asteroid->body = sf::CircleShape(ASTEROID_SIZE_1);

	new_asteroid->body.setTexture(&asteroid_texture);

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
	new_asteroid->velocity.x = ASTEROID_SPEED_1 * cos(asteroid_angle * (PI / 180));
	new_asteroid->velocity.y = ASTEROID_SPEED_1 * sin(asteroid_angle * (PI / 180));

	//Give asteroid randome spin
	new_asteroid->rotate_speed = rand() % MAX_ROTATION;
	if (rand() % 2 == 1) new_asteroid->rotate_speed *= -1;

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

void Game::createAsteroid(float x_pos, float y_pos, int level) {
	Asteroid* new_asteroid = new Asteroid();

	new_asteroid->level = level;

	float speed = 0;

	switch (level) {
	case 1:
		new_asteroid->body = sf::CircleShape(ASTEROID_SIZE_1);
		speed = ASTEROID_SPEED_1;
		break;
	case 2:
		new_asteroid->body = sf::CircleShape(ASTEROID_SIZE_2);
		speed = ASTEROID_SPEED_2;
		break;
	case 3:
		new_asteroid->body = sf::CircleShape(ASTEROID_SIZE_3);
		speed = ASTEROID_SPEED_3;
		break;
	default:
		std::cout << "Create Asteroid: Incompatible Asteroid Level" << std::endl;
	}

	new_asteroid->body.setTexture(&asteroid_texture);

	new_asteroid->position.x = x_pos;
	new_asteroid->position.y = y_pos;

	//std::cout << new_asteroid->position.x << " " << new_asteroid->position.y << std::endl;

	//Gives asteroid random direction to travel
	int asteroid_angle = rand() % 360;
	new_asteroid->velocity.x = speed * cos(asteroid_angle * (PI / 180));
	new_asteroid->velocity.y = speed * sin(asteroid_angle * (PI / 180));

	//Give asteroid randome spin
	new_asteroid->rotate_speed = rand() % MAX_ROTATION;
	if (rand() % 2 == 1) new_asteroid->rotate_speed *= -1;

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

void Game::splitAsteroid(Asteroid* current_asteroid) {
	if (current_asteroid->level != 3) {
		score += 25 * current_asteroid->level;
		createAsteroid(current_asteroid->position.x, current_asteroid->position.y, current_asteroid->level + 1);
		createAsteroid(current_asteroid->position.x, current_asteroid->position.y, current_asteroid->level + 1);
	}
	else {
		score += 100;
	}
}

void Game::initiateLevel() {
	int level = current_level;
	resetAsteroids();
	for (int i = 0; i < level*2; i++) {
		createAsteroid();
	}
	level_active = true;
}

void Game::endGame() {
	running = false;
	if (window->isOpen()) window->close();

}

//Update Functions

void Game::updatePlayer()
{
	//Rotates the player if the keys have been pressed and not released
	if (player_rotating_right) player.body.rotate(3.5f);
	if (player_rotating_left) player.body.rotate(-3.5f);

	//Gets the angle of the player in radians
	float player_rotation = player.body.getRotation();

	//Moves the player if the key has been pressed and not released
	if (player_moving_forward) {
		//Normalizes the acceleration and adds to both the x and y directiong
		player.velocity.x += ACCELERATION_CONST * sin(player_rotation * (PI / 180));
		player.velocity.y += ACCELERATION_CONST * cos(player_rotation * (PI / 180));

		Texture_pos++;
		if (Texture_pos * SPRITE_WIDTH >= ship_texture.getSize().x) Texture_pos = 1;

		player.body.setTextureRect(sf::IntRect(SPRITE_WIDTH * Texture_pos, 0, SPRITE_WIDTH, SPRITE_HEIGHT));
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
	Blast* current_blast;
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
	Asteroid* current_asteroid = asteroid_ptr;

	while (current_asteroid) {

		current_asteroid->body.rotate(current_asteroid->rotate_speed);
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

void Game::updateScore()
{
	this->scoreText.setString("Score: " + std::to_string(score));
}

void Game::updateLevel()
{
	this->levelText.setString("Level " + std::to_string(current_level));
	this->levelText.setOrigin(levelText.getCharacterSize() * levelText.getString().getSize() / 4,
		levelText.getCharacterSize() / 2);
	this->levelText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	resetStars();
}

void Game::checkAsteroidCollision()
{
	//Asteroid-Blast Collision
	Blast* prev_blast = NULL;
	Blast* current_blast = blast_ptr;
	bool blast_deleted = false;

	while (current_blast) {
		Asteroid* prev_asteroid = NULL;
		Asteroid* next_asteroid;
		Asteroid* current_asteroid = asteroid_ptr;
		blast_deleted = false;
		while (current_asteroid && !blast_deleted) {
			if (current_blast->body.getGlobalBounds().intersects(current_asteroid->body.getGlobalBounds())) {
				std::cout << "Struct Pos:" << current_asteroid->position.x << ", " << current_asteroid->position.y << std::endl;
				//Removes Asteroid from linked list
				if (current_asteroid == asteroid_ptr) {
					asteroid_ptr = current_asteroid->next;
					//next_asteroid = asteroid_ptr;
					splitAsteroid(current_asteroid);
					delete current_asteroid;
					current_asteroid = asteroid_ptr;
				}
				else {
					prev_asteroid->next = current_asteroid->next;
					splitAsteroid(current_asteroid);
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
				asteroid_explosion_sound.play();
				break;
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
	Asteroid* current_asteroid = asteroid_ptr;
	Asteroid* prev_asteroid = NULL;
	while (current_asteroid && !game_is_over) {
		if (sqrt(pow(player.position.x - current_asteroid->position.x, 2)
			+ pow(player.position.y - current_asteroid->position.y, 2)) < current_asteroid->body.getRadius()+11) {
			game_is_over = true;
			level_active = false;

			if (current_asteroid == asteroid_ptr) asteroid_ptr = current_asteroid->next;
			else prev_asteroid->next = current_asteroid->next;

			splitAsteroid(current_asteroid);
			delete current_asteroid;

			ship_explosion_sound.play();
			break;
		}
		prev_asteroid = current_asteroid;
		current_asteroid = current_asteroid->next;
	}
}

void Game::resetPlayer()
{
	player.position.x = WINDOW_WIDTH / 2;
	player.position.y = WINDOW_HEIGHT / 2;
	player.velocity.x = 0;
	player.velocity.y = 0;
	player.body.setRotation(0);
	Texture_pos = 0;
	player.body.setTextureRect(sf::IntRect(SPRITE_WIDTH * Texture_pos, 0, SPRITE_WIDTH, SPRITE_HEIGHT));
	player.body.setTexture(ship_texture);
	player.body.setOrigin(player.body.getTextureRect().width/2, player.body.getTextureRect().height/2); // Will need to change when changed to sprite
	player.body.setPosition(player.position);
}

void Game::resetAsteroids()
{
	Asteroid* next_asteroid = asteroid_ptr;
	Asteroid* current_asteroid;
	while (next_asteroid) {
		current_asteroid = next_asteroid;
		next_asteroid = next_asteroid->next;
		delete current_asteroid;
	}
	asteroid_ptr = NULL;
}

void Game::resetStars()
{
	Star* current_star = star_ptr;
	while (current_star) {
		current_star->body.setRadius(float(rand() % 3) / 2);
		current_star->body.setPosition(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
		current_star = current_star->next;
	}
}

void Game::resetScore()
{
	score = 0;
	updateScore();
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
				if (!blaster_locked && !game_is_over && !between_levels && !end_level) {
					blast_sound.play();
					createBlast();
					blaster_locked = true;
				}
				break;

			//Create Asteroid For Testing
			case sf::Keyboard::Z:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && !end_level && !between_levels && !game_is_over) createAsteroid();
				break;
			
			case sf::Keyboard::Enter:
				if (game_is_over) {
					current_level = 1;
					resetScore();
					resetAsteroids();
					resetPlayer();
					game_is_over = false;
					updateLevel();
					between_levels = true;
				}
				else if (between_levels) {
					initiateLevel();
					between_levels = false;
				}
				else if (end_level) {
					current_level += 1;
					resetPlayer();
					updateLevel();
					end_level = false;
					between_levels = true;
				}

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
				Texture_pos = 0;
				player.body.setTextureRect(sf::IntRect(SPRITE_WIDTH * Texture_pos, 0, SPRITE_WIDTH, SPRITE_HEIGHT));
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		blaster_locked = false;
	}
}

void Game::update()
{
	if (!between_levels) updatePlayer();
	updateBlasts();
	updateAsteroids();

	checkAsteroidCollision();
	checkPlayerCollision();

	updateScore();
	
	if (level_active && !asteroid_ptr) {
		end_level = true;
		level_active = false;
	}

}

void Game::render()
{
	window->clear();

	Star* current_star = star_ptr;
	while (current_star) {
		window->draw(current_star->body);
		current_star = current_star->next;
	}

	Blast* current_blast = blast_ptr;
	while (current_blast) {
		window->draw(current_blast->body);
		current_blast = current_blast->next;
	}

	Asteroid* current_asteroid = asteroid_ptr;
	while (current_asteroid) {
		window->draw(current_asteroid->body);
		current_asteroid = current_asteroid->next;
	}

	if (!game_is_over) window->draw(player.body);
	else window->draw(gameEndedText);

	if (between_levels) window->draw(levelText);
	if (end_level) window->draw(endLevelText);


	window->draw(scoreText);

	window->display();
}
