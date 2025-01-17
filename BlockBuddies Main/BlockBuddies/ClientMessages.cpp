#include "ClientMessages.hpp"
#include "MessageType.h"

sf::Packet ClientMessages::StartPacket(){
	sf::Packet ret;
	ret << PacketDecode::PACKET_START;
	return ret;
}

sf::Packet ClientMessages::SwapPacket(sf::Uint8 p1row, sf::Uint8 p1col, sf::Uint8 p2row, sf::Uint8 p2col){
	
	sf::Packet ret;
	ret << PacketDecode::PACKET_SWAP << p1row << p1col << p2row << p2col;
	return ret;
}

sf::Packet ClientMessages::NewRowPacket(){

	sf::Packet ret;
	ret << PacketDecode::PACKET_REQUESTNEWROW;
	return ret;
}

sf::Packet ClientMessages::RequestMultiplayer(){

	sf::Packet ret;
	ret << PacketDecode::PACKET_MULTIPLAYERQUEUE;
	return ret;
}

//pass it a packet full of game state data, and decode it into a passed in GameStateObject
void ClientMessages::DecodeGameState(sf::Packet& decodeMe, GameStateObject& gso){
	
	decodeMe >> gso;

	//gso.PrintToFile();

	/*
	sf::Uint32 sc;
	decodeMe >> sc;
	gso.score = sc;

	sf::Uint8 square = 9;

	for (int colNum = 0; colNum < gso.boardWidth; colNum++)
	{
		decodeMe >> square;
		//std::cout << "Decoded square: " << (int)square << std::endl;
		gso.tempRow[colNum] = square;
	}

	for (int rowNum = 0; rowNum < gso.boardHeight; rowNum++) {
		for (int colNum = 0; colNum < gso.boardWidth; colNum++)
		{
			decodeMe >> square;
			//std::cout << "Decoded square: " << (int)square << std::endl;
			gso.gameBoard[rowNum][colNum] = square;
		}
	}
	*/
}



bool ClientMessages::ProcessMessage(sf::Packet& toProcess, GameStateObject& gso){

	PacketDecode decode;
	sf::Uint8 command;
	toProcess >> command;
	decode = PacketDecode(command);

	//Current game state
	if (command == PacketDecode::PACKET_GAMESTATE){ 
		DecodeGameState(toProcess, gso);
		return true;
	}


	//Game Started
	else if (command == PacketDecode::PACKET_START){
		//StartGame() function needs to be called
		//std::cout << "Game has started!" << std::endl;
		return true;
	}

	//Game Over
	else if (command == PacketDecode::PACKET_GAMEOVER){
		//call GameOver()
		//std::cout << "Game Over!" << std::endl;
		return true;
	}

	return false;
}