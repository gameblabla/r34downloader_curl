#ifndef SDL_GUI_LIB_H
#define SDL_GUI_LIB_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "core.h"

#define FPS_VIDEO 60
#define MAX_IMAGE 512
extern const float real_FPS;

#define WHITE_COLOR_BG SDL_MapRGB(screen->format, 255, 255, 255)
#define BLUE_COLOR_BG SDL_MapRGB(screen->format, 0, 0, 255)

extern SDL_Surface *sprites_img[MAX_IMAGE];
extern SDL_Surface *screen;

#define Buttons_UP SDLK_UP
#define Buttons_LEFT SDLK_LEFT
#define Buttons_RIGHT SDLK_RIGHT
#define Buttons_DOWN SDLK_DOWN
#define Buttons_A SDLK_x
#define Buttons_B SDLK_c
#define Buttons_C SDLK_v
#define Buttons_D SDLK_b
#define Buttons_START SDLK_SPACE
#define Buttons_SELECT SDLK_BACKSPACE
#define Buttons_QUIT SDLK_ESCAPE

#define Joypad_A 0 
#define Joypad_B 1
#define Joypad_C 2
#define Joypad_D 3
#define Joypad_START 7
#define Joypad_SELECT 5 

extern struct input BUTTON;

extern int done;

extern void Init_Video();

extern void Load_Image(unsigned short a, const char* directory);

extern void Load_Image_Stretch(unsigned short a, const char* directory, short w, short h);

extern void Put_image(unsigned short a, short x, short y);

extern void Put_image_scaled(unsigned short a, short x, short y, short w, short h);

extern void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned short f);

extern void Update_video();

extern void Controls();

extern void Clear_Image(unsigned short a);

extern void Clear_Images();

extern void Faster_clearing(short x, short y, unsigned short w, unsigned short h, unsigned int color);

extern void Print_text(short x, short y, short distance, char *text_ex);

extern unsigned char Collision(short x, short y, short w, short h, short x2, short y2, short w2, short h2);

#endif
