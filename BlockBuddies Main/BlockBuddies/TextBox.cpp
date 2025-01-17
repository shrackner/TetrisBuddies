#include "TextBox.hpp"
#include "GraphicsManager.hpp"
#include <iostream>

TextBox::TextBox(std::string message,
    float posX,
	float posY,
	float width,
	Alignments textAlignment,
	bool isTitle,
    bool borderOutline)
	:message(sf::Text(message,
    GraphicsManager::getInstance()->messageFont,
	GraphicsManager::getInstance()->messageSize)),
	boundingWidth(width * GraphicsManager::getInstance()->scale),
    borderOutline(borderOutline)
{
	// The isTitle parameter indicates whether or not the textbox is a larger title
	// e.g. "Login" for the LoginScreen, or just a normal instructions e.g. "Enter credentials below"
	// Right now, they are only differentiated by two different font sizes
	// If we would like, we could change the parameter to accept a font size, or
	// change the font of either within GraphicsManager
	if (isTitle)
		this->message.setCharacterSize(GraphicsManager::getInstance()->titleSize);

	// Sets the message color of the TextBox
	this->message.setColor(GraphicsManager::getInstance()->typeColor);

	// Determines whether it is drawn based on the TextBox's left-center or by the center
	if (textAlignment == Alignments::LEFT)
		this->message.setOrigin(GraphicsManager::getInstance()->getLeftCenter(this->message));
	else if (textAlignment == Alignments::CENTER)
		this->message.setOrigin(GraphicsManager::getInstance()->getCenter(this->message));

	// Save this alignment for if we need to reset the position
	this->textAlignment = textAlignment;

	// Sets the message position, by first going to the center of the screen and then moving 
	// a scaled distance away
	this->message.setPosition(sf::Vector2f(GraphicsManager::getInstance()->window.getSize()) / 2.0f);
	this->message.move(posX * GraphicsManager::getInstance()->scale, posY * GraphicsManager::getInstance()->scale);

	// If we need to reset the position, we need to remember where to go
	targetPosition = sf::Vector2f(floorf(posX), floorf(posY));

	textWrap();

    if(borderOutline)
    {
        sf::FloatRect messageRect = this->message.getGlobalBounds();
        border = sf::RectangleShape(sf::Vector2f(boundingWidth + 20 * GraphicsManager::getInstance()->scale, 
                                    messageRect.height + 20 * GraphicsManager::getInstance()->scale));
        border.setOrigin(GraphicsManager::getInstance()->getCenter(border));
        border.setPosition(this->message.getPosition());
        border.setOutlineColor(GraphicsManager::getInstance()->buttonColor);
        border.setOutlineThickness(2);
        border.setFillColor(sf::Color::Transparent);
    }
}

