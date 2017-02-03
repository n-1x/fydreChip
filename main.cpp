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
	sf::Clock clock, cycleClock;

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

		if(true)//cycleClock.getElapsedTime() >= sf::milliseconds(150))
		{
			chip.runSingleCycle();
			cycleClock.restart();
		}

		//decrement chip8 timers at correct frequency
		if(clock.getElapsedTime() >= sf::milliseconds(1000/60)) 
		{
				chip.decTimers();
				clock.restart();
		}

		window.display();
	}
	
	return 0;
}
