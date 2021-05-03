#pragma once
#include <iostream>

class Chip8
{
private:
	uint8_t memory[4096]; // RAM

	uint8_t V[16]; // 16th byte is used as a flag register VARIABLE_REGISTERS

	uint16_t pc; // PROGRAM COUNTER
	uint16_t I; // INDEX REGISTER
	uint16_t stack[16]; // STACK
	uint16_t sp; // STACK POINTER
	uint16_t opcode; // CURRENT OPCODE


	uint8_t delayTimer;
	uint8_t soundtimer;

public:
	bool gpu[64 * 32]; // SCREEN DISPLAY
	bool drawFlag;
	uint8_t keypad[16];

	Chip8();
	~Chip8();

	void Initialise();

	bool LoadGame(const char* gameName);
	void ExecuteCycle();
	void ReduceTimers();
	uint8_t GetSoundTimer();
};

