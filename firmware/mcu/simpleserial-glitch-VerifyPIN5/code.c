/**************************************************************************/
/*                                                                        */
/*  This file is part of FISSC.                                           */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.0.                                              */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.0                 */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/

#include "interface.h"
#include "types.h"
#include "commons.h"

extern SBYTE g_ptc;
extern BOOL g_authenticated;
extern UBYTE g_userPin[PIN_SIZE];
extern UBYTE g_cardPin[PIN_SIZE];

BOOL verifyPIN() {
    int i;
    BOOL status;
    BOOL diff;
    int stepCounter;
    SBYTE ptcCpy = g_ptc;
    g_authenticated = BOOL_FALSE;

    if(g_ptc > 0) {
        if(ptcCpy != g_ptc) {
            countermeasure();
        }
        g_ptc--;
        if(g_ptc != ptcCpy-1) {
            countermeasure();
        }
        ptcCpy--;

        status = BOOL_FALSE;
        diff = BOOL_FALSE;
        stepCounter = 0;
        for(i = 0; i < PIN_SIZE; i++) {
            if(g_userPin[i] != g_cardPin[i]) {
                diff = BOOL_TRUE;
            }
            stepCounter++;
        }
        if(stepCounter != PIN_SIZE) {
            countermeasure();
        }
        if(i != PIN_SIZE) {
            countermeasure();
        }
        if (diff == BOOL_FALSE) {
            status = BOOL_TRUE;
        } else {
            status = BOOL_FALSE;
        }

        if(status == BOOL_TRUE) {
            if(ptcCpy != g_ptc) {
                countermeasure();
            }
            g_ptc = 3;
            g_authenticated = BOOL_TRUE; // Authentication();
            return BOOL_TRUE;
        }
    }

    return BOOL_FALSE;
}
