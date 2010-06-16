#include "core/GraphicsSystem.h"
#include "core/PadSystem.h"
#include "core/VideoSystem.h"

#include <FreeTypeGX.h>

extern uint8_t rursus_compact_mono_ttf[]; 		// Font data pointer to the compiled buffer within the distributable (defined in example2.s)
extern FT_Long rursus_compact_mono_ttf_size;    // Size of the TrueType file in bytes (defined in example2.s)

int main(int argc, char **argv) {
	VideoSystem* videoSystem = new VideoSystem();
	GraphicsSystem *graphicsSystem = new GraphicsSystem(videoSystem);
	PadSystem *padSystem = new PadSystem();

	FreeTypeGX *fontSystem = new FreeTypeGX(GX_TF_IA8);
	fontSystem->loadFont(rursus_compact_mono_ttf, rursus_compact_mono_ttf_size, 64, false);	// Initialize the font system with the font parameters linked above

	while(!padSystem->pressedExitButton(padSystem->scanPads(0))) {

		fontSystem->drawText(320,	50,		_TEXT("THE QUICK BROWN"),	(GXColor){0xff, 0x00, 0x00, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_ALIGN_TOP);
		fontSystem->drawText(320,	125,	_TEXT("FOX JUMPS OVER"),	(GXColor){0x00, 0xff, 0x00, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
		fontSystem->drawText(320,	200,	_TEXT("THE LAZY DOG"),		(GXColor){0x00, 0x00, 0xff, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_ALIGN_BOTTOM);

		fontSystem->drawText(320,	275,	_TEXT("the quick brown"),	(GXColor){0xff, 0xff, 0x00, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_STYLE_UNDERLINE);
		fontSystem->drawText(320,	350,	_TEXT("fox jumps over"),	(GXColor){0xff, 0x00, 0xff, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_STYLE_UNDERLINE);
		fontSystem->drawText(320,	425,	_TEXT("the lazy dog"),		(GXColor){0x00, 0xff, 0xff, 0xff},	FTGX_JUSTIFY_CENTER | FTGX_STYLE_UNDERLINE);

		graphicsSystem->updateScene(videoSystem->getVideoFramebuffer());
		videoSystem->flipVideoFramebuffer();
	}
	while(!padSystem->pressedExitButton(padSystem->scanPads(0))) {}

	delete fontSystem;
	delete padSystem;
	delete graphicsSystem;
	delete videoSystem;
	
	return 0;
}

