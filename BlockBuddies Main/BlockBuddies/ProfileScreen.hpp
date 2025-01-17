#ifndef PROFILESCREEN_HPP
#define PROFILESCREEN_HPP
#include "TextBox.hpp"
#include "Screen.hpp"
#include "Button.hpp"
#include "Section.hpp"

// Screen that displays the logged in user's statistics
// and stats. In the future we may enhance this to allow
// for searching up rankings and the likes
class ProfileScreen: public Screen
{
public:
	ProfileScreen();
	void update();
	void draw();
	void reload();

private:
    Section* backSection;
	Section* section;
    TextBox* title;
    TextBox* status;

	TextBox* usernameTag;
	TextBox* highscoreTag;
	TextBox* gamesplayedTag;
	TextBox* gameswonTag;
	TextBox* winpercentageTag;

	TextBox* nameTag;
	TextBox* hsTag;
	TextBox* gpTag;
	TextBox* gwTag;
	TextBox* wpTag;

    Button* home;
};

#endif//PROFILESCREEN_HPP