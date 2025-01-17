#include "AnimationManager.hpp"
#include "TextureManager.hpp"
#include "GraphicsManager.hpp"
#include "ScreenManager.hpp"
#include "SoundManager.h"

#include <iostream>
AnimationManager* AnimationManager::instance;

AnimationManager* AnimationManager::getInstance()
{
	if (!instance)
		instance = new AnimationManager();

	return instance;
}

void AnimationManager::init()
{
	speed = (rand() % 300 + 200) / 100.0f;
	dangerUp = true;
	pause = false;
	clearAddFinish = false;
	clearSound.setBuffer(*SoundManager::getInstance().getSound("blast"));
	matchFlashing.setBuffer(*SoundManager::getInstance().getSound("charge"));
	matchFlashing2.setBuffer(*SoundManager::getInstance().getSound("charge"));
	matchFlashing3.setBuffer(*SoundManager::getInstance().getSound("charge"));
}

void AnimationManager::update()
{
	stime = sclock.getElapsedTime();
	if (!ctime.empty() && !cclock.empty())
	{
		for (int i = 0; i < ctime.size(); i++)
			ctime.at(i) = cclock.at(i).getElapsedTime();
	}
	swap();
	clear();
	danger();
	draw();
}

void AnimationManager::draw()
{
	if (!swapSprites.empty())
	{
		GraphicsManager::getInstance()->window.draw(swapSprites.at(0));
		GraphicsManager::getInstance()->window.draw(swapSprites.at(1));
	}
	if (!clearBlocks.empty() && !cclock.empty() && !ctime.empty())
	{
		for (int i = 0; i < clearBlocks.size(); i++)
		{
			if (clearBlocks.at(i).first.getPosition().y > GraphicsManager::getInstance()->window.getSize().y)
			{
				clearBlocks.erase(clearBlocks.begin() + i);
				acceleration.erase(acceleration.begin() + i);
				cclock.erase(cclock.begin() + i);
				ctime.erase(ctime.begin() + i);
			}
			else
				GraphicsManager::getInstance()->window.draw(clearBlocks.at(i).first);
		}
	}

}

void AnimationManager::danger()
{
	if (!dangerSprites.empty() && !pause)
	{
		for (int row = 0; row < dangerSprites.size(); row++)
		{
			float x = dangerSprites.at(row).first.getPosition().x;
			float y = dangerSprites.at(row).first.getPosition().y;
			if (dangerUp) //its going up
			{
				y = y - dangerSprites.at(row).second;
				dangerSprites.at(row).second = dangerSprites.at(row).second - 1;
			}
			else //its going down
			{
				dangerSprites.at(row).second = dangerSprites.at(row).second + 1;
				y = y + dangerSprites.at(row).second;
			}
			dangerSprites.at(row).first.setPosition(x, y);

			GraphicsManager::getInstance()->window.draw(dangerSprites.at(row).first);
		}

		if (dangerSprites.at(0).second == 0)
			dangerUp = false;
		else if (dangerSprites.at(0).second == 5)
			dangerUp = true;

	}

}

void AnimationManager::clear()
{
	if (!clearBlocks.empty() && !cclock.empty() && !ctime.empty())
	{
		for (int i = 0; i < clearBlocks.size(); i++)
		{
			if (ctime.at(i).asMilliseconds() > 2000)
			{
				acceleration.at(i) = acceleration.at(i) + 9.8*.05;
				if (clearBlocks.at(i).second % 2 == 0)
					clearBlocks.at(i).first.setPosition(clearBlocks.at(i).first.getPosition().x + 10, clearBlocks.at(i).first.getPosition().y + acceleration.at(i));
				else
					clearBlocks.at(i).first.setPosition(clearBlocks.at(i).first.getPosition().x - 10, clearBlocks.at(i).first.getPosition().y + acceleration.at(i));

				sf::Color color = clearBlocks.at(i).first.getFillColor();
				color.a = 255;
				clearBlocks.at(i).first.setFillColor(color);
			}
			else
			{
				sf::Color color = clearBlocks.at(i).first.getFillColor();
				color.a = color.a - ctime.at(i).asSeconds()*blockSize;
				clearBlocks.at(i).first.setFillColor(color);

				if (ctime.at(i).asMilliseconds() <= 1999 && ctime.at(i).asMilliseconds() >= 1970)
				{
					ScreenManager::getInstance()->shake(.5);
					clearSound.play();
				}
			}
		}
	}

}

