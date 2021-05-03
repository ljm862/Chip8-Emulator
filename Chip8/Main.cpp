#include <iostream>
#include "SDL.h"
#include "Window.h"
#include "Chip8.h"
#include <chrono>
#include <thread>

#define SCREENWIDTH 1024
#define SCREENHEIGHT 512

int main(int argc, char* argv[])
{
	if (argc != 2 || argv[1] == NULL) {
		std::cout << "Game path should be given as arugment: Usage: Chip8.exe <ROM>" << std::endl;
		return -1;
	}

	Chip8* chip = new Chip8();
	Window* window = new Window();

	chip->Initialise();
	if (!chip->LoadGame(argv[1])) {
		return -2;
	}
	window->Setup(SCREENWIDTH, SCREENHEIGHT);

	auto currentTime = std::chrono::system_clock::now();
	int executions = 0;
	uint32_t ticks = SDL_GetTicks();

	bool isRunning = true;
	SDL_Event ev;
	while (isRunning) {

		if (executions > 700) {
			if (std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now() - currentTime).count() < 1) {
				std::cout << "We were going too fast, let's sleep for a bit." << std::endl;
				std::this_thread::sleep_for(std::chrono::microseconds(50));
			}
			else {
				currentTime = std::chrono::system_clock::now();
				executions = 0;
			}
		}
		else {

			chip->ExecuteCycle();

			while (SDL_PollEvent(&ev) != 0)
			{
				if (ev.type == SDL_QUIT) {
					isRunning = false;
				}
				else if (ev.type == SDL_KEYDOWN)
				{
					window->HandleKeyInputs(ev.key.keysym.sym, chip->keypad, 1);
				}
				else if (ev.type == SDL_KEYUP)
				{
					window->HandleKeyInputs(ev.key.keysym.sym, chip->keypad, 0);
				}
			}

			if (chip->drawFlag) {
				for (int i = 0; i < 2048; i++)
				{
					uint8_t pixel = chip->gpu[i];
					window->pixelBuffer[i] = (0x00FFFFFF * pixel) | 0xFF000000;
				}
				window->Draw();
				chip->drawFlag = false;
			}

			if (SDL_GetTicks() - ticks > 16) {
				if (chip->GetSoundTimer() > 0) {
					window->PlayAudio();
				}
				chip->ReduceTimers();
				ticks = SDL_GetTicks();
			}

			std::this_thread::sleep_for(std::chrono::microseconds(1350));
			executions++;
		}
	}

	window->Close();
	return 0;
}



