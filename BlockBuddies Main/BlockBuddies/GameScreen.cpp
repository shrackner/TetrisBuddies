#include "GameScreen.hpp"
#include "ClientManager.h"
#include "SoundManager.h"
#include "BlockShowerManager.hpp"
#include <iostream>
#include "InputManager.hpp"

// To be filled in at the gameplay people's discretion
GameScreen::GameScreen()
	: pressed(false), pressed2(false), reset(false), raiseBlock(false)
{
	bh = new BlockHandler(GraphicsManager::getInstance()->window.getSize().x, GraphicsManager::getInstance()->window.getSize().y);
	ch = new CursorHandler(bh->SCREENWIDTH, bh->SCREENHEIGHT, GraphicsManager::getInstance()->window.getSize().x, GraphicsManager::getInstance()->window.getSize().y,25, 0);// 0 is the offset, 25 is the block size	
	//draws a large rectangle around the game screen
	rec.setSize(sf::Vector2f(bh->SCREENWIDTH, bh->SCREENHEIGHT));
	rec.setFillColor(sf::Color::Transparent);
	rec.setPosition(GraphicsManager::getInstance()->window.getSize().x/2 - bh->SCREENWIDTH/2, GraphicsManager::getInstance()->window.getSize().y/2 - bh->SCREENHEIGHT/2	);
	rec.setOutlineThickness(25);
	rec.setOutlineColor(sf::Color::Black);
}

void GameScreen::reload()
{
    BlockShowerManager::getInstance()->fade.state = FadeStates::FADING_OUT;
}

void GameScreen::update()
{
	Screen::update();
	if (reset)
	{
		bh = new BlockHandler(GraphicsManager::getInstance()->window.getSize().x, GraphicsManager::getInstance()->window.getSize().y);
		ch = new CursorHandler(bh->SCREENWIDTH, bh->SCREENHEIGHT, GraphicsManager::getInstance()->window.getSize().x, GraphicsManager::getInstance()->window.getSize().y,25, 0);
		raiseBlock = false;
		reset = false;
		clock.restart();
	}

	if (!bh->GameOver())
	{

		time = clock.getElapsedTime();
		//Timed Block Rises, boolean raiseBlock prevents multiple block raises when time has multiple of same number because of rounding
		if (std::round(time.asSeconds() * 10) / 10 <= 0){}
		else if (std::round(time.asSeconds() * 10) / 10 < 60) //every 10 seconds for the 1st minute
		{
			if (fmod(std::round(time.asSeconds() * 10) / 10, 10) == 0 && !raiseBlock)
			{
				bh->raiseBlocks();
				ch->Up(sf::Keyboard::Key::Up);
				raiseBlock = true;
			}
			else if (fmod(std::round(time.asSeconds() * 10) / 10, 10) != 0 && raiseBlock)
				raiseBlock = false;
		}
		else if (std::round(time.asSeconds() * 10) / 10 < 120) // every 6 seconds for the 2nd minute
		{
			if (fmod(std::round(time.asSeconds() * 10) / 10, 6) == 0 && !raiseBlock)
			{
				bh->raiseBlocks();
				ch->Up(sf::Keyboard::Key::Up);
				raiseBlock = true;
			}
			else if (fmod(std::round(time.asSeconds() * 10) / 10, 6) != 0 && raiseBlock)
				raiseBlock = false;
		}
		else if (std::round(time.asSeconds() * 10) / 10 > 120) // every 3 seconds after the 2nd minute
		{
			if (fmod(std::round(time.asSeconds() * 10) / 10, 3) == 0 && !raiseBlock)
			{
				bh->raiseBlocks();
				ch->Up(sf::Keyboard::Key::Up);
				raiseBlock = true;
			}
			else if (fmod(std::round(time.asSeconds() * 10) / 10, 3) != 0 && raiseBlock)
				raiseBlock = false;
		}


		//keyboard movement for cursor
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
			if (pressed2 == false)
			{
				bh->swapBlocksHorizL(ch->getMainCursor().getPosition().x, ch->getMainCursor().getPosition().y);
				pressed2 = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) //swaps the main block with the right block
		{
			if (pressed2 == false)
			{
				bh->swapBlocksHorizR(ch->getMainCursor().getPosition().x, ch->getMainCursor().getPosition().y);
				pressed2 = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) //swaps the main block with the top block
		{
			if (pressed2 == false)
			{
				bh->swapBlocksVertT(ch->getMainCursor().getPosition().x, ch->getMainCursor().getPosition().y);
				pressed2 = true;

			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) //swaps the main block with the bottom block
		{
			if (pressed2 == false)
			{
				bh->swapBlocksVertB(ch->getMainCursor().getPosition().x, ch->getMainCursor().getPosition().y);
				pressed2 = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) //creates a new line of blocks at the bottom
		{
			if (pressed2 == false)
			{
				bh->raiseBlocks();
				ch->Up(sf::Keyboard::Key::Up);
				pressed2 = true;
			}
		}
		else
			pressed2 = false; //cannot hold swap button to keep swapping
	}
	else
	{
		SoundManager::getInstance().playMusic("Sounds/Slamstorm.ogg");
		BlockHandler::displayScore = bh->score;
		delete bh;
		delete ch;
		reset = true;

        if(ClientManager::getInstance().isConnected)
            ScreenManager::getInstance()->switchScreen(RESULT);
        else
            ScreenManager::getInstance()->switchScreen(OFFLINERESULT);
	}

    if(InputManager::getInstance()->escape)
    {
        ScreenManager::getInstance()->addScreen(Screens::OFFLINECLOSE);
    }
}

void GameScreen::draw()
{
	Screen::draw();
	if (!reset)
	{
		//draws a border around the game
		GraphicsManager::getInstance()->window.draw(rec);

		//draws blocks on screen
		for (int i = 0; i < bh->SCREENHEIGHT / 25; i++)
		{
			for (int j = 0; j < bh->SCREENWIDTH / 25; j++)
			{
				GraphicsManager::getInstance()->window.draw(bh->getBlocks(j, i));
			}
		}

		GraphicsManager::getInstance()->window.draw(ch->getMainCursor()); //draws main cursor
		GraphicsManager::getInstance()->window.draw(ch->getLeftCursor()); //draws left cursor
		GraphicsManager::getInstance()->window.draw(ch->getRightCursor()); //draws right cursor
		GraphicsManager::getInstance()->window.draw(ch->getTopCursor()); //draws top cursor
		GraphicsManager::getInstance()->window.draw(ch->getBottomCursor()); //draws bottom cursor
	}
}