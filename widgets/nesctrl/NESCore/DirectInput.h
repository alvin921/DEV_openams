//
// DirectInput class
//
#ifndef	__DIRECTINPUT_INCLUDED__
#define	__DIRECTINPUT_INCLUDED__

#include "macro.h"

#define NES_KEY_A		0x01			// 0
#define NES_KEY_B		0x02			// 1
#define NES_KEY_SELECT	0x04			// 2
#define NES_KEY_START	0x08			// 3
#define NES_KEY_UP		0x10			// 4
#define NES_KEY_DOWN	0x20			// 5
#define NES_KEY_LEFT	0x40			// 6
#define NES_KEY_RIGHT	0x80			// 7
#define NES_KEY_A_RAPID	0x100		// 8
#define NES_KEY_B_RAPID	0x200		// 9

#define NES_KEY_PRIOR	0x400
#define NES_KEY_NEXT	0x800

void	DirectInput_Initial(void);
void	DirectInput_Poll(void);
WORD	DirectInput_GetBits(void);


#endif // !__DIRECTINPUT_INCLUDED__
