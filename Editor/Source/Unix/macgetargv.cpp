/***********************************************************
Copyright 1991-1997 by Stichting Mathematisch Centrum, Amsterdam,
The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.

STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/* Construct argc and argv for main() by using Apple Events */
/* From Jack's implementation for STDWIN */

/* Modified for use with FreeType from MacPython's macgetargv.c
   by Just van Rossum <just@letterror.com> */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "macgetargv.h"

#ifndef USING_CARBON
#include <Types.h>
#include <Files.h>
#include <Events.h>
#include <Processes.h>
#include <Errors.h>
#include <AppleEvents.h>
#include <AEObjects.h>
#else
#include <Carbon.h>
#endif

#if defined(GENERATINGCFM) && !defined(USING_CARBON)	/* Defined to 0 or 1 in Universal headers */
#define HAVE_UNIVERSAL_HEADERS
#endif

#if !defined(HAVE_UNIVERSAL_HEADERS) && !defined(USING_CARBON)
#define NewAEEventHandlerProc(x) (x)
#define AEEventHandlerUPP EventHandlerProcPtr
#endif

static int arg_count;
static char *arg_vector[256];
static char app_name[256];
static int app_name_inited;


/* Duplicate a string to the heap. */

static char *
strdup(char *src)
{
	char *dst = (char *)malloc(strlen(src) + 1);
	if (dst)
		strcpy(dst, src);
	return dst;
}


/* Given an FSSpec, return the FSSpec of the parent folder */

static OSErr
get_folder_parent (FSSpec * fss, FSSpec * parent)
{
	CInfoPBRec rec;
	short err;

        * parent = * fss;
        rec.hFileInfo.ioNamePtr = parent->name;
        rec.hFileInfo.ioVRefNum = parent->vRefNum;
        rec.hFileInfo.ioDirID = parent->parID;
		rec.hFileInfo.ioFDirIndex = -1;
        rec.hFileInfo.ioFVersNum = 0;
        err = PBGetCatInfoSync (& rec);
        if (err)
        	return err;
        parent->parID = rec.dirInfo.ioDrParID;
/*	parent->name[0] = 0; */
        return 0;
}


/* Given an FSSpec return a full, colon-separated pathname */

static OSErr
FTMac_GetFullPath (FSSpec *fss, char *buf)
{
	short err;
	FSSpec fss_parent, fss_current;
	char tmpbuf[256];
	int plen;

	fss_current = *fss;
	plen = fss_current.name[0];
	memcpy(buf, &fss_current.name[1], plen);
	buf[plen] = 0;
	/* Special case for disk names */
	if ( fss_current.parID <= 1 ) {
		buf[plen++] = ':';
		buf[plen] = 0;
		return 0;
	}
	while (fss_current.parID > 1) {
    		/* Get parent folder name */
                err = get_folder_parent(&fss_current, &fss_parent);
                if (err)
             		return err;
                fss_current = fss_parent;
                /* Prepend path component just found to buf */
    			plen = fss_current.name[0];
    			if (strlen(buf) + plen + 1 > 256) {
    				/* Oops... Not enough space (shouldn't happen) */
    				*buf = 0;
    				return -1;
    			}
    			memcpy(tmpbuf, &fss_current.name[1], plen);
    			tmpbuf[plen] = ':';
    			strcpy(&tmpbuf[plen+1], buf);
    			strcpy(buf, tmpbuf);
        }
        return 0;
}


/* Initialize name of current application */

static OSErr
init_app_name( unsigned int psn )
{
	ProcessSerialNumber currentPSN;
	ProcessInfoRec info;
	OSErr err;
	FSSpec appSpec;

	if ( app_name_inited ) return 0;


        err = GetCurrentProcess( &currentPSN );
        if (err)
        {
                std::cerr << "init_app_name GetCurrentProcess() err " << err << std::endl;
		return err;
        }

        std::cerr << "init_app_name GetCurrentProcess() psn " << currentPSN.highLongOfPSN << " " << currentPSN.lowLongOfPSN << std::endl;

	//currentPSN.highLongOfPSN = 0;
	//currentPSN.lowLongOfPSN = kCurrentProcess;
	//currentPSN.lowLongOfPSN = psn;
	info.processInfoLength = sizeof(ProcessInfoRec);
	info.processName = NULL;
	info.processAppSpec = &appSpec;
        std::cerr << "init_app_name before GetProcessInformation() kCurrentProcess " << kCurrentProcess << std::endl;
	err=GetProcessInformation(&currentPSN, &info);
        if (err)
		return err;
	strncpy(app_name, (char*)appSpec.name + 1, appSpec.name[0]);
	app_name[appSpec.name[0]] = '\0';
	app_name_inited = 1;
	return 0;
}


/* Check that there aren't any args remaining in the event */

static OSErr
get_missing_params(const AppleEvent *theAppleEvent)
{
	DescType theType;
	Size actualSize;
	OSErr err;

	err = AEGetAttributePtr(theAppleEvent, keyMissedKeywordAttr, typeWildCard,
				&theType, nil, 0, &actualSize);
	if (err == errAEDescNotFound)
		return noErr;
	else
		return errAEEventNotHandled;
}

