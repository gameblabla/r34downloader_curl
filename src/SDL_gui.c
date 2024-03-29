#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"
#include "SDL_gui_lib.h"
#include "INPUT.h"

#include "common.h"

char text_buffer[64];
char tag_str[256];
char page_str[128];
char tmp_str[512];

char img_tmp_str[71][512];

int player_x  = 0;
int player_y = 0;
int delay_input = 0;
int Game_State = 0;

int current_thumbnail_page = 0;
int current_overall_page = 0;

/* Used for virtual keyboard */
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
	
	if (BUTTON.START)
	{
		int size;
		size = strlen(text_buffer); //Total size of string
		if (size > 0)
		{
			Game_State = 1;
		}
	}
}

void Draw_Virtual_Keyboard()
{
	int i;
	// Draw Virtual keyboard
	Faster_clearing(12, 350, 640-24, 120, BLUE_COLOR_BG);
	for(i=0;i<3;i++)
	{
		Print_text(16, 360+(i*30), 23, rows_of_text[i]);
	}
	Print_text(280, 450, 23, "[OK]");
}

void Update_Progress(int a, int match, int all_match, char* img_filename)
{
	char tmp2_str[512];
	Faster_clearing(0, 0, 640, 480, WHITE_COLOR_BG);
	Print_text(32, 120, 20, "Downloading images on page...");
	Print_text(32, 180, 20, "Please wait !");
	
	snprintf(img_tmp_str[a], sizeof(img_tmp_str[a]), "%s", img_filename);
	
	// Display progress of downloads
	snprintf(tmp2_str, sizeof(tmp2_str), "Image %d/%d, Total is %d", a, match, all_match);
	Print_text(32, 250, 20, tmp2_str);
	
	// Display Image filename
	Print_text(32, 350, 20, img_tmp_str[a]);
	
	Update_video();
}

void Move_Cursor()
{	
	if (BUTTON.UP) player_y -= 3;
	else if (BUTTON.DOWN) player_y += 3;
				
	if (BUTTON.LEFT) player_x -= 3;
	else if (BUTTON.RIGHT) player_x += 3;
}

void Draw_Cursor()
{	
	// Draw cursor
	Put_image(510, player_x, player_y);
}

void Download_Images(int page_number, int overall_page, int offset_start, int offset_max, int type)
{
	int i;
	size_t sz;
	char* str;
	
	Faster_clearing(0, 0, 640, 480, WHITE_COLOR_BG);
	Print_text(32, 120, 20, "Downloading images on page...");
	Print_text(32, 180, 20, "Please wait !");
	Update_video();
	
	if (offset_max > 70)
	{
		offset_max = 70;
	}

	/* The +1 is needed as page 0 on the actual website does not exist */
	snprintf(tag_str, sizeof(tag_str), COMMON_URL_PAGE "/post/list/%s/%d", text_buffer, overall_page+1);

	// Step 2

	/* Create Folder img for storing our images and tmp for the html files.
	 * img is for the permanent image folder
	 * tmp is for the HTML files
	*/
	create_directory(IMG_DIRECTORY, 0755);
	create_directory(THUMB_DIRECTORY, 0755);
	create_directory(TMP_DIRECTORY, 0755);

	snprintf(tmp_str, sizeof(tmp_str), TMP_DIRECTORY"/%s-page%d.html", text_buffer, overall_page);
	// Don't redownload cached HTML file again
	if (access(tmp_str, F_OK) != 0)
	{
		/* We need to download the first page to determine how many pages are available */
		Download_file(tag_str, tmp_str, 0);
	}
	sz = Get_Filesize(tmp_str);
	str = Read_File(tmp_str, sz);
	/* From the first page, determine how many pages are available for the tag */
	pages = Determine_Number_Pages(str, sz);

	Read_HTMLFile(str, sz, overall_page, offset_start, offset_max, type, text_buffer);
	
	for(i=0;i<6;i++)
	{
		Clear_Image(10+i);
	}

	if (type == 1)
	{
		for(i=0;i<offset_max-offset_start;i++)
		{
			Load_Image_Stretch(10+i, thumbnail_image_filename[offset_start+i], 200, 200);
		}	
	}
	else
	{
		for(i=0;i<offset_max-offset_start;i++)
		{
			Load_Image_Stretch(10+i, image_filename[offset_start+i], 200, 200);
		}	
	}
		
	Game_State = 2;
}
		
