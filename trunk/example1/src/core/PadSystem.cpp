#include "PadSystem.h"

PadSystem::PadSystem() {
	this->initializePadSystem();
}

void PadSystem::initializePadSystem() {
#ifdef __WIISYSTEM__
	WPAD_Init();
#else
	PAD_Init();
#endif
}

uint32_t PadSystem::scanPads(int controller) {
#ifdef __WIISYSTEM__
	WPAD_ScanPads();
	return WPAD_ButtonsDown(controller);
#else
	PAD_ScanPads();
	return PAD_ButtonsDown(controller);
#endif
}

bool PadSystem::pressedExitButton(uint32_t buttonsDown) {
#ifdef __WIISYSTEM__
	if (buttonsDown & WPAD_BUTTON_HOME) {
		return true;
	}
#else
	if (buttonsDown & PAD_BUTTON_START) {
		return true;
	}
#endif

	return false;
}
