Rule34downloader_curl/R34SDL
======================

![R34SDL](https://raw.githubusercontent.com/gameblabla/r34downloader_curl/master/r34gui_main.png)

I got the idea of making this after seeing https://github.com/sunx2/r34py.

This was implemented in Python though and i hate Python so i decided to reimplent it in C.

I did not bother to take/convert any of the Python code and it works a bit differently from that one.

The initial version took me a day to program but as you can see, i also started to work on a GUI interface for video game consoles
like the Sega Dreamcast.

This one will take me more time but a POC is already working.

Usage
======

CLI
=====

You must use a terminal or execute it from a script file. For example :

```
rule34curl Sonic_the_Hedgehog f c
```
Second parameter is the tag.

Make sure to use _ for spaces. For example, Sonic_the_Hedgehog.

3rd parameter is whenever you want to download the full pictures or the thumbnails.

f means the bigger pictures should be downloaded.

t means the thumbnails should be downloaded instead.

Thumbnails are a good idea if you just want to have a quick overview or are behind dialup and/or slow internet connection/proxy.

4th parameter is for the connection type.

The "t" means you want to download it over Tor (uses port 9050), remove it if you want to download over clearnet. (without proxies)
Use "tb" if you use the Tor browser instead. (it uses port 9150 instead)
C means clearnet. You don't need to specify this though.

Since this uses CURL, it should also be able to make use of the http_proxy & https_proxy environment variables.
If you do use that option, make sure to use the clearnet option instead.

The images will be downloaded to the folder img.

GUI
=====

Make sure you have SDL 1.2 available. This is only provided as a source only port for now.

The first thing that happens is a virtual keyboard is being displayed on screen.

It is not a QWERTY or AZERTY layout : rather, it uses a stripped down Alphabet keyboard with capital and lowercase letters and numbers.

This was done as to make it less painful for me to program and having a QWERTY virtual keyboard wouldn't improve things.

Controls
========

A : Select a letter or number in the virtual keyboard OR download full picture when in view mode.
B : Delete a letter or number OR go back to virtual keyboard.

L/R : Switch pages
X/Y : Switch pages for thumbnails

