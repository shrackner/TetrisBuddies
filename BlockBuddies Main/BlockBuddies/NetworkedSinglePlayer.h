#pragma once
#include "Screen.hpp"
#include "GraphicsManager.hpp"
#include "ScreenManager.hpp"
#include "CursorHandler.hpp"
#include "GameStateObject.hpp"
#include "ClientManager.h"
#include "UIElement.hpp"
#include "TextBox.hpp"
#include "TextureManager.hpp"
#include <vector>
#include <tuple>
#include <iostream>
#include "AnimatedSprite.h"
#include "Mascot.h"
#include <cstdio>
#include <ctime>
#include <time.h>

class NetworkedSinglePlayer : public Screen
{
public:
	NetworkedSinglePlayer();
	~NetworkedSinglePlayer();
	void update();
	void draw();
    void reload();
	sf::RectangleShape blocks[GameStateObject::boardHeight][GameStateObject::boardWidth];
	sf::RectangleShape nextBlocks[GameStateObject::boardWidth];
	//number of different sprite shapes
	
private:
	std::vector <std::pair<int, int>> dangerColumns;
	std::vector <int> dangerMark;
	Mascot miku;
	void playThis();
	CursorHandler *ch;
	bool pressed, pressed2;
	sf::RectangleShape rec;
	sf::RectangleShape tempRowRec;
	sf::Clock clock;
	sf::Time timer;
	sf::Time lastFrameTime;
	unsigned int winX;
	unsigned int winY;
	int gameScreenHeight;
	int gameScreenWidth;
	GameStateObject gso;
	int blockSizeX;
	int blockSizeY;
	int dangerColumnCounter;
	void initGame();
	void updateBlocks();
	bool reset;
	sf::Sound swapSound;
	TextBox* username;
	TextBox* score;
	TextBox* highScore;
	TextBox* name;
	TextBox* oldHighScore;
	TextBox* scoreToBeat;
	TextBox* next;
	TextBox* row;
	TextBox* elapsedTime;
	TextBox* isPaused;
	TextBox* nextRowIn;

	clock_t initialTime;
	clock_t elapsed;
	bool startTimer = false;
	void startTime();
	sf::Sprite blockShape;



	struct blockAnimate {
		sf::RectangleShape first;
		sf::RectangleShape second;
		sf::RectangleShape third;
		sf::RectangleShape fourth;
		sf::RectangleShape blocks[4];
		sf::Vector2f pos;
		sf::Vector2f vel;

		/*blockAnimate() : first(sf::Vector2f(25, 25))
			,second(sf::Vector2f(25, 25))
			,third(sf::Vector2f(25, 25))
			, fourth(sf::Vector2f(25, 25))
			, vel (15 , 9.8)
		{
			
			blocks[0] = first;
			blocks[1] = second;
			blocks[2] = third;
			blocks[3] = fourth;
		}*/
	};
	

};
