#ifndef COMMON_H
#define COMMON_H

#define COMMON_URL_PAGE "https://rule34.paheal.net"

#ifdef DREAMCAST
// Store these on a RAMdisk
// I may have to look for a better solution though
#define THUMB_DIRECTORY "/ramdisk/thumb"
#define IMG_DIRECTORY "/ramdisk/img"
#define TMP_DIRECTORY "/ramdisk/tmp"
#else
#define THUMB_DIRECTORY "thumb"
#define IMG_DIRECTORY "img"
#define TMP_DIRECTORY "tmp"
#endif

#endif
