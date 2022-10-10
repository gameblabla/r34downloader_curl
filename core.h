#ifndef CORE_H
#define CORE_H

extern int state_gui;

/* This will be used to store where our images are in the HTML files */
extern int occurances[256];
/* This will store our URLs. I've noticed some urls can break the 256 barrier so i increased it to 512. */
extern char image_links[256][512];
/* This will store our filename, we can't assign the URLs as the filename*/
extern char image_filename[256][512];
extern const char ascii_chars[11];

extern int tor;
extern int pages;

#ifdef WINDOWS
#define create_directory(a,x) mkdir(a)
#else
#define create_directory(a,x) mkdir(a,x)
#endif

// From cli.c and GUI.c
extern void Update_Progress(int a, int match, int all_match, char* img_filename);

extern void Download_file(const char* url, const char* file_name, int tor);
extern int Get_Filesize(const char* filepath);
extern char* Read_File(const char* filepath, int size);
extern char* Return_String(char* str, int size, int beginning);
extern int Find_last_character(char* str, int size, char character);
extern void Read_HTMLFile(char* string, int size, int pa, int offset_start, int offset_end);
extern int Determine_Number_Pages(char* string, int size);

#endif
