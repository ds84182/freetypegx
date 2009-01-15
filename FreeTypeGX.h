/* 
 * FreeTypeGX is a wrapper class for libFreeType which renders a compiled
 * FreeType parsable font into a GX texture for Wii homebrew development.
 * Copyright (C) 2008 Armin Tamzarian
 * 
 * This file is part of FreeTypeGX.
 * 
 * FreeTypeGX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FreeTypeGX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with FreeTypeGX.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \mainpage FreeTypeGX
 *
 * \section sec_intro Introduction
 * 
 * FreeTypeGX is a wrapper class for libFreeType which renders a compiled FreeType parsable font into a GX texture for Wii homebrew development.
 * <br>
 * FreeTypeGX is written in C++ and makes use of a selectable pre-buffered or buffer-on-demand methodology to allow fast and efficient printing of text to the EFB.  
 * <p>
 * This library was developed in-full by Armin Tamzarian with the support of developers in \#wiibrew on EFnet. 
 * 
 * \section sec_installation_source Installation (Source Code)
 * 
 * -# Ensure that you have the <a href = "http://www.tehskeen.com/forums/showthread.php?t=9404">libFreeType</a> Wii library installed in your development environment with the library added to your Makefile where appropriate.
 * -# Ensure that you have the <a href = "http://code.google.com/p/metaphrasis">Metaphrasis</a> library installed in your development environment with the library added to your Makefile where appropriate.
 * -# Extract the FreeTypeGX archive.
 * -# Copy the contents of the <i>src</i> directory into your project's development path.
 * -# Place a copy of the desired TrueType or OpenType font you wish to use in the FreeTypeGX/ttf directory.
 * -# Modify the fontface.s file and change the value of the incbin variable to point to your font.
 * -# Modify the fontface.s file and change the value of the fontsize variable to the byte-size of the font file.
 * -# Include the FreeTypeGX header file in your code using syntax such as the following:
 * \code
 * #include "FreeTypeGX.h"
 * \endcode
 * 
 * \section sec_installation_library Installation (Library)
 * 
 * -# Ensure that you have the <a href = "http://www.tehskeen.com/forums/showthread.php?t=9404">libFreeType</a> Wii library installed in your development environment with the library added to your Makefile where appropriate.
 * -# Ensure that you have the <a href = "http://code.google.com/p/metaphrasis">Metaphrasis</a> library installed in your development environment with the library added to your Makefile where appropriate.
 * -# Extract the FreeTypeGX archive.
 * -# Copy the contents of the <i>lib</i> directory into your <i>devKitPro/libogc</i> directory.
 * -# Include the FreeTypeGX header file in your code using syntax such as the following:
 * \code
 * #include "FreeTypeGX.h"
 * \endcode
 * 
 * \section sec_usage Usage
 * 
 * -# Within the file you included the FreeTypeGX.h file create an instance object of the FreeTypeGX class passing in the desired font point size:
 * \code
 * FreeTypeGX *freeTypeGX = new FreeTypeGX(32);
 * \endcode
 * A;ternately you can specify a flag which will load and cache all available font glyphs immidiately. Note that on large font sets enabling this feature could take a significant amount of time. 
 * \code
 * FreeTypeGX *freeTypeGX = new FreeTypeGX(32, true);
 * \endcode
 * Furthermore you can also specify a texture format to which you would like to render the font characters. 
 * \code
 * FreeTypeGX *freeTypeGX = new FreeTypeGX(32, true, GX_TF_RGB565);
 * \endcode
 * Lastly, you can also specify a positional format which you define in your graphics subsystem declaration. 
 * \code
 * FreeTypeGX *freeTypeGX = new FreeTypeGX(32, true, GX_TF_RGB565, GX_POS_XY);
 * \endcode
 * Currently supported textures are:
 * \li <i>GX_TF_I4</i>
 * \li <i>GX_TF_I8</i>
 * \li <i>GX_TF_IA4</i>
 * \li <i>GX_TF_IA8</i>
 * \li <i>GX_TF_RGB565</i>
 * \li <i>GX_TF_RGB5A3</i>
 * \li <i>GX_TF_RGBA8</i>
 * -# Using the allocated FreeTypeGX instance object call the drawText function to print a string at the specified screen X and Y coordinates to the current EFB:
 * \code
 * freeTypeGX->drawText(10, 25, _TEXT("FreeTypeGX Rocks!"));
 * \endcode
 * Alternately you can specify a <i>GXColor</i> object you would like to apply to the printed characters:
 * \code
 * freeTypeGX->drawText(10, 25, _TEXT("FreeTypeGX Rocks!"), (GXColor){0xff, 0xee, 0xaa, 0xff});
 * \endcode
 *
 * \section sec_license License
 * 
 * FreeTypeGX is distributed under the GNU Lesser General Public License.
 *    
 * \section sec_contact Contact
 * 
 * If you have any suggestions, questions, or comments regarding this library feel free to e-mail me at tamzarian1989 [at] gmail [dawt] com.
 */

