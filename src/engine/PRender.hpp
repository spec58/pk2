//#########################
//Pekka Kana 2
//Copyright (c) 2003 Janne Kivilahti
//#########################
#pragma once

#include "engine/PFile.hpp"

namespace PRender {

struct FRECT {
	
	float x, y, w, h;

};

class Renderer {

	public:

	virtual void* create_texture(void* surface) = 0;
	virtual void remove_texture(void* texture) = 0;
	virtual void render_texture(void* texture, float x, float y, float w, float h, float alpha) = 0;

	virtual void clear_screen() = 0;
	virtual void set_screen(FRECT screen_dst) = 0;
	virtual int  set_shader(int mode) = 0;
	virtual int  set_vsync(bool set) = 0;
	virtual void update(void* _buffer8) = 0;
	virtual ~Renderer() {};

};

enum {

	SHADER_NEAREST,
	SHADER_LINEAR,
	SHADER_CRT,
	SHADER_HQX

};

enum {

	RENDERER_SDL,
	RENDERER_OPENGL,
	RENDERER_OPENGLES,
	RENDERER_SDL_SOFTWARE,

};

void* load_texture(PFile::Path file);
void render_texture(void* texture, float x, float y, float w, float h, float alpha);
void remove_texture(void* texture);

int  set_filter(const char* filter);
int  set_shader(int mode);

void set_screen_fill(bool set);
void adjust_screen();
void set_fullscreen(bool set);
void set_window_size(int w, int h);
void get_window_size(int* w, int* h);
void get_cover_size(int* w, int* h);
void get_window_position(int* x, int* y);

int  set_vsync(bool set);
bool is_vsync();

int  init(int width, int height, const char* name, const char* icon);
void terminate();
void update(void* _buffer8);

}