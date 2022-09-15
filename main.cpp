#include <SDL.h>
#include <SDL_Image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include "GF_Image.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int CALC_FPS = 60;
const int DRAW_FPS = 60;

SDL_Window* window = NULL; // screen window
SDL_Surface* screenSurface = NULL; // screen surface
SDL_Event e;

SDL_Renderer* renderer;

GF_Image img;

void screenUpdate() {
	SDL_UpdateWindowSurface(window);
}

bool init() {
	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// create window
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	screenSurface = SDL_GetWindowSurface(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// default bg
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
	screenUpdate();

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	return true;
}

void close() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool loadMedia() {
	img = GF_Image(&renderer, "media/texture.png");
	return true;
}

bool quit = false;
int frame = 0;

void eventLoop() {
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				break;
			}
			break;
		}
	}
}

void calc(int dt) {

}

void draw() {
	// clear screen
	SDL_RenderSetViewport(renderer, nullptr);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_NONE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	// draws a box
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect fillRect = { SCREEN_WIDTH >> 2, SCREEN_HEIGHT >> 2, SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1 };
	SDL_RenderFillRect(renderer, &fillRect);

	// draws a polygon
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_Point pts[7];
	float dir = 0;
	float step = (float)(M_PI / 3);
	float rad = SCREEN_WIDTH * 0.25f;
	SDL_Point center = { SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1 };
	for (int i = 0; i < 6; ++i) {
		pts[i] = { center.x + lroundf(rad * cosf(dir)), center.y + lroundf(rad * sinf(dir)) };
		dir += step;
	}
	pts[6] = pts[0];
	SDL_RenderDrawLines(renderer, pts, 7);

	SDL_Rect miniMapViewport = { 0, 0, SCREEN_WIDTH >> 2, SCREEN_HEIGHT >> 2 };
	SDL_RenderSetViewport(renderer, &miniMapViewport);

	// render image
	img.draw(0, 0);

	// update screen
	SDL_RenderPresent(renderer);
}

int mainLoop() {
	int cdt = (int)(1000.0f / CALC_FPS + 0.5f); // ms between calc frames
	int ddt = (int)(1000.0f / DRAW_FPS + 0.5f); // ms between draw frames
	long time = SDL_GetTicks64();
	printf("Main Loop Entered\n");

	while (!quit) {
		eventLoop();
		long nextFrame = time + (ddt << 1);
		while (time < nextFrame) {
			calc(cdt);
			time += cdt;
			frame++;
		}
		draw();
		long delay = nextFrame - SDL_GetTicks64();
		std::cout << delay << std::endl;
		if (delay > 0)
			SDL_Delay(delay);
	}
	printf("Main Loop Exited");
	return 0;
}

int main( int argc, char* args[] ) {
	// init
	if (!init()) {
		printf("Failed to initialize!\n");
		return 1;
	}

	// load media
	if (!loadMedia()) {
		printf("Failed to load media!\n");
		return 1;
	}

	// main loop
	int ret_val = mainLoop();

	// close
	close();
	return ret_val;
}