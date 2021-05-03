#include "Chip8.h"
#include <fstream>
#include <vector>
#include <stdlib.h>

uint8_t font[0x50] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

void Chip8::Initialise() {

	srand(time(0));

	this->pc = 0x200;
	this->opcode = 0;
	this->I = 0;
	this->sp = 0;

	this->drawFlag = false;

	this->delayTimer = 0;
	this->soundtimer = 0;

	for (int i = 0; i < 16; i++)
	{
		this->V[i] = 0;
		this->stack[i] = 0;
		this->keypad[i] = 0;
		this->gpu[i] = 0;
		this->memory[i] = 0;
	}

	for (int i = 16; i < 2048; i++)
	{
		this->gpu[i] = 0;
		this->memory[i] = 0;
	}

	for (int i = 2048; i < 4096; i++)
	{
		this->memory[i] = 0;
	}

	for (int i = 0; i < 0x50; i++)
	{
		this->memory[i + 0x50] = font[i];
	}


}

Chip8::Chip8()
{
}

Chip8::~Chip8()
{
}

bool Chip8::LoadGame(const char* gamePath) {
	std::ifstream input(gamePath, std::ios::binary);

	if (input.fail()) {
		std::cout << "Error loading file. Is it a .ch8 file?" << std::endl;
		return false;
	}
	std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(input), {});


	if (buffer.size() > 4096 - 512) {
		std::cerr << "Error, file too large." << std::endl;
		return false;
	}

	for (int i = 0; i < buffer.size(); i++)
	{
		this->memory[0x200 + i] = buffer[i];
	}

	input.close();
	return true;
}

void Chip8::ReduceTimers() {
	if (this->delayTimer > 0) {
		this->delayTimer--;
	}
	if (this->soundtimer > 0) {
		this->soundtimer--;
	}
}

uint8_t Chip8::GetSoundTimer()
{
	return this->soundtimer;
}