void AnimationManager::swap()
{
	if (stime.asMilliseconds() < 300 && !swapSprites.empty())
	{
		//swapping with top or bottom
		if (blockx == blockx2 && !stop)
		{
			if (blocky > blocky2) //this means swapping with the top block
			{
				if (swapSprites.at(0).getPosition().y > blocky2)
				{
					swapSprites.at(0).setPosition(blockx, swapSprites.at(0).getPosition().y - stime.asSeconds()*blockSize);
					if (swapSprites.at(0).getPosition().y < blocky2)
						swapSprites.at(0).setPosition(blockx, blocky2);
				}
				else if (swapSprites.at(0).getPosition().y == blocky2)
				{
					swapSprites.at(1).setPosition(blockx, swapSprites.at(1).getPosition().y + stime.asSeconds() * blockSize);
					if (swapSprites.at(1).getPosition().y > blocky)
					{
						swapSprites.at(1).setPosition(blockx2, blocky);
						stop = true;
					}
				}
			}
			else if (blocky < blocky2 && !stop) //this means swapping with the bottom block
			{
				if (swapSprites.at(0).getPosition().y < blocky2)
				{
					swapSprites.at(0).setPosition(blockx, swapSprites.at(0).getPosition().y + stime.asSeconds() * blockSize);
					if (swapSprites.at(0).getPosition().y > blocky2)
						swapSprites.at(0).setPosition(blockx, blocky2);
				}
				else if (swapSprites.at(0).getPosition().y == blocky2)
				{
					swapSprites.at(1).setPosition(blockx, swapSprites.at(1).getPosition().y - stime.asSeconds() * blockSize);
					if (swapSprites.at(1).getPosition().y < blocky)
					{
						swapSprites.at(1).setPosition(blockx2, blocky);
						stop = true;
					}
				}
			}//end else if
		}//end if

		//swapping with left or right
		if (blocky == blocky2 && !stop)
		{
			if (blockx > blockx2) //this means swapping with the left block
			{
				if (swapSprites.at(0).getPosition().x > blockx2)
				{
					swapSprites.at(0).setPosition(swapSprites.at(0).getPosition().x - stime.asSeconds() * blockSize, blocky);
					if (swapSprites.at(0).getPosition().x < blockx2)
						swapSprites.at(0).setPosition(blockx2, blocky);
				}
				else if (swapSprites.at(0).getPosition().x == blockx2)
				{
					swapSprites.at(1).setPosition(swapSprites.at(1).getPosition().x + stime.asSeconds() * blockSize, blocky);
					if (swapSprites.at(1).getPosition().x > blockx)
					{
						swapSprites.at(1).setPosition(blockx, blocky2);
						stop = true;
					}
				}
			}
			else if (blockx < blockx2 && !stop) //this means swapping with the right block
			{
				if (swapSprites.at(0).getPosition().x < blockx2)
				{
					swapSprites.at(0).setPosition(swapSprites.at(0).getPosition().x + stime.asSeconds() * blockSize, blocky);
					if (swapSprites.at(0).getPosition().x > blockx2)
						swapSprites.at(0).setPosition(blockx2, blocky);
				}
				else if (swapSprites.at(0).getPosition().x == blockx2)
				{
					swapSprites.at(1).setPosition(swapSprites.at(1).getPosition().x - stime.asSeconds() * blockSize, blocky);
					if (swapSprites.at(1).getPosition().x < blockx)
					{
						swapSprites.at(1).setPosition(blockx, blocky2);
						stop = true;
					}
				}
			}//end else if
		}//end if

	}
	else
	{
		sclock.restart();
		swapSprites.clear();
		stop = false;
	}
}

void AnimationManager::addSwap(sf::RectangleShape r, sf::RectangleShape r2)
{

	sf::Sprite sprite;
	sprite.setPosition(r.getPosition());
	swapSprites.push_back(sprite);
	s.push_back(r);

	sf::Sprite sprite2;
	sprite2.setPosition(r2.getPosition());
	swapSprites.push_back(sprite2);
	s.push_back(r);

	setTextureSwap(0);
	setTextureSwap(1);

	blockx = r.getPosition().x;
	blocky = r.getPosition().y;
	blockx2 = r2.getPosition().x;
	blocky2 = r2.getPosition().y;

	s.clear();
}

void AnimationManager::addClear(sf::RectangleShape b)
{
	ctemp.push_back(b);
}

void AnimationManager::addDanger(sf::RectangleShape d)
{
	sf::Sprite sprite;
	sprite.setPosition(d.getPosition());
	dangerBlocks.push_back(d.getFillColor());
	dangerSprites.push_back(std::make_pair(sprite, 5));
}

