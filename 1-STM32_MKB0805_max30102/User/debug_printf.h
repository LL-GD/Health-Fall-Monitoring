#ifndef _debug_printf_H
#define _debug_printf_H

#include "stdint.h"
#include "stdio.h"

#define MB_DEBUG_ENABLED                        (  1 )
#define MB_PERSON_CMDS_ENABLED                  (  0 )

typedef enum
{
	CMD00=0x0001,
	CMD01=0x0002,
	CMD02=0x0004,
	CMD03=0x0008,
	CMD04=0x0010,
	CMD05=0x0020,
	CMD06=0x0040,
	CMD07=0x0080,
    CMD08=0x0100,
    CMD09=0x0200,
    CMD010=0x0400,
    CMD011=0x0800,
    CMD012=0x1000,
    CMD013=0x2000,
    CMD014=0x4000,
    CMD015=0x8000,
    CMDSTOP,	
    CMDADMIN,	
	CMDHELP,
}_Debug_Cmd_Fun_G;

uint8_t usmart_strcmp(const char *str1,char *str2,uint8_t len);
void usmart_strcpy(const char *src, char *dest, uint16_t len);
uint8_t DebugGetDatabuf(uint8_t *debugcmd,uint8_t len);
void debug_printf(uint16_t cmd,char *fmt, ...);
uint8_t UsartGetDatabuf(uint8_t *debugcmd,uint16_t *len);
#endif
