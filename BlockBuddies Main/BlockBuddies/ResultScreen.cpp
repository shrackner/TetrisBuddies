#include "ResultScreen.hpp"
#include "InputManager.hpp"
#include "ScreenManager.hpp"
#include "ClientManager.h"
#include "SoundManager.h"
#include "BlockShowerManager.hpp"
#include <sstream>
ResultScreen::ResultScreen()
	:scoreString("5000"),
	 
    backSection(new Section(0.0f,
	                     0.0f,
						 420.0f,
						 410.0f,
                         GraphicsManager::getInstance()->buttonColor,
                         true)),

    section(new Section(0.0f,
                         0.0f,
                         400.0f,
                         390.0f)),
     
     title(new TextBox("GAME OVER",
                       0.0f,
                       -165.0f + 45.0f,
                       300.0f,
                       Alignments::CENTER,
                       true)),

     status(new TextBox("Game over! Play another singleplayer game?",
                        0.0f,
                        -90.0f + 45.0f,
                        300.0f,
                        Alignments::CENTER,
                        false,
                        true)),

     scoreTag(new TextBox("Score:",
                          0.0f,
                          -40.0f + 45.0f,
                          Alignments::LEFT)),

     score(new TextBox("0",
                       0.0f,
                       0.0f + 45.0f,
                       300.0f,
                       Alignments::CENTER,
                       true)),

	 game(new Button(Screens::ONLINESINGLE,
                     "Play Again",
                     87.5f,
                     75.0f + 45.0f,
                     150.0f,
                     50.0f)),

     home(new Button(Screens::HOME,
	                 "Home",
                     -87.5f,
                     75.0f + 45.0f,
                     150.0f,
                     50.0f))
{
    UIElements.push_back(backSection);
    UIElements.push_back(section);
    UIElements.push_back(title);
    UIElements.push_back(status);
    UIElements.push_back(scoreTag);
    UIElements.push_back(score);
    UIElements.push_back(game);
    UIElements.push_back(home);
	scoreString = "5000";

    selectables = { game,
                    home };
	updated = false;

    textWrapped.insert(std::pair<UIElement*, TextBox>(NULL, *status));

    status->message.setString("Play another singleplayer game.");
    status->textWrap();
    textWrapped.insert(std::pair<UIElement*, TextBox>(game, *status));

    status->message.setString("Return to the home screen.");
    status->textWrap();
    textWrapped.insert(std::pair<UIElement*, TextBox>(home, *status));

    status->message.setString("Quit the game.");
    status->textWrap();
    textWrapped.insert(std::pair<UIElement*, TextBox>(close, *status));
}

void ResultScreen::update()
{
    Screen::update();

    if (home->isActivated ||
        (InputManager::getInstance()->enter && home->isSelected))
    {
        ScreenManager::getInstance()->switchScreen(home->toScreen);
    }
    
    else if (game->isActivated ||
             (InputManager::getInstance()->enter && game->isSelected))
    {
		ClientManager::getInstance().requestStartGame();
        ScreenManager::getInstance()->switchScreen(game->toScreen);
	}
	
    int Number = ClientManager::getInstance().lastScore;
    scoreString = static_cast<std::ostringstream*>(&(std::ostringstream() << Number))->str();
    score->message.setString(scoreString);
    updated = true;

    sf::Vector2f mousePosition = sf::Vector2f(sf::Mouse::getPosition(GraphicsManager::getInstance()->window).x,
                                              sf::Mouse::getPosition(GraphicsManager::getInstance()->window).y);

    if (home->boundingRect.getGlobalBounds().contains(mousePosition))
        *status = textWrapped.at(home);

    else if(game->boundingRect.getGlobalBounds().contains(mousePosition))
        *status = textWrapped.at(game);

    else if(close->boundingRect.getGlobalBounds().contains(mousePosition))
        *status = textWrapped.at(close);

    else
        *status = textWrapped.at(NULL);

    if(InputManager::getInstance()->escape)
    {
        ScreenManager::getInstance()->addScreen(close->toScreen);
    }
}

void ResultScreen::draw()
{
    Screen::draw();
}

void ResultScreen::reload()
{
    BlockShowerManager::getInstance()->fade.state = FadeStates::FADING_IN;
}