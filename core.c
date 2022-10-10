#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "core.h"

int state_gui = 0;

/* This will be used to store where our images are in the HTML files */
int occurances[256];
/* This will store our URLs. I've noticed some urls can break the 256 barrier so i increased it to 512. */
char image_links[256][512];
/* This will store our filename, we can't assign the URLs as the filename*/
char image_filename[256][512];

/* Same but for thumbnails */
char thumbnail_image_links[256][512];
char thumbnail_image_filename[256][512];
int thumbnail_occurances[256];


const char ascii_chars[11] = {'0', '1', '2', '3','4','5','6','7','8','9'};

int tor = 0;
int pages = 1;

#ifdef WINDOWS
#define create_directory(a,x) mkdir(a)
#else
#define create_directory(a,x) mkdir(a,x)
#endif

/* We'll use libCURL for downloading the file. It should be possible to use any other alternatives too. */
void Download_file(const char* url, const char* file_name, int tor)
{
	FILE* file;
	CURL* handle;
	
	handle = curl_easy_init();
	
	if (tor > 0)
	{
		/* It's safer to use Socks4a for Tor as Socks5 might leak DNS without some proper handling */
		curl_easy_setopt(handle, CURLOPT_PROXY, "socks4a://127.0.0.1"); 
		switch(tor)
		{
			case 1:
				curl_easy_setopt(handle, CURLOPT_PROXYPORT, 9050);
			break;
			case 2:
				curl_easy_setopt(handle, CURLOPT_PROXYPORT, 9150);
			break;
		}
	}

	curl_easy_setopt( handle, CURLOPT_URL, url ) ;
	
#ifndef NO_DNS_DOH
	curl_easy_setopt(handle, CURLOPT_DOH_URL, "https://doh.opendns.com/dns-query");
#endif

	file = fopen( file_name, "w");

	curl_easy_setopt( handle, CURLOPT_WRITEDATA, file) ;

	curl_easy_perform( handle );

	curl_easy_cleanup( handle );

	fclose(file);
}

/* We need to get the size of our file and also partly to avoid an overflow. */
int Get_Filesize(const char* filepath)
{
	FILE* fi;
	long sz;
	fi = fopen(filepath, "r");
	if (!fi) return 0;
	
	fseek(fi, 0L, SEEK_END);
	sz = ftell(fi);
	fseek(fi, 0L, SEEK_SET);
	
	fclose(fi);
	
	return sz;
}

/* This function will store our HTML file in memory so we can read it */
char* Read_File(const char* filepath, int size)
{
	FILE* fi;
	char *str;
	
	fi = fopen(filepath, "r");
	if (!fi) return "";
	str = malloc(size);
	fread(str, 1, size, fi);
	fclose(fi);
	
	return str;
}

/*
 * UNUSED
void Write_File(const char* to_filepath, char* str, int size)
{
	FILE* fi;
	fi = fopen(to_filepath, "w+");
	if (fi == NULL) return;

	fwrite (str , sizeof(char), size, fi);

	if (fi != NULL) fclose(fi);
}
*/

/* This function is solely used to strip off our URL and just keep the filename */
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

/* Works in junction with Return_String, we can't use the URL as our image filename so i made this function to detect
 * where you should start for the filename.
*/
int Find_last_character(char* str, int size, char character)
{
	int i;
	int match = 0;
	for(i=0;i<size;i++)
	{
		if (str[i] == character)
		{
			match = i;
		}
	}

	return match;
}

/* The main function C:, deals with detecting where the links are, filling them in memory and then proceed to download them 
 * For multiple pages, we'll just put this in a loop.
 * */