void Chip8::ExecuteCycle() {
	//Fetch
	this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc + 1];
	this->pc += 1;
	//printf("Opcode: %.4X\n", this->opcode);
	//Decode
	auto firstNibble = this->opcode & 0xF000;
	auto secondNibble = (this->opcode & 0x0F00) >> 8;
	auto thirdNibble = (this->opcode & 0x00F0) >> 4;
	auto fourthNibble = this->opcode & 0x000F;
	auto secondByte = this->memory[this->pc];
	this->pc += 1;
	auto remainder = this->opcode & 0x0FFF;


	//Execute
	switch (firstNibble) {
	case 0x0000:
		switch (fourthNibble) {
		case 0x0000:
			for (int i = 0; i < 2048; i++)
			{
				this->gpu[i] = 0;
			}
			this->drawFlag = true;
			break;
		case 0x000E:
			--sp;
			this->pc = this->stack[this->sp];
			break;
		default:
			printf("Unknown 0 opcode %.4X", this->opcode);
			break;
		}
		break;

	case 0x1000:
		this->pc = remainder;
		break;

	case 0x2000:
		this->stack[this->sp] = pc;
		this->sp += 1;
		this->pc = remainder;
		break;

	case 0x3000:
		if (this->V[secondNibble] == secondByte) {
			this->pc += 2;
		}
		break;

	case 0x4000:
		if (this->V[secondNibble] != secondByte) {
			this->pc += 2;
		}
		break;

	case 0x5000:
		if (this->V[secondNibble] == this->V[thirdNibble]) {
			this->pc += 2;
		}
		break;

	case 0x6000:
		this->V[secondNibble] = secondByte;
		break;

	case 0x7000:
		this->V[secondNibble] += secondByte;
		break;

	case 0x8000:
		switch (fourthNibble) {
		case 0x0000:
			this->V[secondNibble] = this->V[thirdNibble];
			break;

		case 0x0001:
			this->V[secondNibble] = this->V[secondNibble] | this->V[thirdNibble];
			break;

		case 0x0002:
			this->V[secondNibble] = this->V[secondNibble] & this->V[thirdNibble];
			break;

		case 0x0003:
			this->V[secondNibble] = this->V[secondNibble] ^ this->V[thirdNibble];
			break;

		case 0x0004:
		{
			auto val = this->V[secondNibble] + this->V[thirdNibble];
			this->V[0xF] = val > 255;
			this->V[secondNibble] = val;
			break;
		}
		case 0x0005:
			this->V[0xF] = this->V[secondNibble] > this->V[thirdNibble];
			this->V[secondNibble] = this->V[secondNibble] - this->V[thirdNibble];
			break;

		case 0x0006:
			this->V[0xF] = V[secondNibble] & 0x1;
			this->V[secondNibble] >>= 1;
			break;

		case 0x0007:
			this->V[0xF] = this->V[secondNibble] < this->V[thirdNibble];
			this->V[secondNibble] = this->V[thirdNibble] - this->V[secondNibble];
			break;

		case 0x000E:
			this->V[0xF] = V[secondNibble] & 0b10000000;
			this->V[secondNibble] <<= 1;
			break;

		default:
			printf("Unknown 8 opcode %.4X", this->opcode);
			break;
		}
		break;

	case 0x9000:
		if (this->V[secondNibble] != this->V[thirdNibble]) {
			this->pc += 2;
		}
		break;

	case 0xA000:
		this->I = remainder;
		break;

	case 0xB000:
		this->pc = remainder + this->V[0];
		break;

	case 0xC000:
		this->V[secondNibble] = (rand() % (0xFF + 1)) & secondByte;
		break;

	case 0xD000:
	{
		auto x = this->V[secondNibble];
		auto y = this->V[thirdNibble];
		uint16_t pixel;

		this->V[0xF] = 0;
		for (int row = 0; row < fourthNibble; row++)
		{
			pixel = this->memory[this->I + row];
			for (int col = 0; col < 8; col++) {
				if ((pixel & (0x80 >> col)) != 0) {
					auto val = x + col + ((y + row) * 64);
					if (this->gpu[val] == 1) {
						this->V[0xF] = 1;
					}
					this->gpu[val] ^= 1;
				}
			}
		}

		this->drawFlag = true;
		break;
	}

	case 0xE000:
		switch (fourthNibble) {
		case 0x0001:
			if (this->keypad[this->V[secondNibble]] == 0) {
				this->pc += 2;
			}
			break;

		case 0x000E:
			if (this->keypad[this->V[secondNibble]] == 1) {
				this->pc += 2;
			}
			break;

		default:
			printf("Unknown E opcode %.4X", this->opcode);
			break;
		}
		break;

	case 0xF000:
		switch (secondByte) {
		case 0x0007:
			this->V[secondNibble] = this->delayTimer;
			break;

		case 0x000A:
		{
			for (int i = 0; i < 16; i++)
			{
				if (this->keypad[i] == 1) {
					this->V[secondNibble] = i;
					return;
				}
			}
			this->pc -= 2;
			break;
		}
		case 0x0015:
			this->delayTimer = this->V[secondNibble];
			break;

		case 0x0018:
			this->soundtimer = this->V[secondNibble];
			break;

		case 0x001E:
			this->I += this->V[secondNibble];
			this->V[0xF] = this->I & 0x1000;
			break;

		case 0x0029:
			this->I = this->V[secondNibble] * 5;
			break;

		case 0x0033:
		{
			auto val = this->V[secondNibble];
			for (int i = 2; i >= 0; i--) {
				this->memory[this->I + i] = val % 10;
				val /= 10;
			}
			break;
		}
		case 0x0055:
			for (int i = 0; i <= secondNibble; i++)
			{
				this->memory[this->I + i] = this->V[i];
			}
			break;

		case 0x0065:
			for (int i = 0; i <= secondNibble; i++)
			{
				this->V[i] = this->memory[this->I + i];
			}
			break;

		default:
			printf("Unknown F opcode %.4X", this->opcode);
			break;
		}
		break;
	default:
		printf("Unknown opcode %.4X\n", this->opcode);
	}

}