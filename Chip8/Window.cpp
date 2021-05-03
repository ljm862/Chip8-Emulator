#include <iostream>
#include "SDL.h"
#include "Window.h"
#include "SDL_mixer.h"


void Window::HandleKeyInputs(SDL_Keycode key, uint8_t* keypad, int isDown) {
	switch (key)
	{
	case SDLK_1:
		std::cout << "1 Pressed!" << std::endl;
		keypad[0] = isDown;
		break;
	case SDLK_2:
		std::cout << "2 Pressed!" << std::endl;
		keypad[1] = isDown;
		break;
	case SDLK_3:
		std::cout << "3 Pressed!" << std::endl;
		keypad[2] = isDown;
		break;
	case SDLK_4:
		std::cout << "C Pressed!" << std::endl;
		keypad[3] = isDown;
		break;
	case SDLK_q:
		std::cout << "4 Pressed!" << std::endl;
		keypad[4] = isDown;
		break;
	case SDLK_w:
		std::cout << "5 Pressed!" << std::endl;
		keypad[5] = isDown;
		break;
	case SDLK_e:
		std::cout << "6 Pressed!" << std::endl;
		keypad[6] = isDown;
		break;
	case SDLK_r:
		std::cout << "D Pressed!" << std::endl;
		keypad[7] = isDown;
		break;
	case SDLK_a:
		std::cout << "7 Pressed!" << std::endl;
		keypad[8] = isDown;
		break;
	case SDLK_s:
		std::cout << "8 Pressed!" << std::endl;
		keypad[9] = isDown;
		break;
	case SDLK_d:
		std::cout << "9 Pressed!" << std::endl;
		keypad[10] = isDown;
		break;
	case SDLK_f:
		std::cout << "E Pressed!" << std::endl;
		keypad[11] = isDown;
		break;
	case SDLK_z:
		std::cout << "A Pressed!" << std::endl;
		keypad[12] = isDown;
		break;
	case SDLK_x:
		std::cout << "0 Pressed!" << std::endl;
		keypad[13] = isDown;
		break;
	case SDLK_c:
		std::cout << "B Pressed!" << std::endl;
		keypad[14] = isDown;
		break;
	case SDLK_v:
		std::cout << "F Pressed!" << std::endl;
		keypad[15] = isDown;
		break;
	}
}

int Window::Setup(int width, int height) {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "Error Video/Audio Init: " << SDL_GetError() << std::endl;
		exit(1);
	}
	else {
		this->window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	}

	if (this->window == NULL) {
		std::cout << "Error Creating Window: " << SDL_GetError() << std::endl;
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024) < 0) {
		std::cout << "Error with audio: " << Mix_GetError() << std::endl;
	}

	Mix_AllocateChannels(1);
	this->beep = Mix_LoadWAV("beep.wav");

	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
	SDL_RenderSetLogicalSize(this->renderer, width, height);

	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	return 0;
}

void Window::Draw()
{
	SDL_UpdateTexture(this->texture, NULL, pixelBuffer, 64 * sizeof(Uint32));
	SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
	SDL_RenderPresent(this->renderer);
}

void Window::PlayAudio()
{
	Mix_PlayChannel(-1, this->beep, 0);
}

int Window::Close()
{
	SDL_DestroyWindow(this->window);
	Mix_FreeChunk(this->beep);
	Mix_Quit();
	SDL_Quit();
	return 0;
}

