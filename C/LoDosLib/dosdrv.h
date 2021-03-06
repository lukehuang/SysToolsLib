/*****************************************************************************\
*									      *
*   File name:	    dosdrv.h						      *
*									      *
*   Description:    Generic MS-DOS device drivers definitions		      *
*									      *
*   Notes:								      *
*									      *
*   History:								      *
*    1995-08-22 JFL Created this file					      *
*    2017-12-19 JFL Added workaround for warnings when running h2inc.exe.     *
*									      *
*      (c) Copyright 1995-2017 Hewlett Packard Enterprise Development LP      *
* Licensed under the Apache 2.0 license - www.apache.org/licenses/LICENSE-2.0 *
\*****************************************************************************/

#define FALSE 0
#define TRUE 1

/* Define assembler register types BYTE, WORD, DWORD for C */
/* Note: H2INC.EXE outputs three HI4010 warnings: "identifier is a MASM keyword"
	 when defining these with a typedef.
         I tried preventing that warning, using #ifndef _H2INC, or with info
         from https://msdn.microsoft.com/en-us/library/aa712931.aspx
         or with pragmas, but found no solution that worked.
         So instead, use macros _BYTE, _WORD, _DWORD, and define them
         differently for C (using a typedef) and for H2INC (without that) */
#ifndef _H2INC	/* Compiling in C or C++ programs */
typedef unsigned char	BYTE;	/* 8 bits unsigned character */
typedef unsigned short  WORD;   /* 16-bit unsigned value */
typedef unsigned long   DWORD;  /* 32-bit unsigned integer */
#define _BYTE  BYTE
#define _WORD  WORD
#define _DWORD DWORD
#else		/* Converting to .inc with h2inc.exe */
#define _BYTE unsigned char
#define _WORD unsigned short
#define _DWORD unsigned long
#endif

/*****************************************************************************/
/*		     Definitions generic to all device drivers		     */
/*****************************************************************************/

#pragma pack(1) 	    /* All DOS structures are packed */

typedef struct tagDeviceHeader		/* Device driver header */
    {
    struct tagDeviceHeader far *dhLink;     /* Link to the next driver */
    _WORD dhAttributes;			    /* Device attributes */
    void (near *dhStrategy)();		    /* Strategy-routine offset */
    void (near *dhInterrupt)(); 	    /* Interrupt-routine offset */
    char dhNameOrUnits[8];		    /* Logical device name */
    } DeviceHeader;

typedef struct		    /* Standard request header */
    {
    _BYTE rhLength;		/* Length in bytes of this request header */
    _BYTE rhUnit;		/* Subunit for block device drivers */
    _BYTE rhFunction;		/* Command code */
    _WORD rhStatus;		/* Status word returned by the driver */
    _BYTE rhReserved[8]; 	/* reserved */
    } RequestHeader;

typedef struct		    /* Input/output commands request */
    {
    RequestHeader h;
    _BYTE rwrMediaID;		/* Media descriptor from BPB */
    char far *rwrBuffer;	/* Transfer address */
    _WORD rwrBytesSec;		/* Bytes / sectors count */
    _WORD rwrStartSec;		/* Starting sector */
    char far *rwrVolumeID;	/* Returned pointer on volume ID */
    _DWORD rwrHugeStartSec;	/* Starting sector if rwrStartSec=FFFF */
    } ReadWriteRequest;

typedef struct		    /* Non Destructive Read (Peek) command request */
    {
    RequestHeader h;
    char nrrChar;		/* Character read from device */
    } NDReadRequest;

#define SUCCESS 0x0000
#define BUSY	0x0200
#define ERROR	0x8000

#pragma pack()		    /* Return to default packing */


/* Data prototypes */

extern DeviceHeader deviceHeader;
#ifdef _DEBUG
extern int DebugPort;                   /* Debug port base address */
#endif

/* Function prototypes */

extern int _cdecl Read(ReadWriteRequest far *fpr);
extern int _cdecl Write(ReadWriteRequest far *fpr);
extern int _cdecl Peek(NDReadRequest far *fpr);
extern int _cdecl WStatus(RequestHeader far *fpr);
extern int _cdecl RStatus(RequestHeader far *fpr);
