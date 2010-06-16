#ifndef PADSYSTEM_H_
#define PADSYSTEM_H_

#include <stdlib.h>
#include <gccore.h>

#ifdef __WIISYSTEM__
#include <wiiuse/wpad.h> 
#endif

class PadSystem {
	private:
		void initializePadSystem();

	public:
		PadSystem();
		uint32_t scanPads(int controller);
		bool pressedExitButton(uint32_t buttons);
};
#endif /*PADSYSTEM_H_*/
