# Introduction #

FreeTypeGX is a wrapper class for libFreeType which renders a compiled FreeType parsable font into a GX texture for Wii homebrew development.
FreeTypeGX is written in C++ and makes use of a selectable pre-buffered or buffer-on-demand methodology to allow fast and efficient printing of text to the EFB.

**Note:** As of version 0.2.1 FreeTypeGX has forked into two disparate projects and now relies upon the [Metaphrasis](http://code.google.com/p/metaphrasis) library.

This library was developed in-full by Armin Tamzarian with the support of developers in #wiidev on EFnet.

Full Doxygen API documentation is included within the download archive for assistance with program integration.

# Installation (Source Code) #

  1. Ensure that you have the [FreeType](http://sourceforge.net/projects/devkitpro/files/portlibs/) Wii library installed in your development environment with the library added to your Makefile where appropriate.
  1. Ensure that you have the [Metaphrasis](http://code.google.com/p/metaphrasis) library installed in your development environment with the library added to your Makefile where appropriate.
  1. Extract the FreeTypeGX archive.
  1. Copy the contents of the src directory into your project's development path.
  1. Include the FreeTypeGX header file in your code using syntax such as the following:
```
#include "FreeTypeGX.h"
```

# Installation (Library) #

  1. Ensure that you have the [FreeType](http://sourceforge.net/projects/devkitpro/files/portlibs/) Wii library installed in your development environment with the library added to your Makefile where appropriate.
  1. Ensure that you have the [Metaphrasis](http://code.google.com/p/metaphrasis) library installed in your development environment with the library added to your Makefile where appropriate.
  1. Extract the FreeTypeGX archive.
  1. Copy the contents of the lib directory into your devKitPro/libogc directory.
  1. Include the FreeTypeGX header file in your code using syntax such as the following:
```
#include "FreeTypeGX.h"
```

# FreeTypeGX Prerequisites #
Before you begin using FreeTypeGX in your project you must ensure that the you have some method by which to allocate the font data buffer for use within the initialization routines. For examples showing the most common methods to generate a font data buffer for use with FreeTypeGX see the included examples. These examples illustrate the following methods:

> _example1_ - Compiling a font into the executable

> _example2_ - Loading a font from a file located on an SD card or USB drive

Note that both of these example result with the creation of the following example variables:

> `uint8_t* rursus_compact_mono_ttf` - A buffer containing the font data.

> `FT_Long rursus_compact_mono_ttf_size` - A long value containing the size of the font data buffer in bytes.

# FreeTypeGX Usage #

  * Within the file you included the FreeTypeGX.h header create an instance object of the FreeTypeGX class:
```
FreeTypeGX *freeTypeGX = new FreeTypeGX();
```
> Alternately you can specify a texture format to which you would like to render the font characters. Note that the default value for this parameter is GX\_TF\_RGBA8.
```
FreeTypeGX *freeTypeGX = new FreeTypeGX(GX_TF_RGB565);
```
> Furthermore, you can also specify a positional format as defined in your graphics subsystem initialization. Note that the default value for this parameter is GX\_POS\_XYZ.
```
FreeTypeGX *freeTypeGX = new FreeTypeGX(GX_TF_RGB565, GX_POS_XY);
```

> Currently supported textures are:
    * GX\_TF\_I4
    * GX\_TF\_I8
    * GX\_TF\_IA4
    * GX\_TF\_IA8
    * GX\_TF\_RGB565
    * GX\_TF\_RGB5A3
    * GX\_TF\_RGBA8

> Currently supported position formats are:
    * GX\_POS\_XY
    * GX\_POS\_XYZ

  * Using the allocated FreeTypeGX instance object call the loadFont function to load the font from the compiled buffer and specify the desired point size:
```
fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, 64);
```
> Alternately you can specify a flag which will load and cache all available font glyphs immidiately. Note that on large font sets enabling this feature could take a significant amount of time.
```
fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, 64, true);
```
  * Using the allocated FreeTypeGX instance object call the drawText function to print a string at the specified screen X and Y coordinates to the current EFB:
```
freeTypeGX->drawText(10, 25, _TEXT("FreeTypeGX Rocks!"));
```
> Alternately you can specify a GXColor object you would like to apply to the printed characters:
```
freeTypeGX->drawText(10, 25, _TEXT("FreeTypeGX Rocks!"),
                     (GXColor){0xff, 0xee, 0xaa, 0xff});
```
> Furthermore you can also specify a group of styling parameters which will modify the positioning or style of the text:
```
freeTypeGX->drawText(10, 25, _TEXT("FreeTypeGX Rocks!"),
                     (GXColor){0xff, 0xee, 0xaa, 0xff},
                     FTGX_JUSTIFY_CENTER | FTGX_ALIGN_BOTTOM | FTGX_STYLE_UNDERLINE);
```

> Currently style parameters are:
    * FTGX\_JUSTIFY\_LEFT
    * FTGX\_JUSTIFY\_CENTER
    * FTGX\_JUSTIFY\_RIGHT
    * FTGX\_ALIGN\_TOP
    * FTGX\_ALIGN\_MIDDLE
    * FTGX\_ALIGN\_BOTTOM
    * FTGX\_STYLE\_UNDERLINE
    * FTGX\_STYLE\_STRIKE

# License #

FreeTypeGX is distributed under the GNU Lesser General Public License.

# Contact #

If you have any suggestions, questions, or comments regarding this library feel free to e-mail me at tamzarian1989 [at](at.md) gmail [dawt](dawt.md) com.