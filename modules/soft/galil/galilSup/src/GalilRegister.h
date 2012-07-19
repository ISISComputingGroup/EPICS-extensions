/*
FILENAME...	GalilRegister.h
USAGE... This file contains function prototypes for Galil IOC shell commands.

Version:	  1-4
Original Author:  Clift, M.
Date:	          2006/04/17
*/

/*
 * Modification Log:
 * -----------------
 */

#include "motor.h"
#include "motordrvCom.h"

/* Function prototypes. */
extern RTN_STATUS  G21X3Setup(int, int);
extern RTN_STATUS  G21X3Config(int, char *, int, int, int, int, int);
extern RTN_STATUS G21X3NameConfig(int, char *, int, int, int, double, int, int, int, int, int, int, int, int, int, int, double, char *, int, int, char *, int, char *);
extern RTN_STATUS G21X3StartCard(int, char *, int, int);
