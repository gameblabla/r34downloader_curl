#include "SDL_gui_lib.h"


#define FPS_VIDEO 60
#define MAX_IMAGE 512
const float real_FPS = 1000/FPS_VIDEO;

SDL_Surface *sprites_img[MAX_IMAGE];
SDL_Surface *screen;

#include "INPUT.h"
struct input BUTTON;

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

int done = 0;

void Load_Image(unsigned short a, const char* directory)
{
	SDL_Surface *tmp;
	if (sprites_img[a] != NULL)
	{
		SDL_FreeSurface(sprites_img[a]);
	}

	tmp = IMG_Load(directory);
	SDL_SetColorKey(tmp, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(tmp->format, 255, 0, 255));

	sprites_img[a] = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);
}


void Put_image(unsigned short a, short x, short y)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	SDL_BlitSurface(sprites_img[a], NULL, screen, &position);
}


void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned short f)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;

	SDL_Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;

	SDL_BlitSurface(sprites_img[a], &frame, screen, &position);
}

void Update_video()
{
	Uint32 start;
	start = SDL_GetTicks();
	SDL_Flip(screen);
	if(real_FPS > SDL_GetTicks()-start) SDL_Delay(real_FPS-(SDL_GetTicks()-start));
}

void Controls()
{
    Uint8 *keystate = SDL_GetKeyState(NULL);
    unsigned char joy_b[8] = {0};
    
		#ifdef JOYSTICK
		    short x_joy = 0, y_joy = 0;
			unsigned char i;
			
			if (SDL_NumJoysticks() > 0)
			{
				joy = SDL_JoystickOpen(0);
			}
        
			for(i=0;i<8;i++)
			{
				joy_b[i] = SDL_JoystickGetButton(joy, i) ? 1 : 0;
			}
			
			x_joy = SDL_JoystickGetAxis(joy, 0);
			y_joy = SDL_JoystickGetAxis(joy, 1);
			
			SDL_JoystickUpdate();
		#endif
		
		BUTTON.UP = keystate[Buttons_UP] ? 1 : 0;
		BUTTON.DOWN = keystate[Buttons_DOWN] ? 1 : 0;
		BUTTON.LEFT = keystate[Buttons_LEFT] ? 1 : 0;
		BUTTON.RIGHT = keystate[Buttons_RIGHT] ? 1 : 0;
		
		BUTTON.A = (keystate[Buttons_A] || joy_b[Joypad_A]) ? 1 : 0;
		BUTTON.B = (keystate[Buttons_B] || joy_b[Joypad_B]) ? 1 : 0;
		BUTTON.C = (keystate[Buttons_C] || joy_b[Joypad_C]) ? 1 : 0;
		BUTTON.D = (keystate[Buttons_D] || joy_b[Joypad_D]) ? 1 : 0;
		BUTTON.START = (keystate[Buttons_START] || joy_b[Joypad_START]) ? 1 : 0;
		BUTTON.SELECT = (keystate[Buttons_SELECT] || joy_b[Joypad_SELECT]) ? 1 : 0;
		BUTTON.QUIT = keystate[Buttons_QUIT] ? 1 : 0;
			
		#ifdef JOYSTICK		
			if (y_joy < -5000)
			{
				BUTTON.UP = 1;
			}
			else if (y_joy > 5000)
			{
				BUTTON.DOWN = 1;
			}

			if (x_joy < -5000)
			{
				BUTTON.LEFT = 1;
			}
			else if (x_joy > 5000)
			{
				BUTTON.RIGHT = 1;
			}
        #endif

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    done = 1;
                    break;
                }
            }
        }
}

void Clear_Image(unsigned short a)
{
	if (sprites_img[a])
	{
		SDL_FreeSurface(sprites_img[a]);
	}
}

void Clear_Images()
{
	short i;

	for (i=0;i<MAX_IMAGE;i++)
	{
		if (sprites_img[i])
		{
			SDL_FreeSurface(sprites_img[i]);
		}
	}
}

void Faster_clearing(short x, short y, unsigned short w, unsigned short h, unsigned int color)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;

    SDL_FillRect(screen, &position, color);
}

void Print_text(short x, short y, short distance, char *text_ex)
{
	unsigned char i = 0;
	for (i=0;text_ex[i]!='\0';i++)
	{
		if (text_ex[i] > 32) Put_sprite(511, x + (distance * i), y, 20, 20, text_ex[i]-32);
	}
}

unsigned char Collision(short x, short y, short w, short h, short x2, short y2, short w2, short h2)
{
	if ((x + w > x2) && (x < x2 + w2) && (y + h > y2) && (y < y2 + h2))
	{
		return 1;
	}	
	return 0;
}
