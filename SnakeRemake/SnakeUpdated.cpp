#include "Menu.h"
#include <iostream>
#include <Windows.h>
#include <list>
#include <fstream>
#include <algorithm>

#define UP 'W'
#define DOWN 'S'
#define RIGHT 'D'
#define LEFT 'A'
#define CLEAR '\0'

namespace snake {
	int headX, headY, length = 1;
	int prevX = 0, prevY = 0;
	int tailX = 0, tailY = 0;
	std::vector<std::list<int>> bodyPos;
	std::list<int> bodyYOrder;
}

namespace game {
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

void gameLoop();
char getInput();
void spawnFruit();
void updateHead();
void updateBody();
void checkCollision();
bool onSnakeEntity();
void drawScreen();


int main() {

	loadSettings();

	initializeSettings();

	startScreen();

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

	std::cin >> snake::headX; //double check these later
	std::cin >> snake::headY;

	if (fin.get() == 1) game::teleportWalls = true;
	else game::teleportWalls = false;
	std::cin >> game::gameWidth;
	std::cin >> game::gameHeight;
	game::playSpace = (game::gameWidth - 2) * (game::gameHeight - 2);

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
	snake::bodyPos.resize(game::gameHeight);
}

void startScreen() {
	Menu startScreen("SNAKE");
	Menu settings("SNAKE > Settings");
	Text controls("Snake > Controls");
	Text credits("SNAKE > Credits");

	startScreen.addExit("Play");
	startScreen.addStaticEntry("Settings", settings);
	startScreen.addStaticEntry("Controls", controls);
	startScreen.addStaticEntry("Credits", credits);

	startScreen.start();
}

void gameLoop() {
	while (!game::gameEnd) {
		//spawnFruit();
		updateHead();
		//std::cout << "X Pos: " << snake::headX;
		//std::cout << "\nY Pos: " << snake::headY;
		updateBody();
		checkCollision();
		if (game::gameEnd) break;
		drawScreen();
	}
}

char getInput() {
	if (GetAsyncKeyState(UP)) return UP;
	if (GetAsyncKeyState(DOWN)) return DOWN;
	if (GetAsyncKeyState(RIGHT)) return RIGHT;
	if (GetAsyncKeyState(LEFT)) return LEFT;
	return CLEAR;
}

void updateHead() {
	char input = getInput();
	if (input == CLEAR) input = game::prevMove;

	snake::prevX = snake::headX;
	snake::prevY = snake::headY;

	switch (input) {
	case UP:
		if (game::prevMove != DOWN) ++snake::headY;
		else {
			--snake::headY;
			return;
		}
		break;

	case DOWN:
		if (game::prevMove != UP) --snake::headY;
		else {
			++snake::headY;
			return;
		}
		break;

	case RIGHT:
		if (game::prevMove != LEFT) ++snake::headX;
		else {
			--snake::headX;
			return;
		}
		break;

	case LEFT:
		if (game::prevMove != RIGHT) --snake::headX;
		else {
			++snake::headX;
			return;
		}
		break;
	}

	game::prevMove = input;
}

void updateBody() {
	//Move body forwards
	snake::bodyPos[snake::prevY].push_front(snake::prevX);
	snake::bodyYOrder.push_front(snake::prevY);

	//Remove Tail
	if (snake::bodyYOrder.size() == snake::length) {
		snake::bodyPos[snake::bodyYOrder.back()].pop_back();
		snake::bodyYOrder.pop_back();
	}
}

void checkCollision() {
	//With wall teleports on:
	if (game::teleportWalls) {
		if (snake::headX < 1) snake::headX = game::gameWidth - 1;
		if (snake::headX > game::gameWidth - 1) snake::headX = 1;

		if (snake::headY < 1) snake::headY = game::gameHeight - 1;
		if (snake::headY > game::gameHeight - 1) snake::headY = 1;
		return;
	}
	//With wall teleports off (game over):
	if (snake::headX < 1 || snake::headX > game::gameWidth - 1) game::gameEnd = true;
	if (snake::headY < 1 || snake::headY > game::gameHeight - 1) game::gameEnd = true;
}

bool onSnakeEntity(int y, int x) {
	//On head
	if (x == snake::headX && y == snake::headY) return true;

	//On body
	auto begin = snake::bodyPos[y].begin();
	auto end = snake::bodyPos[y].end();
	if (std::find(begin, end, x) != end) return true;
	return false;
}

void drawScreen() {
	std::system("CLS");

	//Top boarder
	for (int i = 0; i <= game::gameWidth; ++i) std::cout << "X ";
	std::cout << std::endl;

	//Middle rows
	for (int i = game::gameHeight - 1; i > 0; --i) {
		std::cout << "X ";

		//Non-occupied row
		if (snake::bodyPos[i].empty() && i != snake::headY) {
			for (int j = 1; j < game::gameWidth; ++j) std::cout << "  ";
		}

		//Occupied row
		else {
			for (int j = 1; j < game::gameWidth; ++j) {
				if (onSnakeEntity(i, j)) std::cout << char(254) << ' '; //Only draws snake head and body, not fruit for now
				else std::cout << "  ";
			}
		}

		std::cout << "X ";
		std::cout << std::endl;
	}

	//Bottom boarder
	for (int i = 0; i <= game::gameWidth; ++i) std::cout << "X ";

	std::cout << "\nHead X: " << snake::headX << std::endl;
	std::cout << "Head Y: " << snake::headY;
}