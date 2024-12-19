/*
** Codesets.c - Codesets plugin for AmIRC.
** Copyright (C) 2024 Joni Valtanen <jvaltane@kapsi.fi>
*
** Based on Typo.c - Typo plugin for AmIRC.
** Copyright (C) 2002-2003 Robin Cloutman <rycochet@rycochet.com>
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
**
****************************************************************************
*/

#include <exec/types.h>
#include <dos/dos.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/codesets.h>
#include <SDI_compiler.h>
#include "libinit.h"

#define COPYRIGHT2 "Copyright ©" COPYRIGHT "\nGNU General Public License V2"
#define INFO "Simple UTF-8 support using codesets.library"
#define SETUPNAME "Codesets"

/* #define DEBUG */

/***************************************************************************/

extern struct ExecBase *SysBase;

struct Library *CodesetsBase = NULL;
#ifdef DEBUG
struct DosLibrary *DOSBase = NULL;
#endif
struct PrivateData
{
    struct codeset *amigaCodeset;
    struct codeset *serverCodeset;
    STRPTR buffer;
    BOOL inUse;
};

BOOL UserLibOpen( struct myBase *myBase )
{
#ifdef DEBUG
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37);
    if ( !DOSBase ) return FALSE;
#endif
    CodesetsBase = (struct Library *)OpenLibrary(CODESETSNAME, CODESETSVER);
    if ( !CodesetsBase ) return FALSE;
    return TRUE;
}

void UserLibClose( struct myBase *myBase )
{
    if (CodesetsBase) CloseLibrary( (struct Library*)CodesetsBase );
#ifdef DEBUG
    if (DOSBase) CloseLibrary( (struct Library*)DOSBase );
#endif
}

/***************************************************************************/
static inline char lower( const char c ) { return(c>='A'&&c<='Z'?c+'a'-'A':c); }

int strlen( const char *a )
{
    int n = 0;
    while( *a++ )n++;
    return(n);
}

char *strcpy( char *a, const char *b )
{
    while( ( *a++ = *b++ ) );
    return(--a);
}

/* be carefull with this!!! */
int strnicmp( const char *a, const char *b, ULONG len )
{
    ULONG pos = 0;
    while( pos < len && *a && *b && lower(*a)==lower(*b) ) { a++,b++,pos++; };
    if (pos == (len - 1)) return 0; /* match */
    return 1; /* no match */
}

/***************************************************************************/

struct amiplug_cmd mycmd = {{NULL,NULL},"UTF8","[ON/OFF]", 1, 0, FALSE};

struct TagItem mytaglist[] =
{
    { AMIPLUG_Query_MinAmIRCVersion, 0x00030000 },
    { AMIPLUG_Query_Version, VERSION },
    { AMIPLUG_Query_Revision, REVISION },
    { AMIPLUG_Query_Copyright, (ULONG)COPYRIGHT2 },
    { AMIPLUG_Query_Infostring, (ULONG)INFO },

    { AMIPLUG_Query_CustomCommand, (ULONG)&mycmd },

    { AMIPLUG_Query_Hook_Input, TRUE },
    { AMIPLUG_Query_Hook_Rawline, TRUE },
    { AMIPLUG_Query_Hook_Rawline2, TRUE }, /* This line probably is not needed */
    { AMIPLUG_Query_Hook_DCCChat, TRUE },
    { TAG_DONE }
};

/* Setup & Query function */
struct TagItem *AMIPLUG_Setup( REG(a0,struct amiplug_functable *ctx) )
{
    struct PrivateData *data = ctx->amiplug_malloc( ctx, sizeof(struct PrivateData) );;
    ctx->userdata = NULL;
    if (data) {
        data->amigaCodeset = CodesetsFindA(NULL, NULL);
        data->serverCodeset = CodesetsFind("UTF-8", CSA_FallbackToDefault, FALSE, TAG_DONE);
        data->buffer = NULL;
        data->inUse = TRUE;
        ctx->userdata = data;
    }
    return( mytaglist );
}

void AMIPLUG_Cleanup( REG(a0,struct amiplug_functable *ctx) )
{
    struct PrivateData *data = (struct PrivateData *)ctx->userdata;
    if (data) {
        if (data->buffer) {
            CodesetsFreeA(data->buffer, NULL);
        }
        ctx->amiplug_free (data);
    }
    ctx->userdata = NULL;
}

int AMIPLUG_Hook_Rawline( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR line), REG(d0,ULONG len) )
{
    STRPTR dst;
    struct PrivateData *data = (struct PrivateData *)ctx->userdata;
    if (!(data && data->inUse)) return 0;
    if (!(data->amigaCodeset && data->serverCodeset)) {
        ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«Error»", "Codesets: Invalid codeset to client or server." );
        return 0;
    }

    dst = CodesetsConvertStr (CSA_SourceCodeset, data->serverCodeset,
                                CSA_DestCodeset, data->amigaCodeset,
                                CSA_Source, line,
                                CSA_SourceLen, len,
                                TAG_DONE);
    if (dst) {
#ifdef DEBUG
        PutStr("Rawline:'"); PutStr(dst); PutStr("'\n");
#endif
        /* This is kind of dirty trick. Assuption is that UTF8 is always
           longer or same lenght than one-byte per char. -> Just
           overwrite current line
        */
        (void)strcpy (line, dst); /* error handling: no */
        CodesetsFreeA(dst, NULL);
    }
    return 0;
}

