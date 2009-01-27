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

#include "FreeTypeGX.h"

/**
 * Default constructor for the FreeTypeGX class.
 * 
 * @param pointSize	The desired point size this wrapper's configured font face.
 * @param textureFormat	Optional format (GX_TF_*) of the texture as defined by the libogc gx.h header file. If not specified default value is GX_TF_RGBA8.
 * @param cacheAll	Optional flag to specify if all font characters should be cached when the class object is created. If specified as false the characters only become cached the first time they are used. If not specified default value is false.
 */ 

FreeTypeGX::FreeTypeGX(FT_UInt pointSize, uint8_t textureFormat, bool cacheAll) {
	FT_Init_FreeType(&this->ftLibrary);
	FT_New_Memory_Face(this->ftLibrary, (FT_Byte *)fontface, fontsize, 0, &this->ftFace);
	FT_Set_Pixel_Sizes(this->ftFace, 0, pointSize);

	this->ftSlot = this->ftFace->glyph;
	this->textureFormat = textureFormat;
	
	if (cacheAll) {
		this->cacheAllGlyphData();
	}
}

/**
 * Default destructor for the FreeTypeGX class.
 */

FreeTypeGX::~FreeTypeGX() {
	
	for( std::map<wchar_t, fontCharData>::iterator i = this->fontData.begin(); i != this->fontData.end(); i++) {
		free(i->second.glyphDataTexture);
	}
}

/**
 * Adjusts the texture data buffer to necessary width for a given texture format.
 * 
 * This routine determines adjusts the given texture width into the required width to hold the necessary texture data for proper alignment.
 * 
 * @param textureWidth	The initial guess for the texture width.
 * @param textureFormat	The texture format to which the data is to be converted.
 * @return The correctly adjusted texture width.
 */

uint16_t FreeTypeGX::adjustTextureWidth(uint16_t textureWidth, uint8_t textureFormat) {
	uint16_t alignment;
	
	switch(textureFormat) {
		case GX_TF_I4:		/* 8x8 Tiles - 4-bit Intensity */
		case GX_TF_I8:		/* 8x4 Tiles - 8-bit Intensity */
		case GX_TF_IA4:		/* 8x4 Tiles - 4-bit Intensity, , 4-bit Alpha */
			alignment = 8;
			break;

		case GX_TF_IA8:		/* 4x4 Tiles - 8-bit Intensity, 8-bit Alpha */
		case GX_TF_RGB565:	/* 4x4 Tiles - RGB565 Format */
		case GX_TF_RGB5A3:	/* 4x4 Tiles - RGB5A3 Format */
		case GX_TF_RGBA8:	/* 4x4 Tiles - RGBA8 Dual Cache Line Format */
		default:
			alignment = 4;
			break;
	}
	return textureWidth % alignment == 0 ? textureWidth : alignment + textureWidth - (textureWidth % alignment);

}

/**
 * Adjusts the texture data buffer to necessary height for a given texture format.
 * 
 * This routine determines adjusts the given texture height into the required height to hold the necessary texture data for proper alignment.
 * 
 * @param textureHeight	The initial guess for the texture height.
 * @param textureFormat	The texture format to which the data is to be converted.
 * @return The correctly adjusted texture height.
 */

uint16_t FreeTypeGX::adjustTextureHeight(uint16_t textureHeight, uint8_t textureFormat) {
	uint16_t alignment;
	
	switch(textureFormat) {
		case GX_TF_I4:		/* 8x8 Tiles - 4-bit Intensity */
			alignment = 8;
			break;

		case GX_TF_I8:		/* 8x4 Tiles - 8-bit Intensity */
		case GX_TF_IA4:		/* 8x4 Tiles - 4-bit Intensity, , 4-bit Alpha */
		case GX_TF_IA8:		/* 4x4 Tiles - 8-bit Intensity, 8-bit Alpha */
		case GX_TF_RGB565:	/* 4x4 Tiles - RGB565 Format */
		case GX_TF_RGB5A3:	/* 4x4 Tiles - RGB5A3 Format */
		case GX_TF_RGBA8:	/* 4x4 Tiles - RGBA8 Dual Cache Line Format */
		default:
			alignment = 4;
			break;
	}
	return textureHeight % alignment == 0 ? textureHeight : alignment + textureHeight - (textureHeight % alignment);

}