// Snippet of code for dealing with text wraps
void TextBox::textWrap()
{
	// Only textwrap if the previous and the current messages are different
	if (message.getString() != prevMessage.getString())
	{
		// If the text is greater than the given boundingWidth
		if (message.getLocalBounds().width > boundingWidth)
		{
			// We make a variable that will check from the first letter of message
			// until a letter that goes over the boundingWidth
			sf::Text checkBound(sf::Text("",
				                GraphicsManager::getInstance()->messageFont,
								GraphicsManager::getInstance()->messageSize));

            sf::String checkReturn = sf::String("");
            sf::String space = sf::String(" ");

			for (int i = 0; i < message.getString().getSize(); i++)
			{
				// Add in each letter of the message
				checkBound.setString(checkBound.getString() + message.getString()[i]);
				
				// If the check goes over the width
				if (checkBound.getLocalBounds().width > boundingWidth)
				{
					// Loop backwards from the end of the check
                    // We do j = i - 1 to avoid the case where a space is at the last letter
					for (int j = i - 1; j >= 0; j--)
					{
						// Until you find a space
						if (message.getString()[j] == space)
						{
							// Then reset the normal message with a return character at that point
							checkReturn = message.getString();
							checkReturn.insert(j + 1, "\n"); // +1 to go after the designated point
							message.setString(checkReturn);
							checkBound.setString("");
							i = j + 1;
							break;
						}
					}
				}
			}
		}

		// Because this check indicates that the TextBox has been altered,
		// we also reset its position accordingly
		if (textAlignment == Alignments::LEFT)
			message.setOrigin(GraphicsManager::getInstance()->getLeftCenter(message));
		else if (textAlignment == Alignments::CENTER)
			message.setOrigin(GraphicsManager::getInstance()->getCenter(message));
		message.setPosition(sf::Vector2f(GraphicsManager::getInstance()->window.getSize()) / 2.0f);
		message.move(targetPosition.x * GraphicsManager::getInstance()->scale, 
			         targetPosition.y * GraphicsManager::getInstance()->scale);

        if (borderOutline)
        {
            sf::FloatRect messageRect = message.getGlobalBounds();
            border = sf::RectangleShape(sf::Vector2f(boundingWidth + 20 * GraphicsManager::getInstance()->scale,
                                        messageRect.height + 20 * GraphicsManager::getInstance()->scale));
            border.setOrigin(GraphicsManager::getInstance()->getCenter(border));
            border.setPosition(message.getPosition());
            border.setOutlineColor(GraphicsManager::getInstance()->buttonColor);
            border.setOutlineThickness(2);
            border.setFillColor(sf::Color::Transparent);
        }

        prevMessage = message;
	}
}

void TextBox::update()
{
	// Check each loop if the message needs to be wrapped
	// This is needed in cases where, for example, the status is updated
	// and needs to be rewrapped again
	textWrap();
}

void TextBox::draw()
{
    sf::Color adjustColor = message.getColor();
    adjustColor.a = fade.value;
    message.setColor(adjustColor);
    float scaleFade = fade.value/255.0f / 4.0f + 0.75f;
    
    if(borderOutline)
    {
        adjustColor = GraphicsManager::getInstance()->buttonColor;
        adjustColor.a = fade.value;
        border.setOutlineColor(adjustColor);

        border.setScale(sf::Vector2f(scaleFade, scaleFade));

        sf::Vector2f borderPosition = border.getPosition();

        if (fade.state == FadeStates::FADING_IN)
            border.move(sf::Vector2f(0, (1 - fade.value / 255.0f) * -128));
        else if (fade.state == FadeStates::FADING_OUT)
            border.move(sf::Vector2f(0, (1 - fade.value / 255.0f) * 128));

        GraphicsManager::getInstance()->window.draw(border);
        
        border.setPosition(borderPosition);
    }

    message.setScale(sf::Vector2f(scaleFade, scaleFade));

    sf::Vector2f prevPosition = message.getPosition();

    if(fade.state == FadeStates::FADING_IN)
        message.move(sf::Vector2f(0, (1 - fade.value/255.0f) * -128));
    else if(fade.state == FadeStates::FADING_OUT)
        message.move(sf::Vector2f(0, (1 - fade.value/255.0f) * 128));

	GraphicsManager::getInstance()->window.draw(message);

    message.setPosition(prevPosition);
}

void TextBox::reload()
{
    if (borderOutline)
    {
        textWrap();

        sf::FloatRect messageRect = this->message.getLocalBounds();
        border = sf::RectangleShape(sf::Vector2f(boundingWidth + 20 * GraphicsManager::getInstance()->scale, 
                                    messageRect.height + 20 * GraphicsManager::getInstance()->scale));
        border.setOrigin(GraphicsManager::getInstance()->getCenter(border));
        border.setPosition(this->message.getPosition());
        border.setOutlineColor(GraphicsManager::getInstance()->buttonColor);
        border.setOutlineThickness(2);
        border.setFillColor(sf::Color::Transparent);
    }
}