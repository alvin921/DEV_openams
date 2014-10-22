//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      6502 CPU core                                                   //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__CPU_INCLUDED__
#define	__CPU_INCLUDED__

#include "macro.h"
#include "apu.h"
#include "mapper.h"

#if 1 // alvin
#define EMULATOR_ILLEGAL_OP 0

#endif


// 6502 status flags
#define	C_FLAG		0x01		// 1: Carry
#define	Z_FLAG		0x02		// 1: Zero
#define	I_FLAG		0x04		// 1: Irq disabled
#define	D_FLAG		0x08		// 1: Decimal mode flag (NES unused)
#define	B_FLAG		0x10		// 1: Break
#define	R_FLAG		0x20		// 1: Reserved (Always 1)
#define	V_FLAG		0x40		// 1: Overflow
#define	N_FLAG		0x80		// 1: Negative

// Interrupt
#define	NMI_FLAG	0x01
#define	IRQ_FLAG	0x02

#define	IRQ_FRAMEIRQ	0x04
#define	IRQ_DPCM	0x08
#define	IRQ_MAPPER	0x10
#define	IRQ_MAPPER2	0x20
#define	IRQ_TRIGGER	0x40		// one shot(旧IRQ())
#define	IRQ_TRIGGER2	0x80		// one shot(旧IRQ_NotPending())

#define	IRQ_MASK	(~(NMI_FLAG|IRQ_FLAG))

// Vector
#define	NMI_VECTOR	0xFFFA
#define	RES_VECTOR	0xFFFC
#define	IRQ_VECTOR	0xFFFE

// 6502 context
typedef	struct	{
	WORD	PC;	/* Program counter   */
	BYTE	A;	/* CPU registers     */
	BYTE	P;
	BYTE	X;
	BYTE	Y;
	BYTE	S;

	BYTE	INT_pending;	// 割り込みペンディングフラグ
} R6502;

typedef struct CPU_struct CPU;


CPU *	CPU_New( NES* parent );
void	CPU_Destroy(CPU *pme);

INLINE BYTE	CPU_RD6502( CPU *pme, WORD addr );
INLINE void	CPU_WR6502( CPU *pme, WORD addr, BYTE data );
INLINE WORD	CPU_RD6502W( CPU *pme, WORD addr );

void	CPU_Reset(CPU *pme);

void	CPU_NMI(CPU *pme);
void	CPU_SetIRQ( CPU *pme, BYTE mask );
void	CPU_ClrIRQ( CPU *pme, BYTE mask );

void	CPU_DMA( CPU *pme, INT cycles );

INT 	CPU_EXEC( CPU *pme, INT request_cycles );

INT 	CPU_GetDmaCycles(CPU *pme);
void	CPU_SetDmaCycles( CPU *pme, INT cycles );

INT		CPU_GetTotalCycles(CPU *pme);
void	CPU_SetTotalCycles( CPU *pme, INT cycles );

void	CPU_SetContext( CPU *pme, R6502 *r );
void	CPU_GetContext( CPU *pme, R6502 *r );

void	CPU_SetClockProcess( CPU *pme, BOOL bEnable );

#endif	// !__CPU_INCLUDED__
