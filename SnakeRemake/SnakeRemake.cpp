#include "Menu.h"
#include <iostream>
#include <Windows.h>
#include <list>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <utility>

#define UP 'W'
#define DOWN 'S'
#define RIGHT 'D'
#define LEFT 'A'
#define CLEAR '\0'

namespace snake {
	std::list<std::pair<int, int>> bodyQueue;
	int headX, headY, length = 1;
	int prevX = 0, prevY = 0;
	int tailX = 0, tailY = 0;
}

namespace game {
	std::vector<std::vector<short>> board; //0 for empty, 1 for head, 2 for body, 3 for fruit
	std::vector<int> yEntities;
	bool gameEnd = false, gameWin = false, fruitSpawned = false;
	int fruitX = 0, fruitY = 0;
	char prevMove = CLEAR;
	int gameWidth, gameHeight, playSpace;
	bool teleportWalls;
}

void loadSettings();
void setDefault();
void initializeSettings();
void startScreen();
void saveSettings();

void gameLoop();
char getInput();
void spawnFruit();
void updateHead();
void updateBody();
void checkCollision();
void drawScreen();

void endAnimation();
void endScreen();


int main() {

	loadSettings();

	startScreen();

	initializeSettings();

	gameLoop();

}

void loadSettings() {
	std::ifstream fin;
	fin.open("settings.txt");

	if (!fin) {
		fin.close();
		fin.clear();
		setDefault();
		return;
	}

	fin >> snake::headX;
	fin >> snake::headY;

	fin >> game::gameWidth;
	fin >> game::gameHeight;

	fin.close();
}

void setDefault() {
	snake::headX = 10;
	snake::headY = 10;

	game::teleportWalls = true;
	game::gameWidth = 20;
	game::gameHeight = 20;
	game::playSpace = 324;
}

void initializeSettings() {
	//Settings checks
	if (snake::headX <= 0) snake::headX = 1;
	if (snake::headY <= 0) snake::headY = 1;

	if (game::gameWidth <= 4) game::gameWidth = 5;
	if (game::gameWidth > 40) game::gameWidth = 40;
	if (game::gameHeight <= 4) game::gameHeight = 5;
	if (game::gameHeight > 40) game::gameHeight = 40;
	

	if (snake::headX >= game::gameWidth) snake::headX = 1;
	if (snake::headY >= game::gameHeight) snake::headY = 1;

	//Initializations
	game::yEntities.resize(game::gameHeight);
	game::board.resize(game::gameHeight);
	for (int i = 0; i < game::gameHeight; ++i) {
		game::board[i].resize(game::gameWidth);
	}

	std::srand(std::time(NULL));
	game::playSpace = (game::gameWidth - 2) * (game::gameHeight - 2);
	game::teleportWalls = true; //Option to change this in settings will be added later, need to edit Menu class
}

void startScreen() {
	Menu startScreen("SNAKE");

	Menu settings("SNAKE > Settings");
	Modifier modSnakeX(snake::headX, "SNAKE > Settings > Change Starting X Position", "Enter new value: ");
	Modifier modSnakeY(snake::headY, "SNAKE > Settings > Change Starting Y Position", "Enter new value: ");
	Modifier modWidth(game::gameWidth, "SNAKE > Settings > Game Width", "Enter new width: ");
	Modifier modHeight(game::gameHeight, "SNAKE > Settings > Game Height", "Enter new Height: ");
	Custom resetSettings(std::string(), []() { setDefault(); });

	Text controls("Snake > Controls");
	Text credits("SNAKE > Credits");

	startScreen.addExit("Play");
	startScreen.addStaticEntry("Settings", settings);
	startScreen.addStaticEntry("Controls", controls);
	startScreen.addStaticEntry("Credits", credits);
	startScreen.exitAction([]() {
		saveSettings();
	});

	settings.addVariableEntry("Starting X Position: ", snake::headX, modSnakeX);
	settings.addVariableEntry("Starting Y Position: ", snake::headY, modSnakeY);
	settings.addVariableEntry("Playspace Width: ", game::gameWidth, modWidth);
	settings.addVariableEntry("Playspace Height: ", game::gameHeight, modHeight);
	settings.addStaticEntry("Reset to defaults", resetSettings);
	settings.addExit("Return");

	controls.setText("To be added . . . ");

	credits.setText("Programmer: Franz Alarcon \nTo be added . . . ");

	startScreen.start();
}

void saveSettings() {
	std::ofstream fout("settings.txt");

	fout << snake::headX << ' ';
	fout << snake::headY << ' ';

	fout << game::gameWidth << ' ';
	fout << game::gameHeight;

	fout.close();
}

void gameLoop() {
	while (!game::gameEnd) {

		spawnFruit();

		updateHead();

		updateBody();

		checkCollision();
		if (game::gameEnd) break;

		drawScreen();
	}

	endAnimation();

	endScreen();
}

char getInput() {
	if (GetAsyncKeyState(UP)) return UP;
	if (GetAsyncKeyState(DOWN)) return DOWN;
	if (GetAsyncKeyState(RIGHT)) return RIGHT;
	if (GetAsyncKeyState(LEFT)) return LEFT;
	return CLEAR;
}

