Rule34downloader_curl/R34SDL
======================

![R34SDL](https://raw.githubusercontent.com/gameblabla/r34downloader_curl/master/r34gui_main.png)

I got the idea of making this after seeing https://github.com/sunx2/r34py.

This was implemented in Python though and i hate Python so i decided to reimplent it in C.

I did not bother to take/convert any of the Python code and it works a bit differently from that one.

The initial version took me a day to program but as you can see, i also started to work on a GUI interface for video game consoles
like the Sega Dreamcast.

This one will take me more time but a POC is already working for PC and OpenDingux.

Usage
======

CLI
=====

You must run this program from a terminal or execute it from a script.
Below is an example usage:
```
rule34curl Sonic_the_Hedgehog f c
```
- First parameter: The tag
	Use underscores (_) instead of spaces, e.g. Sonic_the_Hedgehog or Genshin_Impact.

- Second parameter: Whether you want to download full pictures or thumbnails
	f = full/bigger pictures.
	t = thumbnails (good for quick overviews or slow connections).

- Third parameter: The connection type
	t = Tor background app (port 9050).
	tb = Tor Browser proxy mode (port 9150).
	c = clearnet (no proxy, DNS over HTTPS if compiled in).

If you omit this parameter entirely, the downloader defaults to clearnet.

**Examples**

Full images on clearnet:
```
rule34curl Genshin_Impact f
```
(No third parameter means clearnet.)

Thumbnails over Tor:
```
rule34curl Sonic_the_Hedgehog t t
```
(The final t here means Tor background on port 9050.)

Thumbnails over Tor Browser:
```
rule34curl Genshin_Impact t tb
```
Full images over Tor Browser:
```
    rule34curl Sonic_the_Hedgehog f tb
```

*Proxy Environment Variables*

This program uses libcurl. If you have the environment variables http_proxy or https_proxy set, curl will try to use those.

If you want to use a normal HTTP/HTTPS proxy instead of Tor, make sure to use the clearnet mode (c or omit the parameter) so it doesn’t conflict with Tor settings.

**Where Are the Images Stored?**

Downloaded images go into the img/ folder (created if it doesn’t exist). Thumbnails go there as well, unless you modify the code to place them elsewhere.

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

L/R : Switch pages (1-X)

X/Y : Switch pages for the images (generally from 0 to 70)

