# ShadertoyHarness
A c++ wrapper to let you step through shadertoy code for debugging purposes.

Drop your shadertoy source into main.cpp, with #include "glslAdapters.h" at the top.  Fix compile errors, and step through shader code!

Settings.h has some settings such as image resolution and the file name of the bmp file generated.

Many features and functions are missing, and some of the features I have added may not work correctly in all circumstances (such as swizzling).

Hopefully better than nothing.

I'm definitely open to changes if you can improve things!

I don't have a lot of free time, so just threw something together that worked well enough for my needs.