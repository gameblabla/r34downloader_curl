#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"

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
	
	printf("Rule34CurlC image downloader by Gameblabla\n");
	printf("This is very unoptimised but it has\nminimal dependencies so it should be fairly portable.\n\n");
	
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

	if (argc > 2)
	{
		if (argv[2][1] == 'b' && argv[2][0] == 't')
		{
			printf("Tor proxy (Browser, port 9150)\n\n");
			tor = 2;	
		}
		else if (argv[2][0] == 't')
		{
			printf("Tor proxy (port 9050)\n\n");
			tor = 1;
		}
	}
	
	/* Create Folder img for storing our images and tmp for the html files */
	create_directory("img", 0755);
	create_directory("tmp", 0755);
	
	/* We need to download the first page to determine how many pages are available */
	Download_file(tag_str, "tmp/page1.html", tor) ;

	sz = Get_Filesize("tmp/page1.html");
	str = Read_File("tmp/page1.html", sz);

	/* From the first page, determine how many pages are available for the tag */
	pages = Determine_Number_Pages(str, sz);
	
	/* If there are more than 1 page, then we will proceed to download each one of them. */
	if (pages > 1)
	{
		printf("Downloading %d pages\n", pages);
		for(i=2;i<=pages;i++)
		{
			printf("Progress : %d/%d\n", i, pages);
			snprintf(tag_str, sizeof(tag_str), "https://rule34.paheal.net/post/list/%s/%d", argv[1], i);
			snprintf(page_str, sizeof(page_str), "tmp/page%d.html", i);
			Download_file(tag_str, page_str, tor) ;
		}
	}
	//Write_File("1.html", str, sz);
	
	/* If there are more than 1 page, then we will proceed to download each one of them. */
	for(i=1;i<=pages;i++)
	{
		snprintf(page_str, sizeof(page_str), "tmp/page%d.html", i);
		printf("Downloading Images in Page %d\n", i);
		str = Read_File(page_str, sz);
		Read_HTMLFile(str, sz, i, 0, 0);
	}
	
	printf("Done !\nYou can find your images in the img folder !\nEnjoy fapping to them.\n");
	
	return 0;
}