/**
 * Locates each character in this wrapper's configured font face and proccess them.
 *
 * This routine locates each character in the configured font face and renders the glyph's bitmap.
 * Each bitmap and relevant information is loaded into its own quickly addressible structure within an instance-specific map.
 */

uint16_t FreeTypeGX::cacheAllGlyphData() {
	uint16_t i = 0;
	FT_UInt gIndex;
	FT_ULong charCode = FT_Get_First_Char( this->ftFace, &gIndex );
	while ( gIndex != 0 ) {

		if(this->cacheGlyphData(charCode) != NULL) {
			i++;
		}

		charCode = FT_Get_Next_Char( this->ftFace, charCode, &gIndex );
	}
	
	return i;
}

/**
 * Caches the given font glyph in the instance font texture buffer.
 *
 * This routine renders and stores the requested glyph's bitmap and relevant information into its own quickly addressible
 * structure within an instance-specific map.
 * 
 * @param charCode	The requested glyph's character code.
 * @return A pointer to the allocated font structure.
 */

fontCharData *FreeTypeGX::cacheGlyphData(wchar_t charCode) {
	FT_UInt gIndex;
	uint16_t textureWidth = 0, textureHeight = 0;

	gIndex = FT_Get_Char_Index( this->ftFace, charCode );
	if (!FT_Load_Glyph(this->ftFace, gIndex, FT_LOAD_DEFAULT )) {
		FT_Render_Glyph( this->ftSlot, FT_RENDER_MODE_NORMAL );
		
		if(this->ftSlot->format == FT_GLYPH_FORMAT_BITMAP) {
			FT_Bitmap *glyphBitmap = &this->ftSlot->bitmap;

			textureWidth = adjustTextureWidth(glyphBitmap->width, this->textureFormat);
			textureHeight = adjustTextureHeight(glyphBitmap->rows, this->textureFormat);

			this->fontData[charCode] = (fontCharData){
				this->ftSlot->advance.x >> 6,
				textureWidth,
				textureHeight,
				this->ftSlot->bitmap_top,
				this->ftSlot->bitmap_top,
				textureHeight - this->ftSlot->bitmap_top,
				NULL
			};
			this->loadGlyphData(glyphBitmap, &this->fontData[charCode]);
			
			return &this->fontData[charCode];
		}
	}

	return NULL;
}

/**
 * Loads the rendered bitmap into the relevant structure's data buffer.
 * 
 * This routine does a simple byte-wise copy of the glyph's rendered 8-bit grayscale bitmap into the structure's buffer.
 * Each byte is converted from the bitmap's intensity value into the a uint32_t RGBA value.
 * 
 * @param bmp	A pointer to the most recently rendered glyph's bitmap.
 * @param charData	A pointer to an allocated fontCharData structure whose data represent that of the last rendered glyph.
 */