#ifndef FREETYPEGX_H_
#define FREETYPEGX_H_

#include <gccore.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Metaphrasis.h>

#include <malloc.h>
#include <string.h>
#include <map>


extern char fontface[];	/**< TrueType font face initializer from fontface.s */
extern int fontsize;	/**< TrueType font face initializer from fontface.s */

/*! \struct fontCharData_
 * 
 * Font face character glyph relevant data structure.
 */
typedef struct fontCharData_ {
	uint16_t glyphAdvanceX;	/**< Character glyph X coordinate advance in pixels. */

	uint16_t textureWidth;	/**< Texture width in pixels/bytes. */
	uint16_t textureHeight;	/**< Texture glyph height in pixels/bytes. */

	uint16_t renderOffsetY;	/**< Texture Y axis bearing offset. */
	uint16_t renderOffsetMax;	/**< Texture Y axis bearing maximum value. */
	uint16_t renderOffsetMin;	/**< Texture Y axis bearing minimum value. */

	uint32_t* glyphDataTexture;	/**< Glyph texture bitmap data buffer. */
} fontCharData;

#define _TEXT(t) L ## t /**< Unicode helper macro. */

/*! \class FreeTypeGX
 * \brief Wrapper class for the libFreeType library with GX rendering.
 * \author Armin Tamzarian
 * \version 0.2.2
 * 
 * FreeTypeGX acts as a wrapper class for the libFreeType library. It supports precaching of transformed glyph data into
 * a specified texture format. Rendering of the data to the EFB is accomplished through the application of high performance
 * GX texture functions resulting in high throughput of string rendering.  
 */
class FreeTypeGX {

	private:
		FT_Library ftLibrary;	/**< FreeType FT_Library instance. */
		FT_Face ftFace;			/**< FreeType reusable FT_Face typographic object. */
		FT_GlyphSlot ftSlot;	/**< FreeType reusable FT_GlyphSlot glyph container object. */

		uint8_t textureFormat;		/**< Defined texture format of the target EFB. */
		uint8_t positionFormat;		/**< Defined position format of the texture. */
		std::map<wchar_t, fontCharData> fontData; /**< Map which holds the glyph data structures for the corresponding characters. */

		static uint16_t adjustTextureWidth(uint16_t textureWidth, uint8_t textureFormat);
		static uint16_t adjustTextureHeight(uint16_t textureHeight, uint8_t textureFormat);

		uint16_t cacheAllGlyphData();
		fontCharData *cacheGlyphData(wchar_t charCode);
		void loadGlyphData(FT_Bitmap *bmp, fontCharData *charData);
		void copyTextureToFramebuffer(GXTexObj *texObj, uint16_t texWidth, uint16_t texHeight, uint16_t screenX, uint16_t screenY, GXColor color);

	public:
		FreeTypeGX(FT_UInt pointSize, bool cacheAll = false, uint8_t textureFormat = GX_TF_RGBA8, uint8_t positionFormat = GX_POS_XYZ);
		~FreeTypeGX();

		uint16_t drawText(uint16_t x, uint16_t y, wchar_t *text, GXColor color);
		uint16_t drawText(uint16_t x, uint16_t y, wchar_t *text);
		uint16_t drawText(uint16_t x, uint16_t y, wchar_t const *text, GXColor color);
		uint16_t drawText(uint16_t x, uint16_t y, wchar_t const *text);

		uint16_t getWidth(wchar_t *text);
		uint16_t getWidth(wchar_t const *text);
		uint16_t getHeight(wchar_t *text);
		uint16_t getHeight(wchar_t const *text);
};

#endif /* FREETYPEGX_H_ */