int AMIPLUG_Hook_NumericMsg( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR line), REG(d0,ULONG numericcode), REG(a2,STRPTR prefix), REG(a3,STRPTR dest), REG(d1,ULONG numargs), REG(a4,STRPTR *args) )
{
    return 0 ;
}

void AMIPLUG_DoCommand( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,struct amiplug_cmdparm *parms) )
{
    struct PrivateData *data = (struct PrivateData *)ctx->userdata;
    if (!data) return;

    if (parms->parms) {
        if (strnicmp("ON", parms->parms, 3) == 0) {
            data->inUse = TRUE;
        } else if (strnicmp("OFF", parms->parms, 4) == 0) {
            data->inUse = FALSE;
        } else {
            ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«Error»", "Usage: /UTF8 [ON/OFF]" );
            return;
        }
    }
    if (data->inUse) ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«UTF8»", "Is ON" );
    else ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«UTF8»", "Is OFF" );
}

void AMIPLUG_DoMenu( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,APTR muiapp) )
{
}

void AMIPLUG_Hook_DCC( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG dccid) )
{
}

void AMIPLUG_DoCTCP( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,STRPTR from), REG(a2,STRPTR rest), REG(a3,STRPTR to) )
{
}

void AMIPLUG_FinalSetup( REG(a0,struct amiplug_functable *ctx) )
{
}


/*
 * Inclide ome commands from input conversion
 */
static int input_include (const char *str)
{
    if (strlen(str) < 2) return 1;

    /* NULL-terminations are checked in purpose */
    if (strnicmp("/WHO ", str, 6) == 0) return 0;
    else if (strnicmp("/AWAY ", str, 7) == 0) return 0;
    else if (strnicmp("/OPER ", str, 7) == 0) return 0;
    else if (strnicmp("/PART ", str, 7) == 0) return 0;
    else if (strnicmp("/PASS ", str, 7) == 0) return 0;
    else if (strnicmp("/QUIT ", str, 7) == 0) return 0;
    else if (strnicmp("/USER ", str, 7) == 0) return 0;
    else if (strnicmp("/TOPIC ", str, 8) == 0) return 0;
    else if (strnicmp("/SETNAME ", str, 10) == 0) return 0;
    else if (strnicmp("/WALLOPS ", str, 10) == 0) return 0;
    return 1;
}

char *AMIPLUG_Hook_Input( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR string), REG(d0,ULONG len) )
{
    struct PrivateData *data = (struct PrivateData *)ctx->userdata;
    if (!(data && data->inUse)) return string;
    if (!(data->amigaCodeset && data->serverCodeset)) {
        ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«Error»", "Codesets: Invalid codeset to client or server." );
        return string;
    }

#ifdef DEBUG
    PutStr("Input:'"); PutStr(string); PutStr("'\n");
#endif
    if (input_include(string) == 0) {
#ifdef DEBUG
        PutStr("Convert\n");
#endif
        if (data->buffer) {
            CodesetsFreeA(data->buffer, NULL);
        }
        data->buffer = CodesetsConvertStr (CSA_SourceCodeset, data->amigaCodeset,
                                          CSA_DestCodeset, data->serverCodeset,
                                          CSA_Source, string,
                                          CSA_SourceLen, len,
                                          TAG_DONE);
        return (char *)data->buffer;
    }
    return (char *)string;
}

int AMIPLUG_DoRexx( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,STRPTR *args), REG(a2,APTR muiapp) )
{
    return 0;
}

void AMIPLUG_Hook_Prefs( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG methodid), REG(a1,struct amiplug_prefs *prefs) )
{
}

int AMIPLUG_Hook_DCCChat( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG dccid), REG(a1,STRPTR nick), REG(a2,STRPTR line), REG(d1,ULONG len) )
{
    STRPTR dst;
    struct PrivateData *data = (struct PrivateData *)ctx->userdata;
    if (!(data && data->inUse)) return 0;
    if (!(data->amigaCodeset && data->serverCodeset)) {
        ctx->amiplug_out_defwin( ctx, amirc_tc_local, "\eb«Error»", "Codesets: Invalid codeset to client or server." );
        return 0;
    }

    dst = CodesetsConvertStr (CSA_SourceCodeset, data->serverCodeset,
                                CSA_DestCodeset, data->amigaCodeset,
                                CSA_Source, line,
                                CSA_SourceLen, len,
                                TAG_DONE);
    if (dst) {
#ifdef DEBUG
        PutStr("DCC:'"); PutStr(dst); PutStr("'\n");
#endif
        /* This is kind of dirty trick. Assuption is that UTF8 is always
           longer or same lenght than one-byte per char. -> Just
           overwrite current line
        */
        (void)strcpy (line, dst); /* error handling: no */
        CodesetsFreeA(dst, NULL);
    }
    return 0;
}

