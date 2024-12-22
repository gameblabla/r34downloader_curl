#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include "core.h"
#include "common.h"

int state_gui = 0;

#define MAX_OCCURANCES 256

/* This will be used to store where our images are in the HTML files */
int occurances[MAX_OCCURANCES];
/* This will store our URLs. I've noticed some urls can break the 256 barrier so i increased it to 512. */
char image_links[MAX_OCCURANCES][512];
/* This will store our filename, we can't assign the URLs as the filename*/
char image_filename[MAX_OCCURANCES][512];

/* Same but for thumbnails */
char thumbnail_image_links[MAX_OCCURANCES][512];
char thumbnail_image_filename[MAX_OCCURANCES][512];
int thumbnail_occurances[MAX_OCCURANCES];


const char ascii_chars[11] = {'0', '1', '2', '3','4','5','6','7','8','9'};

int tor = 0;
int pages = 1;
int total_images = 0;

#ifdef WINDOWS
#define create_directory(a,x) mkdir(a)
#else
#define create_directory(a,x) mkdir(a,x)
#endif

/* We'll use libCURL for downloading the file. It should be possible to use any other alternatives too. */
void Download_file(const char* url, const char* file_name, int tor)
{
	FILE* file;
	int res;
	CURL* handle;
	
	handle = curl_easy_init();
	
#ifndef NO_TOR
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
#endif

	curl_easy_setopt( handle, CURLOPT_URL, url ) ;
	
	curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
	
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

/* You can enable this if you live in a free country... I don't : ( */
#ifndef NO_DNS_DOH
	curl_easy_setopt(handle, CURLOPT_DOH_URL, "https://cloudflare-dns.com/dns-query");
#endif

	file = fopen( file_name, "wb");

	curl_easy_setopt( handle, CURLOPT_WRITEDATA, file) ;

	res = curl_easy_perform(handle);

	curl_easy_cleanup( handle );

   
    fclose(file);

    /*if (res != CURLE_OK)
    {
        // If you want to remove a 0-byte file on error:
        printf("0 byte file\n");
        exit(0);
    }*/

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
void Read_HTMLFile(char* string, int size, int pa, int offset_start, int offset_end, 
                   int thumbnail_dl, char* tag)
{
    int i, a;
    int match = 0;
    int thumbnail_match = 0;
    int new_match;
    unsigned short result;
    char* tmp_str;
    char temp[512];

    // Clear counters
    match = 0;
    thumbnail_match = 0;

    // 1) Find big image links: look for <a href='https://r34i.paheal-cdn.net/...'>
    //    This captures the "File Only" links that point to the large version.
    for(i = 0; i < size; i++)
    {
        // Make sure we have enough room to compare safely
        if (i + 34 < size &&
            strncmp(&string[i], "<a href='https://r34i.paheal-cdn.net/", 34) == 0)
        {
            // 9 = skip "<a href='" part
            occurances[match] = i + 9;
            match++;
            if (match >= MAX_OCCURANCES) break;
        }
    }

    // 2) Find thumbnail links: look for src='/_thumbs/...'
    //    We’ll prepend https://mario.sx to these later.
    for(i = 0; i < size; i++)
    {
        // 13 = length of "src='/_thumbs/"
        if (i + 13 < size &&
            strncmp(&string[i], "src='/_thumbs/", 13) == 0)
        {
            // 5 = skip "src='"
            thumbnail_occurances[thumbnail_match] = i + 5;
            thumbnail_match++;
            if (thumbnail_match >= MAX_OCCURANCES) break;
        }
    }

    printf("%d Images were found on Page %d, downloading them\n", match, pa);

    // 3) Extract the actual links from the HTML buffer
    //    Then build filenames for each.
    for(a = 0; a < match; a++)
    {
        // Initialize the destination arrays to avoid junk characters
        memset(image_links[a], 0, sizeof(image_links[a]));
        memset(thumbnail_image_links[a], 0, sizeof(thumbnail_image_links[a]));
        memset(image_filename[a], 0, sizeof(image_filename[a]));
        memset(thumbnail_image_filename[a], 0, sizeof(thumbnail_image_filename[a]));

        // Parse the large image link (r34i.paheal-cdn.net)
        // Stop at the next quote (') or double-quote (") or string end
        for(i = 0; i < 255; i++)
        {
            char c = string[occurances[a] + i];
            if (c == '\0' || c == '\'' || c == '\"') break;
            image_links[a][i] = c;
        }

        // Parse the thumbnail link (/_thumbs/...)
        for(i = 0; i < 255; i++)
        {
            char c = string[thumbnail_occurances[a] + i];
            if (c == '\0' || c == '\'' || c == '\"') break;
            thumbnail_image_links[a][i] = c;
        }

        // 4) Prepend the domain for thumbnails
        snprintf(temp, sizeof(temp), COMMON_URL_PAGE"%s", thumbnail_image_links[a]);
        strcpy(thumbnail_image_links[a], temp);

        // 5) Build the filenames
        //    For main images, we just take the last chunk after '/' and add ".jpg".
        result = Find_last_character(image_links[a], 256, '/');
        tmp_str = Return_String(image_links[a], 256, result);
        snprintf(image_filename[a], 256, IMG_DIRECTORY"%s.jpg", tmp_str);
        free(tmp_str);

        // For thumbnail we’re using "page%d-%d-thumb.jpg"
        snprintf(thumbnail_image_filename[a], 256,
                 THUMB_DIRECTORY"/page%d-%d-thumb.jpg", pa, a);
    }

    // 6) Offset logic
    new_match = match;
    if (offset_end != 0)
    {
        new_match = offset_end;
        if (new_match > match)
            new_match = match;
    }

    // 7) Download loop
    for(a = offset_start; a < new_match; a++)
    {
        char* cur_filename;
        char* cur_links;

        // Decide if we want to download thumbnails or full images
        if (thumbnail_dl == 1)
        {
            cur_filename = thumbnail_image_filename[a];
            cur_links    = thumbnail_image_links[a];
        }
        else
        {
            cur_filename = image_filename[a];
            cur_links    = image_links[a];
        }

        Update_Progress(a, new_match, match, cur_filename);

        // Download only if not present
        if (access(cur_filename, F_OK) != 0)
        {
            Download_file(cur_links, cur_filename, tor);
        }
    }

    // Keep track of total images for later use
    total_images = match;
}


/* Retrieves total number of pages according to Last which implicitely tells us how many there are */
int Determine_Number_Pages(char* html, int size)
{
    char *last_ptr = strstr(html, ">Last</a>");
    if (!last_ptr) return 1;

    char *search_str = "href='/post/list/";
    char *p = last_ptr;
    char *found = NULL;

    while (p >= html) {
        char *maybe = strstr(p, search_str);
        if (maybe && maybe < last_ptr) {
            found = maybe;
            break;
        }
        if (p == html) break;
        p--;
    }

    if (!found) return 1;
    found += strlen(search_str);

    while (*found && *found != '/') found++;
    if (!*found) return 1;
    found++;

    int pages = 0;
    while (*found && isdigit((unsigned char)*found)) {
        pages = pages * 10 + (*found - '0');
        found++;
    }

    return pages == 0 ? 1 : pages;
}
