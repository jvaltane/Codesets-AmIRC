#ifndef LIB_H
#define LIB_H
/*
** libinit.h - generic Amiga shared library header code.
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

#define	VERSION		1
#define	REVISION		0
#define	BETA			0
#define	DATE			"14.12.2024"
#define	NAME			"Codesets.AmIPlug"
#define	AUTHOR		"Joni Valtanen <jvaltane@kapsi.fi>"

#if	defined(__mc68060__)
#define	CPUSTR	"[060]"
#elif	defined(__mc68040__)
#define	CPUSTR	"[040]"
#elif	defined(__mc68030__)
#define	CPUSTR	"[030]"
#elif	defined(__mc68020__)
#define	CPUSTR	"[020]"
#else
#define	CPUSTR	""
#endif

#define	VERS			"Codesets.AmIPlug" CPUSTR " 1.0"
#define	SHORTVERS	"1.0"
#define	VSTRING		VERS " (11.12.24)\r\n"
#define	VERSTAG		"\0$VER: " VERS " (" DATE ")"
#define	COPYRIGHT	"2024 Joni Valtanen <jvaltane@kapsi.fi>"
#define	SVER			"$VER: " VERS " (" DATE ") ©" COPYRIGHT

/* Any includes you want to make... */
#include	"amirc_plugin.h"

/* The base structure for our library */
struct myBase
{
	struct Library	mb_LibNode;
	BPTR				mb_SegList;

	/* Add your own entries after here */
};

/* These are called on first open/last close of the library */
BOOL UserLibOpen( struct myBase *myBase );
void UserLibClose( struct myBase *myBase );

#endif	/* LIB_H */
