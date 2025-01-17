#include "NetworkedSinglePlayer.h"
#include "InputManager.hpp"
#include "SoundManager.h"
#include "AnimationManager.hpp"
#include <sstream>
#include <vector>
#include "BlockShowerManager.hpp"

NetworkedSinglePlayer::NetworkedSinglePlayer()
	: pressed(false), pressed2(false), reset(false),
	highScore(new TextBox("Player Score: ",
	480.0f,
	-380.0f,
	600.0f)),
	score(new TextBox("Score goes here",
	550.0f,
	-380.0f,
	600.0f)),
	username(new TextBox("Username: ",
	-665.0f,
	-380.0f,
	600.0f)),
	name(new TextBox("Player name goes here",
	-590.0f,
	-380.0f,
	600.0f)),
	oldHighScore(new TextBox("Score to beat: ",
	480.0f,
	-350.0f,
	600.0f)),
	scoreToBeat(new TextBox("Score to beat goes here",
	560.0f,
	-350.0f,
	600.0f)),
	next(new TextBox("Next",
	-165.0f,
	350.0f,
	600.0f)),
	row(new TextBox("Row",
	-165.0f,
	330.0f,
	600.0f)),
	elapsedTime(new TextBox("01:27",
	-10.0f,
	-360.0f,
	600.0f)),
	isPaused(new TextBox("|\|",
	145.0f,
	340.0f,
	600.0f)),
	nextRowIn(new TextBox("in xx seconds",
	205.0f,
	340.0f,
	600.0f))

{
	initGame();
	UIElements.push_back(next);
	UIElements.push_back(row);
	UIElements.push_back(elapsedTime);
	UIElements.push_back(highScore);
	UIElements.push_back(score);
	UIElements.push_back(username);
	UIElements.push_back(name);
	UIElements.push_back(oldHighScore);
	UIElements.push_back(scoreToBeat);
	UIElements.push_back(isPaused);
	UIElements.push_back(nextRowIn);

	swapSound.setBuffer(*SoundManager::getInstance().getSound("heya"));

	miku.miku.setPosition(blockSizeX * 2, blockSizeX * 2);
	miku.miku.setScale(6, 6);
}


NetworkedSinglePlayer::~NetworkedSinglePlayer()
{
}

void NetworkedSinglePlayer::initGame()
{
	
	winX = GraphicsManager::getInstance()->window.getSize().x;
	winY = GraphicsManager::getInstance()->window.getSize().y;
	gameScreenHeight = (int)(winY * 0.8);
	//fill in initial board
	int gridPosy = 0;
	int gridPosx = 0;
	blockSizeX = std::floor((float)gameScreenHeight / gso.boardHeight);
	blockSizeY = std::floor((float)gameScreenHeight / gso.boardHeight);

	AnimationManager::getInstance()->setBlockSize(blockSizeX);
	dangerColumnCounter = 0;
	//draws a large rectangle around the game screen
	int offset = gameScreenHeight - (blockSizeY * GameStateObject::boardHeight);
	gameScreenWidth = blockSizeX * gso.boardWidth;

	//ch = new CursorHandler(gameScreenWidth, gameScreenHeight, winX, winY, blockSizeX, -(offset + blockSizeX)); //5 is the offset, dunno why yet
	
	rec.setSize(sf::Vector2f(gameScreenWidth, -blockSizeY * GameStateObject::boardHeight));
	rec.setFillColor(sf::Color::Transparent);
	rec.setPosition((winX / 2) - gameScreenWidth / 2, (winY - (3 * blockSizeX)));
	rec.setOutlineThickness(blockSizeX/2);
	rec.setOutlineColor(sf::Color::Black);
	
	//the row that holds the temp row
	tempRowRec.setSize(sf::Vector2f(gameScreenWidth, blockSizeX));
	tempRowRec.setFillColor(sf::Color::Transparent);
	tempRowRec.setPosition((winX / 2) - gameScreenWidth / 2, (winY - (2.5 * blockSizeX)));
	tempRowRec.setOutlineThickness(blockSizeX / 3);
	tempRowRec.setOutlineColor(sf::Color::Black);

	//creates blocks and puts them in 2D array
	for (int i = 0; i < GameStateObject::boardHeight; i++)
	{
		for (int j = 0; j < GameStateObject::boardWidth; j++)
		{
			sf::RectangleShape shape(sf::Vector2f(blockSizeX, blockSizeY));
			shape.setPosition(rec.getPosition().x + (j * blockSizeX), rec.getPosition().y - (i * blockSizeX) -blockSizeX);
			//shape.setPosition(j + (winX / 2 - gameScreenWidth / 2), i - (winY / 2 - gameScreenHeight / 2) - blockSizeX); //puts it in the middle of the screen
			shape.setFillColor(sf::Color::Transparent); //transparent blocks to appear as empty space
			blocks[gridPosx][gridPosy] = shape;
			gridPosy++;
			
		}
		gridPosy = 0;
		gridPosx++;
	}
	
	//creates the temp row below this
	for (int i = 0; i < gso.boardWidth; i++)
	{
		sf::RectangleShape shape(sf::Vector2f(blockSizeX, blockSizeY));
		shape.setPosition(tempRowRec.getPosition().x + (i * blockSizeX), tempRowRec.getPosition().y);
		shape.setFillColor(sf::Color::Transparent);
		nextBlocks[i] = shape;
	}

	ch = new CursorHandler(blocks[(GameStateObject::boardHeight / 2) - 1][(GameStateObject::boardWidth / 2) - 1].getPosition(), 
		rec.getPosition(), gameScreenHeight, gameScreenWidth, blockSizeX);
	std::cout << rec.getPosition().x << rec.getPosition().y;
	std::cout << "first block pos" << blocks[9][4].getPosition().x << " " << blocks[9][4].getPosition().y << std::endl;
	std::cout << "cursor pos " << ch->getMainCursor().getPosition().x << " " << ch->getMainCursor().getPosition().y << std::endl;
	ch->setCursorAt(0, 0);
}


