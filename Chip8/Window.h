#pragma once
#include "SDL.h"
#include "SDL_mixer.h"

class Window {
public:
	uint32_t pixelBuffer[2048];

	void HandleKeyInputs(SDL_Keycode key, uint8_t* keypad, int isDown);
	int Setup(int w, int h);
	int Close();
	void Draw();
	void PlayAudio();

private:
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	Mix_Chunk* beep;
};
