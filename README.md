Rule34downloader_curl
======================

I got the idea of making this after seeing https://github.com/sunx2/r34py.

This was implemented in Python though and i hate Python so i decided to reimplent it in C.

I did not bother to take/convert any of the Python code and it works a bit differently from that one.

You must use a terminal or execute it from a script file. For example :

rule34curl Mario t

Mario is the tag you want to download images.

The "t" means you want to download it over Tor (uses port 9050), remove it if you want to download over clearnet. (without proxies)
Use "tb" if you use the Tor browser instead. (it uses port 9150 instead)

Since this uses CURL, it should also be able to make use of the http_proxy & https_proxy environment variables.

The images will be downloaded to the folder img.

This took me at least half a day to implement it.
