#include "core/GraphicsSystem.h"
#include "core/PadSystem.h"
#include "core/VideoSystem.h"

#include <FreeTypeGX.h>
#include "rursus_compact_mono_ttf.h"	// Include the compiled font.
										// Once compiled you can view the contents of this file in example1/all/rursus_compact_mono_ttf.h

int main(int argc, char **argv) {
	VideoSystem* videoSystem = new VideoSystem();
	GraphicsSystem *graphicsSystem = new GraphicsSystem(videoSystem);
	PadSystem *padSystem = new PadSystem();

	FreeTypeGX *fontSystem = new FreeTypeGX(GX_TF_IA8);
	FT_UInt fontSize = 64;
	fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, fontSize, false);	// Initialize the font system with the font parameters from rursus_compact_mono_ttf.h

	uint32_t buttons = 0x0000;
	bool isUnderlined = false;
	bool isStrike = false;

	while(!padSystem->pressedExitButton(buttons)) {

		uint32_t textStyle = FTGX_JUSTIFY_CENTER;
		if(isUnderlined) {
			textStyle |= FTGX_STYLE_UNDERLINE;
		}
		if(isStrike) {
			textStyle |= FTGX_STYLE_STRIKE;
		}

		fontSystem->drawText(320,	50,		_TEXT("THE QUICK BROWN"),	(GXColor){0xff, 0x00, 0x00, 0xff},	textStyle | FTGX_ALIGN_TOP);
		fontSystem->drawText(320,	125,	_TEXT("FOX JUMPS OVER"),	(GXColor){0x00, 0xff, 0x00, 0xff},	textStyle | FTGX_ALIGN_MIDDLE);
		fontSystem->drawText(320,	200,	_TEXT("THE LAZY DOG"),		(GXColor){0x00, 0x00, 0xff, 0xff},	textStyle | FTGX_ALIGN_BOTTOM);

		fontSystem->drawText(320,	275,	_TEXT("the quick brown"),	(GXColor){0xff, 0xff, 0x00, 0xff},	textStyle);
		fontSystem->drawText(320,	350,	_TEXT("fox jumps over"),	(GXColor){0xff, 0x00, 0xff, 0xff},	textStyle);
		fontSystem->drawText(320,	425,	_TEXT("the lazy dog"),		(GXColor){0x00, 0xff, 0xff, 0xff},	textStyle);

		graphicsSystem->updateScene(videoSystem->getVideoFramebuffer());
		videoSystem->flipVideoFramebuffer();

		buttons = padSystem->scanPads(0);
		if(padSystem->pressedUp(buttons)) {
			fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, ++fontSize, false);
		}
		if(padSystem->pressedDown(buttons)) {
			fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, fontSize > 6 ? --fontSize : fontSize, false);
		}
		if(padSystem->pressedLeft(buttons)) {
			isUnderlined = !isUnderlined;
		}
		if(padSystem->pressedRight(buttons)) {
			isStrike = !isStrike;
		}
	}

	delete fontSystem;
	delete padSystem;
	delete graphicsSystem;
	delete videoSystem;
	
	return 0;
}