void Read_HTMLFile(char* string, int size, int pa, int offset_start, int offset_end, int thumbnail_dl)
{
	int i, a;
	int match, new_match;
	int thumbnail_match, thumbnail_new_match;
	unsigned short result;
	char* tmp_str;
	
	// For for... checks with thumbnails or real file names (see below)
	char* cur_filename;
	char* cur_links;

	
	match = 0;

	/* This is the code that crawls through the HTTP file and tries to find the magic 
	 * for image URL links. We must also exclude thumbnails.
	 * I noticed that the image URL links were not HTTPS protected (bad !) so i'm relying on that.
	 * When this gets fixed, i'll have to find another way. (Which i'm sure won't be too hard)
	 * */
	for(i=1;i<size;i++)
	{
		if (string[i] == '<')
		{
			if (string[i+1] == 'a' && string[i+3] == 'h')
			{
				if (string[i-3] == 'b' && string[i-2] == 'r' && string[i-6] == 'a')
				{
					occurances[match] = i+9;
					match += 1;	
					//printf("Found Image Link at %d\n", i);
				}
			}
		}
	}
	
	// Now do the same for thumbnails
	thumbnail_match = 0;
	for(i=1;i<size;i++)
	{
		if (string[i] == '/')
		{
			
			if (string[i+1] == '_' && string[i+2] == 't' && string[i-31] == '"')
			{
				thumbnail_occurances[thumbnail_match] = i-24;
				thumbnail_match ++;
				//printf("Found Image Link at %d\n", i);
			}
		}
	}
	
	printf("%d Images were found on Page %d, downloading them\n", match, pa);

	/* Since we now know where each of our URLs are in the HTML file, we'll just set them to each position (where they were detecting) and
	 * fill our array until it encounters ", which means this is the end of the URL and we can proceed to the next URL.
	*/
	for(a=0;a<match;a++)
	{
		image_links[a][0] = string[occurances[a]];
		thumbnail_image_links[a][0] = string[thumbnail_occurances[a]];
		
		for(i=1;i<256;i++)
		{
			if (string[occurances[a]+i] == '"' && string[occurances[a]+i+1] == '>')
			{
				break;
			}
			image_links[a][i] = string[occurances[a]+i];
		}
		
		for(i=1;i<256;i++)
		{
			if (string[thumbnail_occurances[a]+i] == '"')
			{
				break;
			}
			thumbnail_image_links[a][i] = string[thumbnail_occurances[a]+i];
		}
		
		printf("URL link : %s\n", thumbnail_image_links[a]);
	}
	
	/* Strip the array in order to only keep the filename of the file */
	for(a=0;a<match;a++)
	{
		result = Find_last_character(image_links[a], 256, '/');
		tmp_str = Return_String(image_links[a], 256, result);
		snprintf(image_filename[a], 256-result, "img/%s", tmp_str);
		snprintf(thumbnail_image_filename[a], 256-result, "thumb/%s", tmp_str);
		free(tmp_str);
		//printf("Filename : %s\n", image_filename[a]);
	}
	
	/* If an end offset is set, set it to that. otherwise, download everything. */
	new_match = match;
	if (offset_end != 0)
	{
		new_match = offset_end;
		// Make sure it doesn't go above what was actually found
		if (new_match > match)
		{
			new_match = match;
		}
	}
	
	for(a=offset_start;a<new_match;a++)
	{
		/* Download thumbnails or the bigger pictures instead ?*/
		if (thumbnail_dl == 1)
		{
			cur_filename = thumbnail_image_filename[a];
			cur_links = thumbnail_image_links[a];
		}
		else
		{
			cur_filename = image_filename[a];
			cur_links = image_links[a];
		}
		
		Update_Progress(a, new_match, match, cur_filename);
		// Don't download the image again if it's already downloaded/cached
		if (access(cur_filename, F_OK) != 0)
		{
			Download_file(cur_links, cur_filename, tor) ;
		}
	}
	
	/* Make sure to empty the arrays to 0 to avoid leftovers */
	for(a=offset_start;a<new_match;a++)
	{
		memset(image_links[a], 0, sizeof(image_links[a]));
		memset(image_filename[a], 0, sizeof(image_filename[a]));
	}
}

/* This was slightly more tricky than i expected because i need to convert them to decimals
 * before we can process it and it was slightly more involved than i expected.
 * */
int Determine_Number_Pages(char* string, int size)
{
	int i, a, c;
	char tmp[5];
	int n[5], size_n = 0, truenumber = 0;
	
	/* 
	 * This is the code that crawls through the HTTP file and tries to find the word "Last".
	 * This is the only way to determine how many pages there are available for the tag.
	*/
	for(i=1;i<size;i++)
	{
		if (string[i] == 'L')
		{
			if (string[i+4] == '<')
			{
				if (string[i-2] == '"')
				{
					/* There may be more than 9 pages so this handles the reading of 2 numbers in the HTML file (buffer) */
					for(c=0;c<3;c++)
					{
						if (string[i-(3+c)] == '/') break;
						
						tmp[c] = string[i-(3+c)];
						for(a=0;a<10;a++)
						{
							if (tmp[c] == ascii_chars[a])
							{
								n[c] = a;
								size_n++;
							}
						}
					}

					if (size_n == 2)
					{
						truenumber = n[0] + n[1]*10;
					}
					else if (size_n == 3)
					{
						truenumber = n[0] + n[1]*10 + n[2]*100;
					}
					else
					{
						truenumber = n[0];
					}
					printf("Number of Pages %d\n", truenumber);
					return truenumber;
				}
			}
		}
	}
	
	printf("Only 1 Page was found\n");
	
	return 1;
}
