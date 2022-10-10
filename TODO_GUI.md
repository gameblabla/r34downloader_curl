SDL GUI TODO LIST
==================

Aside from the ports, everything else is pretty low on my list.

- Porting to platforms

Game consoles with toolchains that support libcurl
- Sega Dreamcast (potentially. BBA is supported and newer toolchain has support for Modem too)
- Gamecube (Only works for hardcoded IPs, no DNS resolver. BBA only. This was confirmed to work though.)
- Wii (libcurl precompiled binary provided. DNS resolver is available as a patch)
- WiiU
- Switch

Need to look further
- Original Xbox (will work over Linux but native might not work)
- Classic Mac OS (old libcurl might work)
- Sony PS2 (is the network adapter even supported ?)
- PS3 without OtherOS
- Sony PSP
- PS Vita (apparently is the case)

Not supported right now
- Megadrive with WIFI cartridge
The provided example can do some neat stuff but this would need a custom solution.
Also, we would need some other way to convert the images.
We could make our own proxy of sorts to take care of that but that makes things more complex...
Not worth bothering

- Sega Saturn
There is a modem for it and people are looking into separating the XBAND library as standalone.
Once this is done, we might look into it. For now, not possible.

- Xbox 360
This should be possible but isn't worth it.

- Animated GIF support

Enough said. I don't know a library for that though.

- MP4/WebM decoder

Optional and besides, Dreamcast won't be able to decode those even at a low resolution...
Gamecube might be able but not ideal anyway.

- Custom API frontends

Once the SDL 1.2 port is complete, we will want native PVR, GX, GX2 ports etc...

