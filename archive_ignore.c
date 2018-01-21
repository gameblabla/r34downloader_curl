#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int occurances[256];
int occurances_end[256];
char image_links[256][512];
char image_filename[256][512];

int Get_Filesize(const char* filepath)
{
	FILE* fi;
	long sz;
	fi = fopen(filepath, "r");
	if (fi == NULL) return 0;
	
	fseek(fi, 0L, SEEK_END);
	sz = ftell(fi);
	fseek(fi, 0L, SEEK_SET);
	
	if (fi == NULL) fclose(fi);
	
	return sz;
}

char* Read_File(const char* filepath, int size)
{
	FILE* fi;
	char *str, *tmp, *tmp2;
	
	fi = fopen(filepath, "r");
	if (fi == NULL) return "";
	
	str = malloc(size);
	tmp = malloc(size);
	tmp2 = malloc(size);
	
	while(fgets (tmp , size , fi)) 
	{
		snprintf(tmp2, size, "%s", str); 
		snprintf(str, size, "%s%s", tmp2, tmp);
	}

	if (fi != NULL) fclose(fi);
	if (tmp != NULL) free(tmp);
	if (tmp2 != NULL) free(tmp2);
	
	return str;
}


void Write_File(const char* to_filepath, char* str, int size)
{
	FILE* fi;
	fi = fopen(to_filepath, "w+");
	if (fi == NULL) return;

	fwrite (str , sizeof(char), size, fi);

	if (fi != NULL) fclose(fi);
}

char* Return_String(char* str, int size, int beginning)
{
	int i;
	char* cstr;
	
	cstr = malloc(size);
	
	for(i=0;i<size;i++)
	{
		cstr[i] = str[i+beginning];
	}
	
	return cstr;
};

int Find_last_character(char* str, int size, char character)
{
	int i;
	int match;
	for(i=0;i<size;i++)
	{
		if (str[i] == character)
		{
			match = i;
		}
	}
	
	printf("Last occurance of character, %d\n", match);
	
	return match;
}

/*<a href="http://*/
/*<a href="http://lotus.paheal.net/*/
void Read_String(char* string, int size)
{
	int i, a;
	int match;
	int result;
	match = 0;
	
	int jpg_found = 0;
	int png_found = 0, gif_found = 0;

	for(i=1;i<size;i++)
	{
		if (string[i] == '<' && string[i+1] == 'a')
		{
			if (string[i+3] == 'h' && string[i+4] == 'r')
			{
				if (string[i+5] == 'e' && string[i+6] == 'f' && string[i+9] == 'h')
				{
					if (string[i+10] == 't' && string[i+11] == 't' && string[i+12] == 'p' && string[i+13] == ':')
					{	
						if (string[i-1] == '>' && string[i-2] == 'r')
						{
							occurances[match] = i+9;
							match += 1;	
							printf("Found Image Link at %d\n", i);
						}
					}
				}
			}
		}
	}
	
	printf("%d Images found\n", match);
	/*
	for(i=1;i<size;i++)
	{
		if (string[i] == 'p')
		{
			if (string[i+2] == 'g' && string[i+4] == '>')
			{
				if (string[i+5] == 'I')
				{
					png_found++;
					printf("PNG at %d\n", i);
					occurances_end[match] = i+4;
					match += 1;	
				}
			}
		}
		else if (string[i] == 'j')
		{
			if (string[i+2] == 'g' && string[i+4] == '>')
			{
				if (string[i+5] == 'I')
				{
					jpg_found++;
					printf("JPG at %d, (No thumbs)\n", i);
					occurances_end[match] = i+4;
					match += 1;	
				}
			}	
		}
		else if (string[i] == 'g')
		{
			if (string[i+2] == 'f' && string[i+4] == '>')
			{
				if (string[i+5] == 'I')
				{
					gif_found++;
					printf("GIF at %d\n", i);
					occurances_end[match] = i+4;
					match += 1;	
				}
			}	
		}
	}
	printf("%d Images found with extensions\n", match);
	match = 0;
	printf("JPG, %d, PNG %d, GIF %d\n", jpg_found, png_found, gif_found);
	*/

	for(a=0;a<match;a++)
	{
		image_links[a][0] = string[occurances[a]];
		for(i=1;i<256;i++)
		{
			if (string[occurances[a]+i] == '"')
			{
				image_links[a][i+1] = '\n';
				break;
			}
			image_links[a][i] = string[occurances[a]+i];
		}
		printf("LINK : %s\n", image_links[a]);
	}
	
	for(a=0;a<match;a++)
	{
		result = Find_last_character(image_links[a], 256, '/');
		//snprintf(image_filename[a], 256-result, "%s", Return_String(image_links[a], 256, result));
		printf("Filename : %s\n", Return_String(image_links[a], 256, result+1));
	}
	
}

int main(int argc, char** argv)
{
	long long sz;
	FILE* fi;
	char *str;

	sz = Get_Filesize("1.html");
	str = malloc(sz);
	str = Read_File("1.html", sz);
	//Write_File("1.html", str, sz);

	Read_String(str, sz);
	
	return 0;
}
