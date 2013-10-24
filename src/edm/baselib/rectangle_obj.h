//  edm - extensible display manager

//  Copyright (C) 1999 John W. Sinclair

//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef __rectangle_obj_h
#define __rectangle_obj_h 1

#include "act_grf.h"
#include "entry_form.h"
#include "app_pkg.h"
#include "act_win.h"

#include "pv_factory.h"
#include "cvtFast.h"

#define ARC_K_COLORMODE_STATIC 0
#define ARC_K_COLORMODE_ALARM 1

#define ARC_MAJOR_VERSION 4
#define ARC_MINOR_VERSION 0
#define ARC_RELEASE 0

#ifdef __rectangle_obj_cc

#include "rectangle_obj.str"

static char *dragName[] = {
  activeRectangleClass_str2,
  activeRectangleClass_str3
};

static void doBlink (
  void *ptr
);

static void unconnectedTimeout (
  XtPointer client,
  XtIntervalId *id );

static void arc_edit_ok (
  Widget w,
  XtPointer client,
  XtPointer call );

static void arc_edit_update (
  Widget w,
  XtPointer client,
  XtPointer call );

static void arc_edit_apply (
  Widget w,
  XtPointer client,
  XtPointer call );

static void arc_edit_cancel (
  Widget w,
  XtPointer client,
  XtPointer call );

static void arc_edit_cancel_delete (
  Widget w,
  XtPointer client,
  XtPointer call );

#endif

class activeRectangleClass : public activeGraphicClass {

private:

friend void doBlink (
  void *ptr
);

friend void unconnectedTimeout (
  XtPointer client,
  XtIntervalId *id );

friend void arc_edit_ok (
  Widget w,
  XtPointer client,
  XtPointer call );

friend void arc_edit_update (
  Widget w,
  XtPointer client,
  XtPointer call );

friend void arc_edit_apply (
  Widget w,
  XtPointer client,
  XtPointer call );

friend void arc_edit_cancel (
  Widget w,
  XtPointer client,
  XtPointer call );

friend void arc_edit_cancel_delete (
  Widget w,
  XtPointer client,
  XtPointer call );

typedef struct editBufTag {
// edit buffer
  int bufX;
  int bufY;
  int bufW;
  int bufH;
  int bufLineColor;
  colorButtonClass lineCb;
  int bufLineColorMode;
  int bufFill;
  int bufFillColor;
  colorButtonClass fillCb;
  int bufFillColorMode;
  char bufMinVisString[39+1];
  char bufMaxVisString[39+1];
  int bufVisInverted;
  char bufAlarmPvName[PV_Factory::MAX_PV_NAME+1];
  char bufVisPvName[PV_Factory::MAX_PV_NAME+1];
  int bufInvisible;
  int bufLineWidth;
  int bufLineStyle;
} editBufType, *editBufPtr;

editBufPtr eBuf;

entryListBase *invisPvEntry, *visInvEntry, *minVisEntry, *maxVisEntry;

entryListBase *fillEntry, *fillColorEntry, *fillAlarmSensEntry;

pvColorClass lineColor;

int lineColorMode;

int fill;

pvColorClass fillColor;

int fillColorMode;

int pvType;
pvValType pvValue, minVis, maxVis;
char minVisString[39+1];
char maxVisString[39+1];

int visibility, prevVisibility, visInverted;
int lineVisibility, prevLineVisibility;
int fillVisibility, prevFillVisibility;

ProcessVariable *alarmPvId;
ProcessVariable *visPvId;

expStringClass alarmPvExpStr;

expStringClass visPvExpStr;

int alarmPvExists, visPvExists;
int activeMode, init, opComplete;

int invisible;
 int lineWidth;
int lineStyle;

int needConnectInit, needAlarmUpdate, needVisUpdate, needRefresh;
int needToDrawUnconnected, needToEraseUnconnected;
XtIntervalId unconnectedTimer;

int curLineColorIndex, curFillColorIndex, curStatus, curSeverity;

static const int alarmPvConnection = 1;
static const int visPvConnection = 2;
pvConnectionClass connection;

public:

static void alarmPvConnectStateCallback (
  ProcessVariable *pv,
  void *userarg
);

static void alarmPvValueCallback (
  ProcessVariable *pv,
  void *userarg
);

static void visPvConnectStateCallback (
  ProcessVariable *pv,
  void *userarg
);

static void visPvValueCallback (
  ProcessVariable *pv,
  void *userarg
);

activeRectangleClass ( void );

// copy constructor
activeRectangleClass
 ( const activeRectangleClass *source );

~activeRectangleClass ( void );

char *objName ( void ) {

  return name;

}

int createInteractive (
  activeWindowClass *aw_obj,
  int x,
  int y,
  int w,
  int h );

int save (
  FILE *f );

int old_save (
  FILE *f );

int createFromFile (
  FILE *fptr,
  char *name,
  activeWindowClass *actWin );

int old_createFromFile (
  FILE *fptr,
  char *name,
  activeWindowClass *actWin );

int importFromXchFile (
  FILE *fptr,
  char *name,
  activeWindowClass *actWin );

int genericEdit ( void );

int edit ( void );

int editCreate ( void );

int draw ( void );

int drawActiveIfIntersects (
  int x0,
  int y0,
  int x1,
  int y1 );

int drawActive ( void );

int erase ( void );

int eraseActive ( void );

int eraseUnconditional ( void );

int expandTemplate (
  int numMacros,
  char *macros[],
  char *expansions[] );

int expand1st (
  int numMacros,
  char *macros[],
  char *expansions[] );

int expand2nd (
  int numMacros,
  char *macros[],
  char *expansions[] );

int containsMacros ( void );

int activate ( int pass, void *ptr );

int deactivate ( int pass );

int isInvisible ( void )
{
  return invisible;
}

void executeDeferred ( void );

char *firstDragName ( void );

char *nextDragName ( void );

char *dragValue (
  int i );

void changeDisplayParams (
  unsigned int flag,
  char *fontTag,
  int alignment,
  char *ctlFontTag,
  int ctlAlignment,
  char *btnFontTag,
  int btnAlignment,
  int textFgColor,
  int fg1Color,
  int fg2Color,
  int offsetColor,
  int bgColor,
  int topShadowColor,
  int botShadowColor );

void changePvNames (
  int flag,
  int numCtlPvs,
  char *ctlPvs[],
  int numReadbackPvs,
  char *readbackPvs[],
  int numNullPvs,
  char *nullPvs[],
  int numVisPvs,
  char *visPvs[],
  int numAlarmPvs,
  char *alarmPvs[] );

void updateColors (
  double colorValue );

void getPvs (
  int max,
  ProcessVariable *pvs[],
  int *n );

char *getSearchString (
  int index );

void replaceString (
  int i,
  int max,
  char *string
);

char *crawlerGetFirstPv ( void );

char *crawlerGetNextPv ( void );

};

#ifdef __cplusplus
extern "C" {
#endif

void *create_activeRectangleClassPtr ( void );
void *clone_activeRectangleClassPtr ( void * );

#ifdef __cplusplus
}
#endif

#endif
