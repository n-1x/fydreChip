fydrechip.o: main.cpp chip8.h
	g++ main.cpp chip8.cpp -lsfml-window -lsfml-system -lsfml-graphics -o fydreChip
