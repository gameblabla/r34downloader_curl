#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"
#include "common.h"

void Update_Progress(int a, int match, int all_match, char* img_filename)
{
	printf("Progress : %d/%d\n", a, match);	
}

int main(int argc, char** argv)
{
	long long sz;
	char *str;
	int i;
	char tag_str[256];
	char page_str[128];
	int thumbnail_download = 0;
	
	printf("Rule34CurlC image downloader by Gameblabla\n");
	printf("This is very unoptimised but it has\nminimal dependencies so it should be fairly portable.\n\nFor example, ./rule34curl Mario f c\n");
	
	if (argv[1] == NULL)
	{
		printf("No tags specified, Please specify one. Type _ rather than spaces for Tags.\n");
		exit(1);
	}
	else
	{
		printf("Tag is %s\n", argv[1]);
		snprintf(tag_str, sizeof(tag_str), "https://rule34.paheal.net/post/list/%s/1", argv[1]);
		
		printf("Link : %s\n", tag_str);
	}

	if (argc > 3)
	{
		if (argv[3][1] == 'b' && argv[3][0] == 't')
		{
			printf("Tor proxy (Browser, port 9150)\n\n");
			tor = 2;	
		}
		else if (argv[3][0] == 't')
		{
			printf("Tor proxy (port 9050)\n\n");
			tor = 1;
		}
		else if (argv[3][0] == 'c')
		{
			printf("Clearnet (with DNS over HTTPS if support is compiled in)\n\n");
		}
	}
	
	if (argc > 2)
	{
		if (argv[2][0] == 't')
		{
			printf("Download thumbnails instead\n\n");
			thumbnail_download = 1;
		}
		else if (argv[2][0] == 'f')
		{
			printf("Download full pictures instead\n\n");
		}
	}
	
	/* Create Folder img for storing our images and tmp for the html files */
	create_directory("img", 0755);
	create_directory("thumb", 0755);
	create_directory("tmp", 0755);
	
	/* We need to download the first page to determine how many pages are available */
	snprintf(page_str, sizeof(page_str), "tmp/%s-page1.html", argv[1]);
	Download_file(tag_str, page_str, tor) ;
	sz = Get_Filesize(page_str);
	str = Read_File(page_str, sz);

	/* From the first page, determine how many pages are available for the tag */
	pages = Determine_Number_Pages(str, sz);
	
	/* If there are more than 1 page, then we will proceed to download each one of them. */
	if (pages > 1)
	{
		printf("Downloading %d pages\n", pages);
		for(i=2;i<=pages;i++)
		{
			printf("Progress : %d/%d\n", i, pages);
			snprintf(tag_str, sizeof(tag_str), COMMON_URL_PAGE "post/list/%s/%d", argv[1], i);
			snprintf(page_str, sizeof(page_str), "tmp/%s-page%d.html", argv[1], i);
			Download_file(tag_str, page_str, tor) ;
		}
	}
	//Write_File("1.html", str, sz);
	
	/* If there are more than 1 page, then we will proceed to download each one of them. */
	for(i=1;i<=pages;i++)
	{
		snprintf(page_str, sizeof(page_str), "tmp/%s-page%d.html", argv[1], i);
		printf("Downloading Images in Page %d\n", i);
		str = Read_File(page_str, sz);
		Read_HTMLFile(str, sz, i, 0, 0, thumbnail_download);
	}
	
	printf("Done !\nYou can find your images in the img folder !\nEnjoy fapping to them.\n");
	
	return 0;
}
