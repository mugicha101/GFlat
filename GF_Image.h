#pragma once

#include "SDL.h"
#include "SDL_Image.h"
#include <string>

class GF_Image {
private:
	SDL_Renderer** renderer = nullptr;
	SDL_Texture* texture = nullptr;
	std::string path;

	int width = 0;
	int height = 0;

	void free() {
		if (texture != nullptr) SDL_DestroyTexture(texture);
		texture = nullptr;
		renderer = nullptr;
		path = "";
		width = 0;
		height = 0;
	}

	static friend void copy(GF_Image& src, GF_Image& dst) {
		dst.setSource(src.renderer, src.path);
	}
public:
	bool setSource(SDL_Renderer** renderer, std::string sourcePath) {
		if (sourcePath == path && this->renderer == renderer) return true;
		free();
		SDL_Surface* loadedSurface = IMG_Load(sourcePath.c_str());
		if (loadedSurface == NULL) {
			printf("Sprite::setSource failed: unable to load image at path %s, SDL_image Error: %s\n", sourcePath.c_str(), IMG_GetError());
			return false;
		}
		width = loadedSurface->w;
		height = loadedSurface->h;
		texture = SDL_CreateTextureFromSurface(*renderer, loadedSurface);
		SDL_FreeSurface(loadedSurface);
		if (texture == NULL) {
			printf("Sprite::setSource failed: unable to create texture from %s, SDL Error: %s\n", sourcePath.c_str(), SDL_GetError());
			return false;
		}
		this->renderer = renderer;
		this->path = sourcePath;
		return true;
	}

	GF_Image(SDL_Renderer** renderer, std::string sourcePath) {
		setSource(renderer, sourcePath);
	}

	GF_Image() {}

	~GF_Image() {
		free();
	}

	GF_Image& operator=(GF_Image other) {
		copy(other, *this);
		return *this;
	}

	void draw(int x, int y) {
		SDL_Rect bounds = { x, y, width, height };
		SDL_Rect viewportBounds;
		SDL_RenderGetViewport(*renderer, &viewportBounds);
		if (SDL_RenderCopy(*renderer, texture, &bounds, &viewportBounds)) {
			printf("draw error: %s\n", IMG_GetError());
		}
	}
};