static int got_one; /* Flag that we can stop getting events */


/* Handle the Print or Quit events (by failing) */

static pascal OSErr
handle_not(const AppleEvent *theAppleEvent, AppleEvent *reply, long refCon)
{
	#pragma unused (reply, refCon)
	got_one = 1;
	return errAEEventNotHandled;
}


/* Handle the Open Application event (by ignoring it) */

static pascal OSErr
handle_open_app(const AppleEvent *theAppleEvent, AppleEvent *reply, long refCon)
{
	#pragma unused (reply, refCon)
#if 0
	/* Test by Jack: would removing this facilitate debugging? */
	got_one = 1;
#endif
	return get_missing_params(theAppleEvent);
}


/* Handle the Open Document event, by adding an argument */

static pascal OSErr
handle_open_doc(const AppleEvent *theAppleEvent, AppleEvent *reply, long refCon)
{
	#pragma unused (reply, refCon)
	OSErr err;
	AEDescList doclist;
	AEKeyword keywd;
	DescType rttype;
	long i, ndocs, size;
	FSSpec fss;
	char path[256];

	got_one = 1;
	err = AEGetParamDesc(theAppleEvent,
				 keyDirectObject, typeAEList, &doclist);
        if (err)
		return err;
	err = get_missing_params(theAppleEvent);
        if (err)
		return err;
	err = AECountItems(&doclist, &ndocs);
        if (err)
		return err;
	for(i = 1; i <= ndocs; i++) {
		err = AEGetNthPtr(&doclist, i, typeFSS,
				  &keywd, &rttype, &fss, sizeof(fss), &size);
		if (err)
			break;
		FTMac_GetFullPath(&fss, path);
		arg_vector[arg_count++] = strdup(path);
	}
	return err;
}


/* Install standard core event handlers */
static AEEventHandlerUPP open_doc_upp;
static AEEventHandlerUPP open_app_upp;
static AEEventHandlerUPP not_upp;

static void
set_ae_handlers()
{
#ifdef USING_CARBON
	open_doc_upp = NewAEEventHandlerUPP(handle_open_doc);
	open_app_upp = NewAEEventHandlerUPP(handle_open_app);
	not_upp = NewAEEventHandlerUPP(handle_not);
#else
	open_doc_upp = NewAEEventHandlerProc(handle_open_doc);
	open_app_upp = NewAEEventHandlerProc(handle_open_app);
	not_upp = NewAEEventHandlerProc(handle_not);
#endif

	AEInstallEventHandler(kCoreEventClass, kAEOpenApplication,
			      open_app_upp, 0L, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
			      open_doc_upp, 0L, false);
	AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
			      not_upp, 0L, false);
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
			      not_upp, 0L, false);
}

/* Uninstall standard core event handlers */

static void
reset_ae_handlers()
{
	AERemoveEventHandler(kCoreEventClass, kAEOpenApplication,
			     open_app_upp, false);
	AERemoveEventHandler(kCoreEventClass, kAEOpenDocuments,
			     open_doc_upp, false);
	AERemoveEventHandler(kCoreEventClass, kAEPrintDocuments,
			     not_upp, false);
	AERemoveEventHandler(kCoreEventClass, kAEQuitApplication,
			     not_upp, false);
}

/* Wait for events until a core event has been handled */

static void
event_loop()
{
	EventRecord event;
	int n;
	int ok;

	got_one = 0;
	for (n = 0; n < 100 && !got_one; n++) {
	#ifndef USING_CARBON
		SystemTask();
	#endif
		ok = GetNextEvent(everyEvent, &event);
		if (ok && event.what == kHighLevelEvent) {
			AEProcessAppleEvent(&event);
		}
	}
}

/* Get the argv vector, return argc. See macgetargv.h for more doco. */


int
FTMac_GetArgv( char ***pargv, char *psn_arg )
{
        unsigned int psn = strtol( &psn_arg[sizeof("-psn_0_")-1], NULL, 10 );

        std::cerr << "FTMac_GetArgv before InstallStandardEventHandler()" << std::endl;
        InstallStandardEventHandler( GetApplicationEventTarget() ) ;

	arg_count = 0;
        std::cerr << "FTMac_GetArgv before init_app_name( " << psn << " )" << std::endl;
	init_app_name( psn );
	arg_vector[arg_count++] = strdup(app_name);
	
        std::cerr << "FTMac_GetArgv before set_ae_handlers()" << std::endl;
	set_ae_handlers();
        std::cerr << "FTMac_GetArgv before event_loop()" << std::endl;
	event_loop();
        std::cerr << "FTMac_GetArgv before reset_ae_handlers()" << std::endl;
	reset_ae_handlers();
	
        std::cerr << "FTMac_GetArgv done()" << std::endl;
	arg_vector[arg_count] = NULL;
	
	*pargv = arg_vector;
	return arg_count;
}
