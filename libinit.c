/*
** libinit.c - generic Amiga shared library init code.
** NOTE: This version has been edited specifically for AmIRC Plugins
** Copyright (C) 2003 Robin Cloutman <rycochet@rycochet.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <exec/exec.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "SDI_lib.h"
#include "SDI_stdarg.h"

#include "libinit.h"

/* Must be first function in the library... */
unsigned long LibStart(void)
{
    return(-1);
}

#define WRONG_CPU "\0\16\16This version of " NAME " has been compiled for a different processor!"

/***************************************************************************/

struct ExecBase	*SysBase;


static APTR LIBFUNC InitLib( REG(a0,BPTR seglist), REG(d0,struct myBase *myBase), REG(a6,struct ExecBase *sysbase) )
{
    SysBase = sysbase;
#if   defined (__mc68060__)
    if( !(SysBase->AttnFlags & AFF_68060) )return( NULL );
#elif defined (__mc68040__)
    if( !(SysBase->AttnFlags & AFF_68040) )return( NULL );
#elif defined (__mc68030__)
    if( !(SysBase->AttnFlags & AFF_68030) )return( NULL );
#elif defined (__mc68020__)
    if( !(SysBase->AttnFlags & AFF_68020) )return( NULL );
#endif
    myBase->mb_LibNode.lib_Node.ln_Type	= NT_LIBRARY;
    myBase->mb_LibNode.lib_Node.ln_Pri		= -128;
    myBase->mb_LibNode.lib_Node.ln_Name	= NAME;
    myBase->mb_LibNode.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    myBase->mb_LibNode.lib_Version = VERSION;
    myBase->mb_LibNode.lib_Revision = REVISION;
    myBase->mb_LibNode.lib_IdString = SVER;
    myBase->mb_SegList = seglist;
    //myBase->sysBase = (APTR)SysBase;
    return (APTR)myBase;
}

static ULONG LIBFUNC ExtFuncLib( void )
{
    return( NULL );
}

static BPTR LIBFUNC ExpungeLib( REG(a6,struct myBase *myBase) )
{
    if( myBase->mb_LibNode.lib_OpenCnt )myBase->mb_LibNode.lib_Flags |= LIBF_DELEXP;
    else
    {
        BPTR seglist = myBase->mb_SegList;

        Remove( (struct Node*)myBase );
        FreeMem( (STRPTR)myBase - myBase->mb_LibNode.lib_NegSize, myBase->mb_LibNode.lib_NegSize + myBase->mb_LibNode.lib_PosSize );
        return( seglist );
    }
    return( NULL );
}


static BPTR LIBFUNC CloseLib( REG(a6,struct myBase *myBase) )
{
    if ( !--myBase->mb_LibNode.lib_OpenCnt )
    {
        UserLibClose( myBase );
        if ( myBase->mb_LibNode.lib_Flags & LIBF_DELEXP )return( ExpungeLib(myBase) );
    }
    return( NULL );
}

static APTR LIBFUNC OpenLib( REG(a6,struct myBase *myBase) )
{
    if ( !myBase->mb_LibNode.lib_OpenCnt && !UserLibOpen(myBase) )return( NULL );
    myBase->mb_LibNode.lib_OpenCnt++;
    myBase->mb_LibNode.lib_Flags &= ~LIBF_DELEXP;
    return (APTR)myBase;
}

APTR __FuncTab__[] =
{
    (APTR)OpenLib,
    (APTR)CloseLib,
    (APTR)ExpungeLib,
    (APTR)ExtFuncLib,
    (APTR)AMIPLUG_Setup,
    (APTR)AMIPLUG_Cleanup,
    (APTR)AMIPLUG_Hook_Rawline,
    (APTR)AMIPLUG_Hook_NumericMsg,
    (APTR)AMIPLUG_DoCommand,
    (APTR)AMIPLUG_DoMenu,
    (APTR)AMIPLUG_Hook_DCC,
    (APTR)AMIPLUG_DoCTCP,
    (APTR)AMIPLUG_FinalSetup,
    (APTR)AMIPLUG_Hook_Input,
    (APTR)AMIPLUG_DoRexx,
    (APTR)AMIPLUG_Hook_Prefs,
    (APTR)AMIPLUG_Hook_DCCChat,
    (APTR)-1
};

const ULONG InitTab[] =
{
    sizeof(struct myBase),
    (ULONG)&__FuncTab__[0],
    0,
    (ULONG)InitLib
};

const struct Resident ROMTag =
{
    RTC_MATCHWORD,
    (struct Resident*)&ROMTag,
    __FuncTab__,
    RTF_AUTOINIT,
    VERSION,
    NT_LIBRARY,
    0,
    NAME,
    SVER,
    (APTR)&InitTab
};