void NetworkedSinglePlayer::update()
{
	//delta timer
	float deltaTime = (clock.getElapsedTime() - lastFrameTime).asSeconds();
	lastFrameTime = clock.getElapsedTime();
	if (deltaTime >= .1){ deltaTime = .1; };

	//update Sprite
	miku.update(deltaTime);
	//if song runs out play rando song
	if (SoundManager::getInstance().music.getStatus() != SoundManager::getInstance().music.Playing)
	{
		SoundManager::getInstance().playMusicRandom();
	}
	//takes care of displaying the score.
	std::string scoreString;
	int Number = (int)gso.score;
	scoreString = static_cast<std::ostringstream*>(&(std::ostringstream() << Number))->str();
	score->message.setString(scoreString);


	if (reset)
	{
		initGame();
		reset = false;
	}
	if (!ClientManager::getInstance().gameOver)
	{
		//update the game if there is an update available
		if (ClientManager::getInstance().isUpdated)
		{
			gso = ClientManager::getInstance().currentGSO;
			ClientManager::getInstance().isUpdated = false;

			if (gso.newRowActive){
				ch->Up(sf::Keyboard::Key::Up);

			}
			updateBlocks();

			if (!gso.clearingBlocks.empty())
			{
				for (int i = 0; i < gso.clearingBlocks.size(); i++)
				{
					AnimationManager::getInstance()->addClear(blocks[gso.clearingBlocks.at(i).first][gso.clearingBlocks.at(i).second]);
				}
				AnimationManager::getInstance()->setClearingAdd();
				miku.cheer();
			}

			AnimationManager::getInstance()->clearDangerBlocks();

			dangerColumns.clear();
			dangerColumnCounter = 0;
			for (int row = 0; row < GameStateObject::boardWidth; row++)
			{
				if (blocks[GameStateObject::boardHeight - 4][row].getFillColor() != sf::Color::Transparent)
					dangerMark.push_back(row);
				else
					dangerMark.push_back(-1);
			}

			for (int col = 0; col < GameStateObject::boardHeight; col++)
			{
				for (int row = 0; row < GameStateObject::boardWidth; row++)
				{
					if (dangerMark.at(row) != -1 && blocks[col][row].getFillColor() != sf::Color::Transparent)
					{
						AnimationManager::getInstance()->addDanger(blocks[col][row]);
						dangerColumns.push_back(std::make_pair(col, row));
					}
				}
			}
			dangerMark.clear();

			AnimationManager::getInstance()->setTextureDanger();

		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			if (pressed == false)
			{
				ch->Left(sf::Keyboard::Key::Left);
				pressed = true; // Cannot hold right to move
			}

		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			if (pressed == false)
			{
				ch->Right(sf::Keyboard::Key::Right);
				pressed = true; // Cannot hold right to move
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (pressed == false)
			{
				ch->Up(sf::Keyboard::Key::Up);
				pressed = true; // Cannot hold right to move
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (pressed == false)
			{
				ch->Down(sf::Keyboard::Key::Down);
				pressed = true; // Cannot hold right to move
			}
		}
		else
			pressed = false;

		//keypress for block switching
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) //swaps the main block with the left block
		{
			if (pressed2 == false && ch->getCursorX() > 0)//cant switch if left side off screen
			{
				ClientManager::getInstance().requestSwap(ch->getCursorY(), ch->getCursorX(), ch->getCursorY(), ch->getCursorX() - 1);
				if (swapSound.getStatus() != swapSound.Playing)
					swapSound.play();
				//ScreenManager::getInstance()->shake(.5);

				int y = ch->getCursorY();
				int x = ch->getCursorX();
				
				AnimationManager::getInstance()->addSwap(blocks[y][x], blocks[y][x - 1]);
				pressed2 = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) //swaps the main block with the right block
		{
			if (pressed2 == false && ch->getCursorX() < GameStateObject::boardWidth - 1)//cant switch if right block is off screen
			{
				ClientManager::getInstance().requestSwap(ch->getCursorY(), ch->getCursorX(), ch->getCursorY(), ch->getCursorX() + 1);
				pressed2 = true;
				//ScreenManager::getInstance()->shake(.5);
				if (swapSound.getStatus() != swapSound.Playing)
					swapSound.play();

				int y = ch->getCursorY();
				int x = ch->getCursorX();

				AnimationManager::getInstance()->addSwap(blocks[y][x], blocks[y][x + 1]);

			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) //swaps the main block with the top block
		{
			if (pressed2 == false && ch->getCursorY() < GameStateObject::boardHeight - 1) // cant switch if top block is offscreen
			{
				ClientManager::getInstance().requestSwap(ch->getCursorY(), ch->getCursorX(), ch->getCursorY() + 1, ch->getCursorX());
				pressed2 = true;
				//ScreenManager::getInstance()->shake(.5);
				if (swapSound.getStatus() != swapSound.Playing)
					swapSound.play();
				int y = ch->getCursorY();
				int x = ch->getCursorX();

				AnimationManager::getInstance()->addSwap(blocks[y][x], blocks[y + 1][x]);
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) //swaps the main block with the bottom block
		{
			if (pressed2 == false && ch->getCursorY() > 0)//cant switch if bottom block is off screen
			{
				ClientManager::getInstance().requestSwap(ch->getCursorY(), ch->getCursorX(), ch->getCursorY() - 1, ch->getCursorX());
				pressed2 = true;
				//ScreenManager::getInstance()->shake(.5);
				if (swapSound.getStatus() != swapSound.Playing)
					swapSound.play();
				int y = ch->getCursorY();
				int x = ch->getCursorX();

				AnimationManager::getInstance()->addSwap(blocks[y][x], blocks[y - 1][x]);
			}
		}	
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
			if (pressed2 == false){
			ClientManager::getInstance().requestNewRow();
			pressed2 = true;
			}
		}
		else
			pressed2 = false; //cannot hold swap button to keep swapping


	}
	else
	{
		AnimationManager::getInstance()->clearDangerBlocks();
		dangerColumns.clear();
		dangerMark.clear();
		//SoundManager::getInstance().playMusic("Sounds/Slamstorm.ogg", false);
		SoundManager::getInstance().playMusic("Sounds/Tetris_Linear_Groove_OC_ReMix.ogg");
		reset = true;
		if (ClientManager::getInstance().isConnected)
		{
			gso = GameStateObject();
			updateBlocks();
			ScreenManager::getInstance()->switchScreen(RESULT);
		}
		else
		{
			ScreenManager::getInstance()->switchScreen(OFFLINERESULT);
		}
		
	}

	AnimationManager::getInstance()->unpauseAnimation();
	Screen::update();

    if(InputManager::getInstance()->escape)
    {
        ScreenManager::getInstance()->addScreen(close->toScreen);
		AnimationManager::getInstance()->pauseAnimation();
	}

	std::string nextRowInString = "in ";

	nextRowInString += std::to_string(ClientManager::getInstance().currentGSO.rowInsertionCountdown / 1000);
	nextRowInString += " seconds";
	nextRowIn->message.setString(nextRowInString);
}
void NetworkedSinglePlayer::updateBlocks()
{
	int color;
	//gso.Print();
	//std::cout << "changing it on the screen now";
	//gso.Print();
	//take this new gso and apply it to our board
	//fill in temp rows
	for (int i = 0; i < gso.boardWidth; i++)
	{
		color = gso.tempRow[i];
		sf::Color c(0, 0, 0, 200);
		switch (color)
		{
			//if its a 0 its empty?
		case 0:
		{
			nextBlocks[i].setFillColor(sf::Color::Transparent);
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(sf::Color::Transparent);

			break;
		}
		case 1: //blue case
		{
			c.b = 128;
			nextBlocks[i].setFillColor(c);
			c.a = 255;
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(c);

			break;
		}
		case 2: //red case
		{
			c.r = 255;
			nextBlocks[i].setFillColor(c);
			c.a = 255;
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(c);

			break;
		}
		case 3://yellow case
		{
			c.r = 255;
			c.g = 140;
			nextBlocks[i].setFillColor(c);
			c.a = 255;
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(c);

			break;
		}
		case 4:// purple case
		{
			c.r = 148;
			c.b = 211;
			nextBlocks[i].setFillColor(c);
			c.a = 255;
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(c);

			break;
		}
		case 5://green case
		{
			c.r = 50;
			c.g = 205;
			c.b = 50;
			nextBlocks[i].setFillColor(c);
			c.a = 255;
			nextBlocks[i].setOutlineThickness(-blockSizeX / 8);
			nextBlocks[i].setOutlineColor(c);

			break;
		}
		default: //error case
		{
			nextBlocks[i].setFillColor(sf::Color::Black);
			break;
		}
		}

	}
	//fill in main board
	for (int i = 0; i < gso.boardHeight; i++)
	{
		for (int j = 0; j < gso.boardWidth; j++)
		{
			color = gso.gameBoard[i][j];
			sf::Color c(0, 0, 0, 200);
			switch (color)
			{
				//if its a 0 its empty?
			case 0:
			{
				blocks[i][j].setFillColor(sf::Color::Transparent);
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(sf::Color::Transparent);
				
				break;
			}
			case 1: //blue case
			{
				c.b = 128;
				blocks[i][j].setFillColor(c);
				c.a = 255;
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(c);
				
				break;
			}
			case 2: //red case
			{
				c.r = 255;
				blocks[i][j].setFillColor(c);
				c.a = 255;
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(c);
				
				break;
			}
			case 3://yellow case
			{
				c.r = 255;
				c.g = 140;
				blocks[i][j].setFillColor(c);
				c.a = 255;
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(c);
				
				break;
			}
			case 4:// purple case
			{
				c.r = 148;
				c.b = 211;
				blocks[i][j].setFillColor(c);
				c.a = 255;
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(c);
				
				break;
			}
			case 5://green case
			{
				c.r = 50;
				c.g = 205;
				c.b = 50;
				blocks[i][j].setFillColor(c);
				c.a = 255;
				blocks[i][j].setOutlineThickness(-blockSizeX / 8);
				blocks[i][j].setOutlineColor(c);
				
				break;
			}
			default: //error case
			{
				blocks[i][j].setFillColor(sf::Color::Black);
				break;
			}
			}
		}
	}
}



void NetworkedSinglePlayer::draw()
{
	if (!startTimer)
	{
		initialTime = std::clock();
		startTimer = true;
	}
	else
	{
		elapsed = std::clock();
		int passed = (elapsed - initialTime) / (double)CLOCKS_PER_SEC;
		int displayMinutes = passed / 60;
		int displaySeconds = passed % 60;
		std::string toDisplay;

		if (displaySeconds <= 9)
			toDisplay =  std::to_string(displayMinutes) + " : " + "0" + std::to_string(displaySeconds);
		else
			toDisplay = std::to_string(displayMinutes) + " : " + std::to_string(displaySeconds);
		elapsedTime->message.setString(toDisplay);
	}

	if (ClientManager::getInstance().currentGSO.rowInsertionPaused)
		isPaused->message.setString("|\|");
	else
		isPaused->message.setString("");

	name->message.setString(ClientManager::getInstance().player.username);
	scoreToBeat->message.setString(std::to_string(ClientManager::getInstance().player.highScore));
	Screen::draw();

	GraphicsManager::getInstance()->window.draw(rec);
	GraphicsManager::getInstance()->window.draw(tempRowRec);
	//draw the temp row and overlay the sprite on top
	for (int i = 0; i < GameStateObject::boardWidth; i++)
	{
		GraphicsManager::getInstance()->window.draw(nextBlocks[i]);
		blockShape.setPosition(nextBlocks[i].getPosition());
		switch (gso.tempRow[i])
		{
		case 1: //blue case
			blockShape.setTexture(*_getTexture("Textures/bluestar.png"));
			//defualt textures are 24 by 24 so scale them to match block size
			blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
			GraphicsManager::getInstance()->window.draw(blockShape);
			break;
		case 2: //red case
			blockShape.setTexture(*_getTexture("Textures/redgear.png"));
			blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
			GraphicsManager::getInstance()->window.draw(blockShape);
			break;
		case 3:// yellow case
			blockShape.setTexture(*_getTexture("Textures/yellowsquare.png"));
			blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
			GraphicsManager::getInstance()->window.draw(blockShape);
			break;
		case 4://purple
			blockShape.setTexture(*_getTexture("Textures/purplespade.png"));
			blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
			GraphicsManager::getInstance()->window.draw(blockShape);
			break;
		case 5://green
			blockShape.setTexture(*_getTexture("Textures/greentriangle.png"));
			blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
			GraphicsManager::getInstance()->window.draw(blockShape);
			break;
		default:
			break;
		}
	}
	//draw the board and overlay the sprite on top
	for (int i = 0; i < GameStateObject::boardHeight; i++)
	{
		for (int j = 0; j < GameStateObject::boardWidth; j++)
		{
			GraphicsManager::getInstance()->window.draw(blocks[i][j]);
			
			if (!dangerColumns.empty() && dangerColumns.at(dangerColumnCounter).first == i && dangerColumns.at(dangerColumnCounter).second == j)
			{
				if (dangerColumnCounter < dangerColumns.size() - 1)
					dangerColumnCounter++;
				else
					dangerColumnCounter = 0;
			}
			else
			{
				//set the shape on the block
				blockShape.setPosition(blocks[i][j].getPosition());
				//std::cout << "block: " << i << ", " << j << " is at position" << blocks[i][j].getPosition().x << " " << blocks[i][j].getPosition().y << std::endl;
				switch (gso.gameBoard[i][j])
				{
					//
				case 1: //blue case
					blockShape.setTexture(*_getTexture("Textures/bluestar.png"));
					//defualt textures are 24 by 24 so scale them to match block size
					blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
					GraphicsManager::getInstance()->window.draw(blockShape);
					break;
				case 2: //red case
					blockShape.setTexture(*_getTexture("Textures/redgear.png"));
					blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
					GraphicsManager::getInstance()->window.draw(blockShape);
					break;
				case 3:// yellow case
					blockShape.setTexture(*_getTexture("Textures/yellowsquare.png"));
					blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
					GraphicsManager::getInstance()->window.draw(blockShape);
					break;
				case 4://purple
					blockShape.setTexture(*_getTexture("Textures/purplespade.png"));
					blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
					GraphicsManager::getInstance()->window.draw(blockShape);
					break;
				case 5://green
					blockShape.setTexture(*_getTexture("Textures/greentriangle.png"));
					blockShape.setScale(sf::Vector2f((float)blockSizeX / 24, (float)blockSizeY / 24));
					GraphicsManager::getInstance()->window.draw(blockShape);
					break;
				default:
					break;
				}
			}
		}
	}
	

	GraphicsManager::getInstance()->window.draw(ch->getMainCursor()); //draws main cursor
	if (ch->getCursorX() > 0)
		GraphicsManager::getInstance()->window.draw(ch->getLeftCursor()); //draws left cursor
	if (ch->getCursorX() < GameStateObject::boardWidth - 1)
		GraphicsManager::getInstance()->window.draw(ch->getRightCursor()); //draws right cursor
	if (ch->getCursorY() < GameStateObject::boardHeight - 1)
		GraphicsManager::getInstance()->window.draw(ch->getTopCursor()); //draws top cursor
	if (ch->getCursorY() > 0)
		GraphicsManager::getInstance()->window.draw(ch->getBottomCursor()); //draws bottom cursor
	highScore->message.setColor(sf::Color::Black);
	oldHighScore->message.setColor(sf::Color::Black);
	scoreToBeat->message.setColor(sf::Color::Black);
	score->message.setColor(sf::Color::Black);
	username->message.setColor(sf::Color::Black);
	name->message.setColor(sf::Color::Black);
	next->message.setColor(sf::Color::Black);
	row->message.setColor(sf::Color::Black);
	elapsedTime->message.setColor(sf::Color::Black);
	nextRowIn->message.setColor(sf::Color::Black);
	isPaused->message.setColor(sf::Color::Black);

	miku.draw(GraphicsManager::getInstance()->window);
}


void NetworkedSinglePlayer::reload()
{
    BlockShowerManager::getInstance()->fade.state = FadeStates::FADING_OUT;
	startTimer = false;
}