#include "g_local.h"

/* x86 workaround for Lua, fsck you Carmack! */

void Sys_SetFPU (void)
{
#ifdef __i386__
	unsigned short fpuword;
	__asm__ __volatile__ ("fnstcw %0" : "=m" (fpuword));
	fpuword &= ~(3 << 8);
	fpuword |= (0 << 8);
	fpuword &= ~(3 << 10);
	fpuword |= (0 << 10);
	__asm__ __volatile__ ("fldcw %0" : : "m" (fpuword));
#endif
}

void Sys_ResetFPU (void)
{
#ifdef __i386__
	unsigned short fpuword = 0x37F; /* magic number \o/ */
	__asm__ __volatile__ ("fldcw %0" : : "m" (fpuword));
#endif
}
