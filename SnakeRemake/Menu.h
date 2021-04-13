#pragma once
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>

class Screen {
public:
	virtual void start() = 0;
	static char getKeyPress();

	static void setUp(char upKey);
	static void setDown(char downKey);

private:
	static const char empty = '\0';
	static const char select = '\n';
	static char up;
	static char down;
	static bool noKeyPressed;
};


class Menu : public Screen {
public:
	Menu(std::string text = std::string());

	void start();
	void drawScreen();

	void addExit(std::string text);
	void addStaticEntry(std::string text);
	void addStaticEntry(std::string text, Screen &entryAction);
	void addVariableEntry(std::string text, int &variable);
	void addVariableEntry(std::string text, int &variable, Screen &entryAction);

private:
	struct Entry {
		std::string entryText;
		int *entryVal;
		bool isVar;

		Entry(std::string text, bool isVar = false, int *val = nullptr) 
			: entryText{ text }, entryVal{ val }, isVar{ isVar } { }
	};

	std::vector<Entry> menuEntries;
	std::vector<Screen*> subScreens;
	std::string headerText;
	int entryCount = 0;
	int cursorLoc = 0;
	int exitLoc = -1;
	bool updateScreen = true;
};


class Modifier : public Screen {
public:
	Modifier(int *inValue, std::string header, std::string text = "Enter new value: ");

	void start();
	void drawScreen();

private:
	int *value;
	std::string headerText;
	std::string modifierText;
};


class Text : public Screen {
public:
	Text(std::string header, std::string body = std::string());

	void start();
	void drawScreen();

	void setText(std::string body);

private:
	std::string headerText;
	std::string bodyText;
};


class Custom : public Screen {
public:
	Custom(std::string header, void(*lambda)() = [] { return; });

	void start();

private:
	std::string headerText;
	void(*lambdaFunc)();
};