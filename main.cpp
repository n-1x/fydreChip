//File: main.cpp
//Author: Nicholas J D Dean
//Date Created: 2017-01-27

#include <iostream>
#include "chip8.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const int PIXEL_SIZE = 10,
          WIDTH = 64,
	       HEIGHT = 32,
	       NUM_PIXELS = WIDTH * HEIGHT;

const float SCALE = 1.0;

int main(int argc, char *argv[])
{
	Chip8 chip;
	int window_width = WIDTH * PIXEL_SIZE * SCALE,
	    window_height = HEIGHT * PIXEL_SIZE * SCALE;
	sf::Clock clock;

	sf::Keyboard::Key keys[] = 
	{
		sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3, sf::Keyboard::Num4,
		sf::Keyboard::Q, sf::Keyboard::W, sf::Keyboard::E, sf::Keyboard::R,
		sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::F,
		sf::Keyboard::Z, sf::Keyboard::X, sf::Keyboard::C, sf::Keyboard::V
	};
	bool keyPressed[16] = {false};

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), 
			        "Fydrechip", 
			        sf::Style::Titlebar | sf::Style::Close);

	chip.loadRom(argv[1]);

	//generate the pixels
	sf::RectangleShape pixels[NUM_PIXELS];
	for(int i = 0; i < NUM_PIXELS; i++)
	{
	
		pixels[i].setSize(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));

		pixels[i].setPosition((i % WIDTH) * PIXEL_SIZE,
				      (i / WIDTH) * PIXEL_SIZE);

		pixels[i].setFillColor(sf::Color(0, 0, 0));
	}

	//main window loop
	while(window.isOpen())
	{
		sf::Event event;

		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}	

		window.clear(sf::Color::Black);

		//update the screen if there has been a draw
		//by the emulator
		if(chip.drawFlag())
		{
			//update the screen
			for(int i = 0; i < NUM_PIXELS; i++)
			{
				bool *pixList = chip.getDisplay();

				if(pixList[i])
				{
					pixels[i].setFillColor(sf::Color::White);
				}
				else
				{
					pixels[i].setFillColor(sf::Color::Black);
				}
			}
		}

		//draw all pixels
		for(int i = 0; i < NUM_PIXELS; i++)
		{
			window.draw(pixels[i]);
		}

		//update keys
		for(int i = 0; i < 0x10; ++i)
		{
			if(sf::Keyboard::isKeyPressed(keys[i]))
			{
				keyPressed[i] = true;
			}
			else
			{
				keyPressed[i] = false;
			}
		}

		chip.setKeys(keyPressed);
		chip.runSingleCycle();

		//decrement chip8 timers at correct frequency (60Hz)
		if(clock.getElapsedTime() >= sf::milliseconds(1000/60)) 
		{
				chip.decTimers();
				clock.restart();
		}

		window.display();
	}
	return 0;
}