void FreeTypeGX::loadGlyphData(FT_Bitmap *bmp, fontCharData *charData) {
	
	uint32_t *glyphData = (uint32_t *)memalign(32, charData->textureWidth * charData->textureHeight * 4);
	memset(glyphData, 0x00, charData->textureWidth * charData->textureHeight * 4);
	
	for (uint16_t imagePosY = 0; imagePosY < bmp->rows; imagePosY++) {
		for (uint16_t imagePosX = 0; imagePosX < bmp->width; imagePosX++) {
			uint32_t pixel = (uint32_t) bmp->buffer[imagePosY * bmp->width + imagePosX];
			glyphData[imagePosY * charData->textureWidth + imagePosX] = 0x00000000 | (pixel << 24) | (pixel << 16) | (pixel << 8) | pixel;
		}
	}
	
	switch(this->textureFormat) {
		case GX_TF_I4:
			charData->glyphDataTexture = Metaphrasis::convertBufferToI4(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_I8:
			charData->glyphDataTexture = Metaphrasis::convertBufferToI8(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_IA4:
			charData->glyphDataTexture = Metaphrasis::convertBufferToIA4(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_IA8:
			charData->glyphDataTexture = Metaphrasis::convertBufferToIA8(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_RGB565:
			charData->glyphDataTexture = Metaphrasis::convertBufferToRGB565(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_RGB5A3:
			charData->glyphDataTexture = Metaphrasis::convertBufferToRGB5A3(glyphData, charData->textureWidth, charData->textureHeight);
			break;
		case GX_TF_RGBA8:
		default:
			charData->glyphDataTexture = Metaphrasis::convertBufferToRGBA8(glyphData, charData->textureWidth, charData->textureHeight);
			break;
	}
	
	free(glyphData);
}

/**
 * Processes the supplied text string and prints the results at the specified coordinates.
 * 
 * This routine processes each character of the supplied text string, loads the relevant preprocessed bitmap buffer,
 * a texture from said buffer, and loads the resultant texture into the EFB.
 * 
 * @param x	Screen X coordinate at which to output the text.
 * @param y Screen Y coordinate at which to output the text. Note that this value corresponds to the text tring origin and not the top or bottom of the glyphs.
 * @param text	NULL terminated string to output.
 * @param color	Optional color to apply to the text characters. If not specified default value is (GXColor){0xff, 0xff, 0xff, 0xff}
 * @return The number of characters printed.
 */

uint16_t FreeTypeGX::drawText(uint16_t x, uint16_t y, wchar_t *text, GXColor color) {
	uint16_t strLength = wcslen(text);
	uint16_t x_pos = x, printed = 0;
	GXTexObj glyphTexture;
	
	for (uint16_t i = 0; i < strLength; i++) {
		
		fontCharData* glyphData = NULL;
		if( this->fontData.find(text[i]) != this->fontData.end() ) {
			glyphData = &this->fontData[text[i]];
		}
		else {
			glyphData = this->cacheGlyphData(text[i]);
		}
		
		if(glyphData != NULL) {
			GX_InitTexObj(&glyphTexture, glyphData->glyphDataTexture, glyphData->textureWidth, glyphData->textureHeight, this->textureFormat, GX_CLAMP, GX_CLAMP, GX_FALSE);
	
			this->copyTextureToFramebuffer(&glyphTexture, glyphData->textureWidth, glyphData->textureHeight, x_pos, y - glyphData->renderOffsetY, color);
		    x_pos += glyphData->glyphAdvanceX;

		    printed++;
		}
	}
	
	return printed;
}

/**
 * 
 * \overload
 */
uint16_t FreeTypeGX::drawText(uint16_t x, uint16_t y, wchar_t *text) {
	return this->drawText(x, y, text, (GXColor){0xff, 0xff, 0xff, 0xff});
}

/**
 * 
 * \overload
 */
uint16_t FreeTypeGX::drawText(uint16_t x, uint16_t y, wchar_t const *text, GXColor color) {
	return this->drawText(x, y, (wchar_t *)text, color);
}

/**
 * 
 * \overload
 */
uint16_t FreeTypeGX::drawText(uint16_t x, uint16_t y, wchar_t const *text) {
	return this->drawText(x, y, (wchar_t *)text, (GXColor){0xff, 0xff, 0xff, 0xff});
}

/**
 * Processes the supplied string and return the width of the string in pixels.
 * 
 * This routine processes each character of the supplied text string and calculates the width of the entire string.
 * Note that if precaching of the entire font set is not enabled any uncached glyph will be cached after the call to this function.
 * 
 * @param text	NULL terminated string to calculate.
 * @return The width of the text string in pixels.
 */

uint16_t FreeTypeGX::getWidth(wchar_t *text) {
	uint16_t strLength = wcslen(text);
	uint16_t strWidth = 0;
	
	for (uint16_t i = 0; i < strLength; i++) {
		
		fontCharData* glyphData = NULL;
		if( this->fontData.find(text[i]) != this->fontData.end() ) {
			glyphData = &this->fontData[text[i]];
		}
		else {
			glyphData = this->cacheGlyphData(text[i]);
		}
		
		if(glyphData != NULL) {
			strWidth += glyphData->glyphAdvanceX;
		}
	}
	
	return strWidth;
}

/**
 * 
 * \overload
 */
uint16_t FreeTypeGX::getWidth(wchar_t const *text) {
	return this->getWidth((wchar_t *)text);
}

/**
 * Processes the supplied string and return the height of the string in pixels.
 * 
 * This routine processes each character of the supplied text string and calculates the height of the entire string.
 * Note that if precaching of the entire font set is not enabled any uncached glyph will be cached after the call to this function.
 * 
 * @param text	NULL terminated string to calculate.
 * @return The height of the text string in pixels.
 */

uint16_t FreeTypeGX::getHeight(wchar_t *text) {
	uint16_t strLength = wcslen(text);
	uint16_t strMax = 0, strMin = 0;
	
	for (uint16_t i = 0; i < strLength; i++) {
		
		fontCharData* glyphData = NULL;
		if( this->fontData.find(text[i]) != this->fontData.end() ) {
			glyphData = &this->fontData[text[i]];
		}
		else {
			glyphData = this->cacheGlyphData(text[i]);
		}
		
		if(glyphData != NULL) {
			strMax = glyphData->renderOffsetMax > strMax ? glyphData->renderOffsetMax : strMax;
			strMin = glyphData->renderOffsetMin > strMin ? glyphData->renderOffsetMin : strMin;
		}
	}
	
	return strMax + strMin;
}

/**
 * 
 * \overload
 */
uint16_t FreeTypeGX::getHeight(wchar_t const *text) {
	return this->getHeight((wchar_t *)text);
}

/**
 * Copies the supplied texture quad to the EFB. 
 * 
 * This routine uses the in-built GX quad builder functions to define the texture bounds and location on the EFB target.
 * 
 * @param texObj	A pointer to the glyph's initialized texture object.
 * @param texWidth	The pixel width of the texture object.
 * @param texHeight	The pixel height of the texture object.
 * @param screenX	The screen X coordinate at which to output the rendered texture.
 * @param screenY	The screen Y coordinate at which to output the rendered texture
 * @param color	Color to apply to the texture.
 */
void FreeTypeGX::copyTextureToFramebuffer(GXTexObj *texObj, uint16_t texWidth, uint16_t texHeight, uint16_t screenX, uint16_t screenY, GXColor color) {

	f32	f32TexWidth = texWidth,
		f32TexHeight = texHeight;

	Mtx model;
	guMtxIdentity(model);
	guMtxTransApply(model, model, screenX, screenY, 0.0f);
	GX_LoadPosMtxImm(model, GX_PNMTX0);

	GX_LoadTexObj(texObj, GX_TEXMAP0);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position2f32(0, 0);
		GX_Color4u8(color.r, color.g, color.b, color.a);
 		GX_TexCoord2f32(0.0f, 0.0f);

 		GX_Position2f32(f32TexWidth, 0);
		GX_Color4u8(color.r, color.g, color.b, color.a);
		GX_TexCoord2f32(1.0f, 0.0f);

		GX_Position2f32(f32TexWidth, f32TexHeight);
		GX_Color4u8(color.r, color.g, color.b, color.a);
		GX_TexCoord2f32(1.0f, 1.0f);

		GX_Position2f32(0, f32TexHeight);
		GX_Color4u8(color.r, color.g, color.b, color.a);
		GX_TexCoord2f32(0.0f, 1.0f);
	GX_End();
}