void spawnFruit() {
	if (game::fruitSpawned) return;

	//Set Y position
	game::fruitY = std::rand() % (game::gameHeight - 2) + 1;
	while (game::yEntities[game::fruitY] >= game::gameHeight - 2) {
		++game::fruitY;
		if (game::fruitY == game::gameHeight - 1) game::fruitY = 1;
	}

	//Set X position
	game::fruitX = std::rand() % (game::gameWidth - 2) + 1;
	while (game::board[game::fruitY][game::fruitX] != 0) {
		++game::fruitX;
		if (game::fruitX == game::gameWidth - 1) game::fruitX = 1;
	}

	//Set in board
	game::board[game::fruitY][game::fruitX] = 3;
	++game::yEntities[game::fruitY];

	game::fruitSpawned = true;
}

void updateHead() {
	char input = getInput();
	bool updatePrev = true;
	if (input == CLEAR) input = game::prevMove;

	snake::prevX = snake::headX;
	snake::prevY = snake::headY;

	switch (input) {
	case UP:
		if (game::prevMove != DOWN) ++snake::headY;
		else {
			--snake::headY;
			updatePrev = false;
		}
		break;

	case DOWN:
		if (game::prevMove != UP) --snake::headY;
		else {
			++snake::headY;
			updatePrev = false;
		}
		break;

	case RIGHT:
		if (game::prevMove != LEFT) ++snake::headX;
		else {
			--snake::headX;
			updatePrev = false;
		}
		break;

	case LEFT:
		if (game::prevMove != RIGHT) --snake::headX;
		else {
			++snake::headX;
			updatePrev = false;
		}
		break;
	}
	if (updatePrev) game::prevMove = input;

	//Board updates in checkCollision function
}

void updateBody() {
	//Move body forwards
	if (snake::length > 1) {
		game::board[snake::prevY][snake::prevX] = 2;
		snake::bodyQueue.emplace_front(snake::prevX, snake::prevY);
	}

	//Remove Tail
	if (snake::length == 1) {
		game::board[snake::prevY][snake::prevX] = 0;
	}
	else if (snake::bodyQueue.size() == snake::length) {
		int yPos = snake::bodyQueue.back().second;
		int xPos = snake::bodyQueue.back().first;
		game::board[yPos][xPos] = 0;
		--game::yEntities[yPos];
		snake::bodyQueue.pop_back();
	}
}

void checkCollision() {
	//Check wall collision
	//With wall teleports on:
	if (game::teleportWalls) {
		if (snake::headX < 1) snake::headX = game::gameWidth - 1;
		if (snake::headX > game::gameWidth - 1) snake::headX = 1;

		if (snake::headY < 1) snake::headY = game::gameHeight - 1;
		if (snake::headY > game::gameHeight - 1) snake::headY = 1;
	}
	//With wall teleports off (game over):
	else {
		if (snake::headX < 1 || snake::headX > game::gameWidth - 1) game::gameEnd = true;
		if (snake::headY < 1 || snake::headY > game::gameHeight - 1) game::gameEnd = true;
	}

	//Check fruit collision
	if (snake::headX == game::fruitX && snake::headY == game::fruitY) {
		++snake::length;
		--game::yEntities[game::fruitY];
		game::fruitSpawned = false;
	}
	//Check win condition
	if (snake::length == game::playSpace) {
		game::gameWin = true;
		game::gameEnd = true;
	}

	//Check body collision
	if (game::board[snake::headY][snake::headX] == 2) game::gameEnd = true;

	//Update head position in board
	game::board[snake::headY][snake::headX] = 1;
	++game::yEntities[snake::headY];
}

void drawScreen() {
	char symbols[4] = { ' ', char(254), char(254), 'O' };

	std::system("CLS");

	//Top boarder
	for (int i = 0; i <= game::gameWidth; ++i) std::cout << "X ";
	std::cout << std::endl;

	//Middle rows
	for (int i = game::gameHeight - 1; i > 0; --i) {
		std::cout << "X ";

		//Non-occupied row
		if (game::yEntities[i] == 0)
			for (int j = 1; j < game::gameWidth; ++j) std::cout << "  ";

		//Occupied row
		else {
			for (int j = 1; j < game::gameWidth; ++j) {
				std::cout << symbols[game::board[i][j]] << ' ';
			}
		}

		std::cout << "X ";
		std::cout << std::endl;
	}

	//Bottom boarder
	for (int i = 0; i <= game::gameWidth; ++i) std::cout << "X ";

	std::cout << "\nHead X: " << snake::headX << std::endl;
	std::cout << "Head Y: " << snake::headY << std::endl;
	std::cout << "Length: " << snake::length << std::endl;
	std::cout << "\nFruit X: " << game::fruitX << std::endl;
	std::cout << "Fruit Y: " << game::fruitY;
}

//drawScreen function for debugging, prints game::board values
//void drawScreen() {
//	std::system("CLS");
//
//	for (int i = game::gameHeight - 1; i >= 0; --i) {
//		for (int j = 0; j < game::gameWidth; ++j) {
//			std::cout << game::board[i][j] << ' ';
//		}
//		std::cout << std::endl;
//	}
//}

void endAnimation() {
	Sleep(1000);

	while (snake::length > 1) {
		game::board[snake::headY][snake::headX] = 0;
		snake::headY = snake::bodyQueue.front().second;
		snake::headX = snake::bodyQueue.front().first;
		snake::bodyQueue.pop_front();

		--snake::length;
		drawScreen();
	}

	snake::headX = 0;
	snake::headY = 0;
	drawScreen();
}

void endScreen() {
	std::system("CLS");
	//std::cin.ignore(9999, '\n');
	if (game::gameWin) {
		Text gameWin(std::string(), "\n\n\nCongratulations!\n\n\n");
		gameWin.start();
	}
	else {
		Text gameOver(std::string(), "\n\n\nGAME OVER\n\n\n");
		gameOver.start();
	}
}