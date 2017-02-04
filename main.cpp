//File: main.cpp
//Author: Nicholas J D Dean
//Date Created: 2017-01-27

#include <iostream>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "chip8.h"

const int PIXEL_SIZE = 13,             //chip8 pixel side length in pixels
          WIDTH = 64,                  //width in chip8 pixels
	      HEIGHT = 32,                 //height in chip8 pixels
	      NUM_PIXELS = WIDTH * HEIGHT; //num of chip8 pixels

const int SAMPLES = 44100, //1s
	  SAMPLE_RATE = 44100; //44.1KHz

typedef sf::RectangleShape display[NUM_PIXELS];

sf::Keyboard::Key keyMap[] = //used to map all of the keys to an index 
	{
		sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3, 
		  sf::Keyboard::Num4,
		sf::Keyboard::Q, sf::Keyboard::W, sf::Keyboard::E, sf::Keyboard::R,
		sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::F,
		sf::Keyboard::Z, sf::Keyboard::X, sf::Keyboard::C, sf::Keyboard::V
	};


bool keyPressed[16] = {false}; //tracks the state of all keys in keyMap

//fills the given array with a full sine wave
void fillWithSamples(sf::Int16 (&raw)[SAMPLES]) //output
{
	float TWO_PI = 3.1415926535f * 2.0f,
		  AMP = 30000.0f,
		  increment = 600.0f / SAMPLE_RATE,
	 	  x = 0.0f;

	for(int i = 0; i < SAMPLES; i++)
	{
		raw[i] = AMP * sin(x * TWO_PI);
		x += increment;
	}
}



//initialise the size, position and colour of all the rectnagles
//representing pixels on the screen
void initScreen(display &d)
{
	for(int i = 0; i < NUM_PIXELS; i++)
	{
	
		d[i].setSize(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));

		d[i].setPosition((i % WIDTH) * PIXEL_SIZE,
				      (i / WIDTH) * PIXEL_SIZE);

		d[i].setFillColor(sf::Color(0, 0, 0));
	}
}



inline void updateScreen(display &d, bool *newDisplay)
{
	//update all pixel colours
	for(int i = 0; i < NUM_PIXELS; i++)
	{
		//CAREFUL: assuming size of newDisplay is
		//NUM_PIXELS

		if(newDisplay[i])
		{
			d[i].setFillColor(sf::Color::White);
		}
		else
		{
			d[i].setFillColor(sf::Color::Black);
		}
	}
}



int main(int argc, char *argv[])
{
	Chip8 chip;                            //the emulator
	int window_width = WIDTH * PIXEL_SIZE,
	    window_height = HEIGHT * PIXEL_SIZE;
	sf::Clock clock, cycleClock;           //for timing decTimers() and clock speed
	sf::SoundBuffer soundBuff;             //to load the samples
	sf::Int16 rawSamples[SAMPLES];         //will hold the sine wave for the sound
	sf::RectangleShape pixels[NUM_PIXELS]; //all the rectangles that represent pixels
	sf::Sound sound;       	               //to play and pause the sound

	fillWithSamples(rawSamples);
	soundBuff.loadFromSamples(rawSamples, SAMPLES, 1, SAMPLE_RATE);

	sound.setBuffer(soundBuff);
	sound.setLoop(true);

	sf::RenderWindow window(sf::VideoMode(window_width, window_height), 
			        "Fydrechip", 
			        sf::Style::Titlebar | sf::Style::Close);

	//use first command line argument as rom path
	chip.loadRom(argv[1]);

	initScreen(pixels);

	std::cout << "\nWelcome to FydreChip!\n";
	std::cout << "Press <F5> to reset.\n";
	std::cout << "---------------------\n";

	//main window loop
	while(window.isOpen())
	{
		sf::Event event;

		//print the cycle time
		std::cout << "\rLast cycle time (ms): "
				  << cycleClock.getElapsedTime().asMilliseconds();
		cycleClock.restart();	

		//poll the window's events
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}	

		//poll keyboard for reset key
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
		{
			chip.hardReset();
			chip.loadRom(argv[1]);
		}

		window.clear(sf::Color::Black);

		//update the screen if there has been a draw
		//by the emulator
		if(chip.drawFlag())
		{
			updateScreen(pixels, chip.getDisplay());
		}

		//start or stop the sound
		chip.soundFlag() ? sound.play() : sound.pause();

		//draw all pixels
		for(int i = 0; i < NUM_PIXELS; i++)
		{
			window.draw(pixels[i]);
		}

		//update key states
		for(int i = 0; i < 0x10; ++i)
		{
			if(sf::Keyboard::isKeyPressed(keyMap[i]))
			{
				keyPressed[i] = true;
			}
			else
			{
				keyPressed[i] = false;
			}
		}

		//send key states to emulator
		chip.setKeys(keyPressed);

		//emulate next cycle
		chip.runSingleCycle();

		//decrement timers at 60Hz
		if(clock.getElapsedTime() >= sf::milliseconds(1000/60)) 
		{
				chip.decTimers();
				clock.restart();
		}

		window.display();
	}
	std::cout << std::endl;
	return 0;
}
