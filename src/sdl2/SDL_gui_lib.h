#ifndef SDL_GUI_LIB_H
#define SDL_GUI_LIB_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "core.h"

#define FPS_VIDEO 60
#define MAX_IMAGE 512
extern const float real_FPS;

#define WHITE_COLOR_BG 0
#define BLUE_COLOR_BG 1

#define Buttons_UP SDL_SCANCODE_UP
#define Buttons_LEFT SDL_SCANCODE_LEFT
#define Buttons_RIGHT SDL_SCANCODE_RIGHT
#define Buttons_DOWN SDL_SCANCODE_DOWN
#define Buttons_A SDL_SCANCODE_X
#define Buttons_B SDL_SCANCODE_C
#define Buttons_X SDL_SCANCODE_V
#define Buttons_Y SDL_SCANCODE_B
#define Buttons_L SDL_SCANCODE_BACKSPACE
#define Buttons_R SDL_SCANCODE_RETURN
#define Buttons_START SDL_SCANCODE_SPACE
#define Buttons_SELECT SDL_SCANCODE_RETURN
#define Buttons_QUIT SDL_SCANCODE_ESCAPE

#define Joypad_A SDL_CONTROLLER_BUTTON_A 
#define Joypad_B SDL_CONTROLLER_BUTTON_B
#define Joypad_X SDL_CONTROLLER_BUTTON_X
#define Joypad_Y SDL_CONTROLLER_BUTTON_Y
#define Joypad_L SDL_CONTROLLER_BUTTON_LEFTSHOULDER
#define Joypad_R SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
#define Joypad_START SDL_CONTROLLER_BUTTON_START 
#define Joypad_SELECT SDL_CONTROLLER_BUTTON_BACK 

extern struct input BUTTON;

extern SDL_Texture* sprites_img[MAX_IMAGE];

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
