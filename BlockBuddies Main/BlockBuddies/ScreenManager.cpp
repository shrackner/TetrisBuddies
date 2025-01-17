#include "ScreenManager.hpp"
#include "LoginScreen.hpp"
#include "RegisterScreen.hpp"
#include "HomeScreen.hpp"
#include "ProfileScreen.hpp"
#include "GameScreen.hpp"
#include "ResultScreen.hpp"
#include "InputManager.hpp"
#include "OfflineResultScreen.hpp"
#include "CloseScreen.hpp"
#include "OfflineCloseScreen.hpp"
#include "MultiplayerScreen.hpp"
#include "NetworkedSinglePlayer.h"
#include "QueueScreen.hpp"
#include "Game.hpp"
#include "NotificationScreen.hpp"
#include "InstructionScreen.hpp"
#include "MultiplayerResultScreen.hpp"

ScreenManager* ScreenManager::instance;

ScreenManager* ScreenManager::getInstance()
{
	if(!instance)
		instance = new ScreenManager;
	return instance;
}

void ScreenManager::init()
{
	// Initialize all the screens to a map
	screens = std::map<Screens, Screen*> { 
		                                     { Screens::LOGIN, new LoginScreen() },
											 { Screens::REGISTER, new RegisterScreen() },
											 { Screens::HOME, new HomeScreen() },
											 { Screens::PROFILE, new ProfileScreen() },
											 { Screens::GAME, new GameScreen() },
											 { Screens::RESULT, new ResultScreen() },
                                             { Screens::OFFLINERESULT, new OfflineResultScreen() },
                                             { Screens::CLOSE, new CloseScreen() },
                                             { Screens::OFFLINECLOSE, new OfflineCloseScreen() },
                                             { Screens::MULTIPLAYER, new MultiplayerScreen() },
											 { Screens::ONLINESINGLE, new NetworkedSinglePlayer() },
                                             { Screens::QUEUE, new QueueScreen() },
                                             { Screens::NOTIFICATION, new NotificationScreen() },
                                             { Screens::INSTRUCTION, new InstructionScreen() },
                                             { Screens::MULTIPLAYERRESULT, new MultiplayerResultScreen() }
	                                     };

	// Set this to something else if you want to start on a specific screen
    switchScreen(Screens::LOGIN);

	//set the initialPosition of the window
	view = GraphicsManager::getInstance()->window.getDefaultView();
}

void ScreenManager::update()
{
    int removeIndex = -1;
    // Loops backwards so we can check updates for the top screens first
    for(int i = currentScreens.size() - 1; i >= 0; i--)
    {
        if (currentScreens[i]->fade.state == FadeStates::FADED_OUT)
            removeIndex = i;

        // Only update the screen if the fade state is faded in
        else if (currentScreens[i]->fade.state == FadeStates::FADED_IN || currentScreens[i]->fade.state == FadeStates::FADING_IN)
        {
            if(ScreenManager::getInstance()->currentScreens.back()->fade.state != FadeStates::FADED_OUT
                && ScreenManager::getInstance()->currentScreens.back()->fade.state != FadeStates::FADING_OUT)
            {
                currentScreens[i]->update();
                break;
            }
        }
    }

    // Removes faded out elements, doesn't matter if this only removes
    // one at a time per loop
    //
    // Also, we must remove after the loop so we don't mess up indexing
    if(removeIndex != -1)
        currentScreens.erase(currentScreens.begin() + removeIndex);

	//screen shake
	if (shakeTimer.asMilliseconds() > 0)
	{
		view = GraphicsManager::getInstance()->window.getDefaultView();
		view.setCenter(sf::Vector2f(view.getCenter().x + rand() % 5 - 2.5f, view.getCenter().y + rand() % 5 - 2.5f));
		GraphicsManager::getInstance()->window.setView(view);
		shakeTimer -= clock.getElapsedTime();
	}
	else
    {
		GraphicsManager::getInstance()->window.setView(GraphicsManager::getInstance()->window.getDefaultView());
	}
	clock.restart();
}

void ScreenManager::draw()
{
    for (int i = 0; i < currentScreens.size(); i++)
        currentScreens[i]->draw();
}

void ScreenManager::switchScreen(const Screens toScreen)
{	
    int removeIndex = -1;
    int counter = -1;
    // Deselects and deactivate any selected elements before going to a new screen
    for(auto& screen : currentScreens)
    {
        counter++;
        screen->deselect();
        screen->deactivate();

        // Start fading out
        screen->fade.state = FadeStates::FADING_OUT;

        // In case the screen we want to go to is already inside our currentScreens,
        // We remove it and just put it in the front
        if(screen == screens[toScreen])
            removeIndex = counter;
    }

    if(removeIndex != -1)
        currentScreens.erase(currentScreens.begin() + removeIndex);

    // Add to the front, as the back will disappear
    currentScreens.push_front(screens[toScreen]);
    currentScreens[0]->fade.state = FadeStates::FADING_IN;
    currentScreens[0]->reload();
}

// For adding a notification screen or a close screen on top of the
// current one
void ScreenManager::addScreen(const Screens toScreen, const sf::String notificationMessage)
{
    currentScreens.push_back(screens[toScreen]);
    if(!notificationMessage.isEmpty())
    {
        ((NotificationScreen*) currentScreens.back())->status->message.setString(notificationMessage);
    }
    currentScreens.back()->reload();
    currentScreens[currentScreens.size() - 1]->fade.state = FadeStates::FADING_IN;
}

void ScreenManager::shake(float seconds)
{
	shakeTimer = sf::seconds(seconds);
}

// Does the necessary things for closing the game
void ScreenManager::closeGame()
{
    Game::getInstance()->isRunning = false;
    if(ClientManager::getInstance().isConnected)
        ClientManager::getInstance().closeConnection();
}