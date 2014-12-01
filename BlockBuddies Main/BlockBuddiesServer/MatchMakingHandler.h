#pragma once

#include "Player.h"
#include <queue>
#include "Message.h"
#include "Game.h"
#include <list>

class MatchMakingHandler
{
public:
	MatchMakingHandler();
	~MatchMakingHandler();

	std::queue<Player> activePlayers;
	static std::list<Game*> activeGames;
	void addMessage(Message message);
	void checkForMatches();
	void makeGame(Player p1, Player p2);

};