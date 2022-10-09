#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "core.h"
#include "SDL_gui_lib.h"
#include "INPUT.h"

char text_buffer[64];

int player_x  = 0;
int player_y = 0;
int delay_input = 0;

char rows_of_text[3][27] = 
{
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
	"abcdefghijklmnopqrstuvwxyz",
	"_-+0123456789  <=",
};

void Add_Character_Keyboard(int i_2, int i_1)
{
	int size;
	size = strlen(text_buffer);
	// Make sure to limit the size of the string
	if (size < 25)
	{
		// Append selected character to string
		strncat(text_buffer, &rows_of_text[i_2][i_1], 1);
	}
	delay_input = 2;
}

void Delete_Character_Keyboard()
{
	int size;
	size = strlen(text_buffer); //Total size of string
	if (size > 0)
	{
		printf("Removed !\n");
		text_buffer[size-1] = '\0';
	}
	delay_input = 2;
}

void Virtual_Keyboard()
{
	int i, i2;
	if (BUTTON.A)
	{
		// Do the delete button separately
		// Delete last character from String
		if (Collision(player_x, player_y, 16, 16, 360, 430, 40, 40))
		{
			Delete_Character_Keyboard();
		}
		else if (player_y > 360)
		{
			/* For virtual keyboard
			// Quite simple really, the virtual keyboard was designed
			// as to directly input the character from the array into the string
			// Clever, right ? This avoids us the use of very CPU intensive collision checks and table lookups.
			// Maybe this could be better optimized though ?
			// However it doesn't matter as much because it only is trigerred once
			// when the user presses the A button.
			*/
			for(i=0;i<26;i++)
			{
				for(i2=0;i2<3;i2++)
				{
					// Don't go beyond the size of the array
					// This is a hack that will work for our purpose
					if (!(i2 == 2 && i > 12))
					{
						if (Collision(player_x,  player_y, 8, 14, 16 + ((i * 23)),       360+(i2*30), 23, 20))
						{
							Add_Character_Keyboard(i2, i);
						}
					}
				}
			}
		}
	}
			
	if (BUTTON.B)
	{
		Delete_Character_Keyboard();
	}
}

void Draw_Virtual_Keyboard()
{
	int i;
	// Draw Virtual keyboard
	Faster_clearing(12, 350, 640-24, 120, 0x0000FF);
	for(i=0;i<3;i++)
	{
		Print_text(16, 360+(i*30), 23, rows_of_text[i]);
	}
	Print_text(280, 450, 23, "[OK]");
}
		
int main(int argc, char** argv)
{
	SDL_Init( SDL_INIT_VIDEO );
	screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);
	
	snprintf(text_buffer, sizeof(text_buffer), "Sonic_the_Hedgehog");
	
	Load_Image(510, "internal_img/cursor.bmp");
	Load_Image(511, "internal_img/font.png");

	while(!BUTTON.QUIT)
	{
		Controls();
		
		Faster_clearing(0, 0, 640, 480, 0xFFFFFF);
		
		if (BUTTON.UP) player_y -= 3;
		else if (BUTTON.DOWN) player_y += 3;
		
		if (BUTTON.LEFT) player_x -= 3;
		else if (BUTTON.RIGHT) player_x += 3;
		
		if (delay_input > 10)
		{
			Virtual_Keyboard();
		}
		
		delay_input++;
		
		// Print text that's in buffer
		Print_text(32, 20, 20, text_buffer);
		
		Draw_Virtual_Keyboard();
		
		// Draw cursor
		Put_image(510, player_x, player_y);
		
		Update_video();
	}
	
	return 0;
}
