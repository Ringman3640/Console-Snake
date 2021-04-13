#include "Menu.h"

//Static Initialization
char Screen::up = 'W';
char Screen::down = 'S';
bool Screen::noKeyPressed = true;


//Screen Statics
char Screen::getKeyPress() {
	if (GetKeyState(up) & 0x8000) {
		if (noKeyPressed) {
			noKeyPressed = false;
			return up;
		}
		noKeyPressed = false;
	}
	else if (GetKeyState(down) & 0x8000) {
		if (noKeyPressed) {
			noKeyPressed = false;
			return down;
		}
		noKeyPressed = false;
	}
	else if (GetKeyState(13) & 0x8000) { //13 is the enter key
		if (noKeyPressed) {
			noKeyPressed = false;
			return select;
		}
		noKeyPressed = false;
	}
	else noKeyPressed = true;
	return empty;
}

void Screen::setUp(char upKey) {
	up = upKey;
}

void Screen::setDown(char downKey) {
	down = downKey;
}


//Menu Class
Menu::Menu(std::string text) : headerText{ text } {

}

void Menu::start() {
	cursorLoc = 0;
	bool exit = false;

	while (!exit) {
		if (updateScreen) drawScreen();

		updateScreen = true;
		switch (getKeyPress()) {
		case 'W':
			if (cursorLoc > 0) --cursorLoc;
			else cursorLoc = entryCount - 1;
			break;

		case 'S':
			if (cursorLoc < entryCount - 1) ++cursorLoc;
			else cursorLoc = 0;
			break;

		case '\n':
			std::cin.ignore(1000, '\n');
			if (subScreens[cursorLoc] != nullptr)
				subScreens[cursorLoc]->start();
			else if (cursorLoc == exitLoc) exit = true;
			break;

		default:
			updateScreen = false;
			break;
		}
	}
	std::system("CLS");
}

void Menu::drawScreen() {
	std::system("CLS");

	std::cout << headerText << std::endl << std::endl;
	for (int i = 0; i < entryCount; ++i) {
		if (i == cursorLoc) std::cout << " > ";
		std::cout << menuEntries[i].entryText;
		if (menuEntries[i].isVar) std::cout << *menuEntries[i].entryVal;
		std::cout << std::endl;
	}

	updateScreen = false;
}

void Menu::addExit(std::string text) {
	menuEntries.push_back(text);
	subScreens.push_back(nullptr);
	exitLoc = entryCount;
	++entryCount;
}

void Menu::addStaticEntry(std::string text) {
	menuEntries.push_back(text);
	subScreens.push_back(nullptr);
	++entryCount;
}
void Menu::addStaticEntry(std::string text, Screen &entryAction) {
	menuEntries.push_back(text);
	subScreens.push_back(&entryAction);
	++entryCount;
}

void Menu::addVariableEntry(std::string text, int& variable) {
	menuEntries.push_back(Entry(text, true, &variable));
	subScreens.push_back(nullptr);
	++entryCount;
}
void Menu::addVariableEntry(std::string text, int& variable, Screen &entryAction) {
	menuEntries.push_back(Entry(text, true, &variable));
	subScreens.push_back(&entryAction);
	++entryCount;
}


//Modifier Class
Modifier::Modifier(int *inValue, std::string header, std::string text)
	:value{ inValue }, headerText { header }, modifierText{ text } {

}

void Modifier::start() {
	int inputVal;
	drawScreen();
	
	while (!(std::cin >> inputVal)) {
		std::cout << std::endl;
		std::cout << "Invalid value type, enter an integer value: ";
		std::cin.clear();
		std::cin.ignore(1000, '\n');
	}
	std::cin.ignore(1000, '\n');

	*value = inputVal;
}

void Modifier::drawScreen() {
	std::system("CLS");

	std::cout << headerText;
	std::cout << std::endl << std::endl;
	std::cout << "Current value: " << *value << std::endl;
	std::cout << modifierText;
}


//Text Class
Text::Text(std::string header, std::string body)
	: headerText{ header }, bodyText { body } {

}

void Text::start() {
	drawScreen();

	std::cin.ignore(1000, '\n');
	std::cin.clear();
}

void Text::drawScreen() {
	std::system("CLS");

	std::cout << headerText;
	std::cout << std::endl << std::endl;
	std::cout << bodyText << std::endl;
	std::cout << "Press Enter to return . . . \n";
}

void Text::setText(std::string body) {
	bodyText = body;
}


//Custom Class
Custom::Custom(std::string header, void(*lambda)())
	: headerText{ header }, lambdaFunc{ lambda } {

}

void Custom::start() {
	std::system("CLS");
	std::cout << headerText << std::endl << std::endl;
	lambdaFunc();
}