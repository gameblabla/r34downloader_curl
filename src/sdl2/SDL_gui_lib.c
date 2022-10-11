#include "SDL_gui_lib.h"


#define FPS_VIDEO 60
#define MAX_IMAGE 512
const float real_FPS = 1000/FPS_VIDEO;

SDL_Window* game_screen;
SDL_Renderer* renderer;

SDL_GameController *pad;
SDL_Joystick *joy;
short instanceID;

SDL_Texture* sprites_img[MAX_IMAGE];


unsigned short sprites_w_[MAX_IMAGE];
unsigned short sprites_h_[MAX_IMAGE];

#include "INPUT.h"
struct input BUTTON;

int done = 0;

void Init_Video()
{
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER );
	game_screen = SDL_CreateWindow("R34GUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);  
	
	/* Create renderer, its resolution, set to HARDWARE aceleration and Vsync turned on */
	renderer = SDL_CreateRenderer(game_screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, 640, 480);
	
	/* Clear everything on screen */
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

void Load_Image(unsigned short a, const char* directory)
{
	SDL_Surface* tmp;
	
	if (sprites_img[a] != NULL)
	{
		SDL_DestroyTexture(sprites_img[a]);
	}
	
	tmp = IMG_Load(directory);
	
	sprites_w_[a] = tmp->w;
	sprites_h_[a] = tmp->h;
	
	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	SDL_SetSurfaceRLE(tmp, 1);
	
	sprites_img[a] = SDL_CreateTextureFromSurface(renderer, tmp);

	SDL_FreeSurface(tmp);
}

void Load_Image_Stretch(unsigned short a, const char* directory, short w, short h)
{
	SDL_Surface* tmp;
	
	if (sprites_img[a] != NULL)
	{
		SDL_DestroyTexture(sprites_img[a]);
	}
	
	tmp = IMG_Load(directory);
	
	sprites_w_[a] = tmp->w;
	sprites_h_[a] = tmp->h;
	
	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	SDL_SetSurfaceRLE(tmp, 1);
	
	sprites_img[a] = SDL_CreateTextureFromSurface(renderer, tmp);

	SDL_FreeSurface(tmp);
}


void Put_image(unsigned short a, short x, short y)
{
	if (!sprites_img[a]) return;
	
	SDL_Rect position;
	position.x = x;
	position.y = y;
	
	position.w = sprites_w_[a];
	position.h = sprites_h_[a];
	SDL_RenderCopy(renderer, sprites_img[a], NULL, &position);
}

void Put_image_scaled(unsigned short a, short x, short y, short w, short h)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	
	position.w = w;
	position.h = h;
	SDL_RenderCopy(renderer, sprites_img[a], NULL, &position);
}

void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned short f)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;
	
	SDL_Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;
	
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put sprite %d on screen and update its position\n X: %d \n Y: %d\n Frame: %d\n", a, x ,y, f);
	#endif
	
	SDL_RenderCopy(renderer, sprites_img[a], &frame, &position);
}

void Update_video()
{
	Uint32 start;
	start = SDL_GetTicks();
	SDL_RenderPresent(renderer);
	if(real_FPS > SDL_GetTicks()-start) SDL_Delay(real_FPS-(SDL_GetTicks()-start));
}

void Controls()
{
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		short x_joy, y_joy;

		x_joy = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
		y_joy = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);
		
		BUTTON.UP = state[Buttons_UP] ? 1 : 0;
		BUTTON.DOWN = state[Buttons_DOWN] ? 1 : 0;
		BUTTON.LEFT = state[Buttons_LEFT] ? 1 : 0;
		BUTTON.RIGHT = state[Buttons_RIGHT] ? 1 : 0;
		
		BUTTON.A = (state[Buttons_A] || SDL_GameControllerGetButton(pad, Joypad_A)) ? 1 : 0;
		BUTTON.B = (state[Buttons_B] || SDL_GameControllerGetButton(pad, Joypad_B)) ? 1 : 0;
		BUTTON.X = (state[Buttons_X] || SDL_GameControllerGetButton(pad, Joypad_X)) ? 1 : 0;
		BUTTON.Y = (state[Buttons_Y] || SDL_GameControllerGetButton(pad, Joypad_Y)) ? 1 : 0;
		BUTTON.L = (state[Buttons_L] || SDL_GameControllerGetButton(pad, Joypad_L)) ? 1 : 0;
		BUTTON.R = (state[Buttons_R] || SDL_GameControllerGetButton(pad, Joypad_R)) ? 1 : 0;
		BUTTON.START = (state[Buttons_START] || SDL_GameControllerGetButton(pad, Joypad_START)) ? 1 : 0;
		BUTTON.SELECT = (state[Buttons_SELECT] || SDL_GameControllerGetButton(pad, Joypad_SELECT)) ? 1 : 0;
		BUTTON.QUIT = state[Buttons_QUIT] ? 1 : 0;

        if (y_joy < -6000)
        {
			BUTTON.UP = 1;
        }
		else if (y_joy > 6000)
        {
			BUTTON.DOWN = 1;
        }

        if (x_joy < -6000)
        {
			BUTTON.LEFT = 1;
        }
        else if (x_joy > 6000)
        {
			BUTTON.RIGHT = 1;
        }
		
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
                
				case SDL_CONTROLLERDEVICEADDED:
					if( SDL_IsGameController( 0 ) ) 
					{
						pad = SDL_GameControllerOpen( 0 );
						if (pad) 
						{
							joy = SDL_GameControllerGetJoystick( pad );
							instanceID = SDL_JoystickInstanceID( joy );
						}
					}
				break;

            }
        }
}

void Clear_Image(unsigned short a)
{
	if (sprites_img[a] != NULL)
	{
		SDL_DestroyTexture(sprites_img[a]);
		sprites_img[a] = NULL;
	}
}

void Clear_Images()
{
	short i;
	
	for (i=0;i<MAX_IMAGE;i++) 
	{
		if (sprites_img[i] != NULL)
		{
			SDL_DestroyTexture(sprites_img[i]);
			sprites_img[i] = NULL;
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
	
	switch(color)
	{
		default:
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			//SDL_RenderClear(renderer);
		break;
		case 1:
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

		break;
	}
	SDL_RenderDrawRect(renderer, &position);
	SDL_RenderFillRect( renderer, &position );
			
	//SDL_RenderClear(renderer);
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