int main(int argc, char** argv)
{
	char tmpp[256];
	int i;

	Init_Video();
	
	snprintf(text_buffer, sizeof(text_buffer), "Sonic_the_Hedgehog");
	
	Load_Image(510, "internal_img/cursor.bmp");
	Load_Image(511, "internal_img/font.bmp");
				
	current_thumbnail_page = 0;
	current_overall_page = 0;

	while(!BUTTON.QUIT)
	{
		Controls();
		
		switch(Game_State)
		{
			// Search
			case 0:
				Faster_clearing(0, 0, 640, 480, WHITE_COLOR_BG);
				
				if (delay_input > 10)
				{
					Virtual_Keyboard();
				}
				
				delay_input++;
				
				Print_text(32, 20, 20, "R34SDL");
				Print_text(32, 50, 20, "Search for a character");
				
				// Print text that's in buffer
				Print_text(32, 120, 20, text_buffer);
				
				Draw_Virtual_Keyboard();
				
				Move_Cursor();
				Draw_Cursor();
			break;
			case 1:
				// First page, from 0 to 6
				Download_Images(current_thumbnail_page, current_overall_page, (6 * current_thumbnail_page), (6 * current_thumbnail_page) + 6, 1);
			break;
			case 2:
				Faster_clearing(0, 0, 640, 480, WHITE_COLOR_BG);
				
				for(i=0;i<3;i++)
				{
					Put_image(10 + i, 10 + (i * 210), 40);
					Put_image(13 + i, 10 + (i * 210), 250);
				}
				
				
				// Display rag
				Print_text(20, 0, 20, text_buffer);
				
				snprintf(tmpp, sizeof(tmpp), "Page : %d", current_overall_page+1);
				Print_text(20, 20, 20, tmpp);
				snprintf(tmpp, sizeof(tmpp), "%d/%d", current_thumbnail_page*6, total_images);
				Print_text(540, 20, 20, tmpp);
				
				Move_Cursor();
				Draw_Cursor();
				
				delay_input++;
				
				if (delay_input > 10)
				{
					if (BUTTON.X) // Previous
					{
						if (current_thumbnail_page > 0)
						{
							current_thumbnail_page-= 1;
							Download_Images(current_thumbnail_page, current_overall_page, (6 * current_thumbnail_page), (6 * current_thumbnail_page) + 6, 1);
						}
						delay_input = 0;
					}
					else if (BUTTON.Y) // Previous
					{
						if (current_thumbnail_page < total_images / 6)
						{
							current_thumbnail_page+= 1;
							Download_Images(current_thumbnail_page, current_overall_page, (6 * current_thumbnail_page), (6 * current_thumbnail_page) + 6, 1);
						}
						delay_input = 0;
					}
					
					if (BUTTON.L) // Previous
					{
						if (current_overall_page > 0)
						{
							current_overall_page--;
							current_thumbnail_page = 0;
							Game_State = 1;
						}
						delay_input = 0;
					}
					else if (BUTTON.R) // Previous
					{
						if (current_overall_page < pages)
						{
							current_overall_page++;
							current_thumbnail_page = 0;
							Game_State = 1;
						}
						delay_input = 0;
					}
					else if (BUTTON.B) // Go back to Search
					{
						Game_State = 0;
						delay_input = 0;
					}
				}
				
			break;
		}
		

		
		Update_video();
	}
	
	return 0;
}