void AnimationManager::setBlockSize(int size)
{
	blockSize = size;
}

void AnimationManager::setClearingAdd()
{
	if (matchFlashing.getStatus() != matchFlashing.Playing)
		matchFlashing.play();
	else if (matchFlashing2.getStatus() != matchFlashing2.Playing)
		matchFlashing2.play();
	else if (matchFlashing3.getStatus() != matchFlashing3.Playing)
		matchFlashing3.play();

	sf::Clock clock;
	sf::Time time;
	time = clock.getElapsedTime();
	for (int i = 0; i < ctemp.size(); i++)
	{
		clearBlocks.push_back(std::make_pair(ctemp.at(i), i));
		acceleration.push_back(-speed);
		cclock.push_back(clock);
		ctime.push_back(time);
	}

	ctemp.clear();
}

void AnimationManager::setTextureSwap(int index)
{
	if (s.at(index).getFillColor().r == 0 && s.at(index).getFillColor().g == 0 && s.at(index).getFillColor().b == 128) //blue case
	{
		swapSprites.at(index).setTexture(*_getTexture("Textures/bluestar.png"));
		//defualt textures are 24 by 24 so scale them to match block size
		swapSprites.at(index).setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
	}
	else if (s.at(index).getFillColor().r == 255 && s.at(index).getFillColor().g == 0 && s.at(index).getFillColor().b == 0) //red case
	{
		swapSprites.at(index).setTexture(*_getTexture("Textures/redgear.png"));
		swapSprites.at(index).setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
	}
	else if (s.at(index).getFillColor().r == 255 && s.at(index).getFillColor().g == 140 && s.at(index).getFillColor().b == 0)// yellow case
	{
		swapSprites.at(index).setTexture(*_getTexture("Textures/yellowsquare.png"));
		swapSprites.at(index).setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
	}
	else if (s.at(index).getFillColor().r == 148 && s.at(index).getFillColor().g == 0 && s.at(index).getFillColor().b == 211)//purple
	{
		swapSprites.at(index).setTexture(*_getTexture("Textures/purplespade.png"));
		swapSprites.at(index).setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
	}
	else if (s.at(index).getFillColor().r == 50 && s.at(index).getFillColor().g == 205 && s.at(index).getFillColor().b == 50)//green
	{
		swapSprites.at(index).setTexture(*_getTexture("Textures/greentriangle.png"));
		swapSprites.at(index).setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
	}

}

void AnimationManager::setTextureDanger()
{
	for (int index = 0; index < dangerSprites.size(); index++)
	{
		if (dangerBlocks.at(index).r == 0 && dangerBlocks.at(index).g == 0 && dangerBlocks.at(index).b == 128) //blue case
		{
			dangerSprites.at(index).first.setTexture(*_getTexture("Textures/bluestar.png"));
			//defualt textures are 24 by 24 so scale them to match block size
			dangerSprites.at(index).first.setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
		}
		else if (dangerBlocks.at(index).r == 255 && dangerBlocks.at(index).g == 0 && dangerBlocks.at(index).b == 0) //red case
		{
			dangerSprites.at(index).first.setTexture(*_getTexture("Textures/redgear.png"));
			dangerSprites.at(index).first.setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
		}
		else if (dangerBlocks.at(index).r == 255 && dangerBlocks.at(index).g == 140 && dangerBlocks.at(index).b == 0)// yellow case
		{
			dangerSprites.at(index).first.setTexture(*_getTexture("Textures/yellowsquare.png"));
			dangerSprites.at(index).first.setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
		}
		else if (dangerBlocks.at(index).r == 148 && dangerBlocks.at(index).g == 0 && dangerBlocks.at(index).b == 211)//purple
		{
			dangerSprites.at(index).first.setTexture(*_getTexture("Textures/purplespade.png"));
			dangerSprites.at(index).first.setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
		}
		else if (dangerBlocks.at(index).r == 50 && dangerBlocks.at(index).g == 205 && dangerBlocks.at(index).b == 50)//green
		{
			dangerSprites.at(index).first.setTexture(*_getTexture("Textures/greentriangle.png"));
			dangerSprites.at(index).first.setScale(sf::Vector2f((float)blockSize / 24, (float)blockSize / 24));
		}
	}

}

void AnimationManager::clearDangerBlocks()
{
	dangerBlocks.clear();
	dangerSprites.clear();
	dangerUp = true;
}

void AnimationManager::pauseAnimation()
{
	pause = true;
}

void AnimationManager::unpauseAnimation()
{
	pause = false;
}