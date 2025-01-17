#ifndef SECTION_HPP
#define SECTION_HPP

#include "UIElement.hpp"
#include "SFML/Graphics.hpp"
#include "GraphicsManager.hpp"
#include <vector>

// This is a UIElement that is basically responsible for just
// displaying a colored rectangle. It's basically a glorified
// RectangleShape object, positioned and centered properly
class Section : public UIElement
{
public:
	// Sections are always drawn from its center
	Section(float posX,
		    float posY,
			float width,
			float height,
            sf::Color color = GraphicsManager::getInstance()->sectionColor,
            bool drawBorder = false);
	void update();
	void draw();
	sf::RectangleShape boundingRect;

private:
    sf::Color color;
    std::vector<sf::RectangleShape> blockBorder;
};

#endif//SECTION_HPP