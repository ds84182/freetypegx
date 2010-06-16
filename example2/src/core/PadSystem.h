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
		u32 scanPads(int controller);
		bool pressedExitButton(u32 buttons);
};
#endif /*PADSYSTEM_H_*/
