#include "g_local.h"

/* workaround for Lua, Linux only */

unsigned short Sys_GetFPUStatus (void)
{
	unsigned short fpuword;
	__asm__ __volatile__ ("fnstcw %0" : "=m" (fpuword));
	return fpuword;
}

/*
 * Round to zero, 24 bit precision
 */
void Sys_SetFPU (void)
{
	unsigned short fpuword;
	fpuword = Sys_GetFPUStatus ();
	fpuword &= ~(3 << 8);
	fpuword |= (0 << 8);
	fpuword &= ~(3 << 10);
	fpuword |= (0 << 10);
	__asm__ __volatile__ ("fldcw %0" : : "m" (fpuword));
}

void Sys_ResetFPU (void)
{
	unsigned short fpuword = 0x37F; /* magic number \o/ */
	__asm__ __volatile__ ("fldcw %0" : : "m" (fpuword));
}
