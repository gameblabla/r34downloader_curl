#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "core.h"
#include "common.h"

static void print_usage(const char* program_name)
{
    printf("Rule34CurlC image downloader by Gameblabla\n");
    printf("Minimal dependencies, yet somewhat unoptimized.\n\n");
    printf("Usage: %s <tag> [thumbnail|full] [tb|tor|c]\n", program_name);
    printf("\n");
    printf("  <tag>         Required. Use underscores instead of spaces (e.g. Genshin_Impact).\n");
    printf("  [thumbnail]   Download thumbnails only.\n");
    printf("  [full]        Download full-size images (default if omitted).\n");
    printf("\n");
    printf("  [tb]          Use Tor Browser proxy (port 9150).\n");
    printf("  [tor]         Use Tor background app (port 9050).\n");
    printf("  [c]           Clearnet with DNS over HTTPS (default if omitted).\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s Genshin_Impact\n", program_name);
    printf("        -> downloads full pictures on clearnet by default.\n\n");
    printf("  %s Genshin_Impact thumbnail c\n", program_name);
    printf("        -> downloads thumbnails on clearnet.\n\n");
    printf("  %s Genshin_Impact full tor\n", program_name);
    printf("        -> downloads full images over Tor (port 9050).\n\n");
    printf("  %s Genshin_Impact thumbnail tb\n", program_name);
    printf("        -> downloads thumbnails over Tor Browser (port 9150).\n\n");
    exit(1);
}

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
    
    if (argc < 2) {
        print_usage(argv[0]);
    }

    /* 1) Parse the first argument: the tag. */
    char* tag_arg = argv[1];
    printf("Tag is %s\n", tag_arg);

    /* 2) Default settings if not provided:
          - thumbnail_download = 0 (full)
          - tor = 0 (clearnet)  */
    thumbnail_download = 0; 
    tor = 0;

    /* 3) Parse the second argument if present (thumbnail/full). */
    if (argc >= 3) {
        if (strcasecmp(argv[2], "thumbnail") == 0) {
            thumbnail_download = 1;
            printf("Download thumbnails.\n");
        }
        else if (strcasecmp(argv[2], "full") == 0) {
            thumbnail_download = 0;
            printf("Download full-size images.\n");
        }
        else if (strcasecmp(argv[2], "tb") == 0) {
            /* They specified the network mode, but not the image mode -> full is default */
            tor = 2;
            printf("Tor Browser (port 9150)\n");
        }
        else if (strcasecmp(argv[2], "tor") == 0) {
            tor = 1;
            printf("Tor background (port 9050)\n");
        }
        else if (strcasecmp(argv[2], "c") == 0) {
            tor = 0;
            printf("Clearnet mode.\n");
        }
        else {
            /* Unrecognized second argument */
            printf("Unrecognized option: %s\n", argv[2]);
            print_usage(argv[0]);
        }
    }

    /* 4) Parse the third argument if present (tb, tor, c). 
          Only if the second argument was recognized as thumbnail/full. */
    if (argc >= 4) {
        if (strcasecmp(argv[3], "tb") == 0) {
            tor = 2;
            printf("Tor Browser (port 9150)\n");
        }
        else if (strcasecmp(argv[3], "tor") == 0) {
            tor = 1;
            printf("Tor background (port 9050)\n");
        }
        else if (strcasecmp(argv[3], "c") == 0) {
            tor = 0;
            printf("Clearnet mode.\n");
        }
        else {
            printf("Unrecognized option: %s\n", argv[3]);
            print_usage(argv[0]);
        }
    }

    printf("\nFinal settings:\n");
    printf("  Tag: %s\n", tag_arg);
    printf("  Mode: %s\n", thumbnail_download ? "thumbnails" : "full images");
    if (tor == 2) {
        printf("  Network: Tor Browser (port 9150)\n\n");
    } else if (tor == 1) {
        printf("  Network: Tor (port 9050)\n\n");
    } else {
        printf("  Network: clearnet\n\n");
    }

    /* Build the page 1 URL. */
    snprintf(tag_str, sizeof(tag_str), "https://rule34.paheal.net/post/list/%s/1", tag_arg);
    printf("Link : %s\n", tag_str);

    /* Create directories for storing images and HTML files. */
    create_directory(IMG_DIRECTORY, 0755);
    create_directory(THUMB_DIRECTORY, 0755);
    create_directory(TMP_DIRECTORY, 0755);
    
    /* Download the first page to figure out how many pages are available. */
    snprintf(page_str, sizeof(page_str), TMP_DIRECTORY"/%s-page1.html", tag_arg);
    Download_file(tag_str, page_str, tor);
    sz = Get_Filesize(page_str);
    str = Read_File(page_str, sz);

    /* Determine how many pages are available for this tag. */
    pages = Determine_Number_Pages(str, sz);

    /* If more than 1 page, download them all. */
    if (pages > 1)
    {
        printf("Downloading %d pages\n", pages);
        for (i = 2; i <= pages; i++)
        {
            printf("Progress : %d/%d\n", i, pages);
            snprintf(tag_str, sizeof(tag_str), "https://rule34.paheal.net/post/list/%s/%d", tag_arg, i);
            snprintf(page_str, sizeof(page_str), TMP_DIRECTORY"/%s-page%d.html", tag_arg, i);
            Download_file(tag_str, page_str, tor);
        }
    }

    /* Parse each downloaded page and grab images. */
    for (i = 1; i <= pages; i++)
    {
        snprintf(page_str, sizeof(page_str), TMP_DIRECTORY"/%s-page%d.html", tag_arg, i);
        printf("Downloading Images in Page %d\n", i);
        free(str); // optional: free str from the previous iteration
        sz = Get_Filesize(page_str);
        str = Read_File(page_str, sz);

        /* Read_HTMLFile will extract links and download them (or thumbnails). */
        Read_HTMLFile(str, sz, i, 0, 0, thumbnail_download, tag_arg);
    }

    printf("Done!\nYou can find your images in the \"%s\" folder.\n", IMG_DIRECTORY);
    printf("Enjoy!\n");

    free(str);
    return 0;
}
