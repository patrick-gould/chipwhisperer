/**
    Author: Patrick T. Gould,
    The Ohio State University - Department of Computer Science and Engineering.

    This file, adapted from Chipwhisperer's + ARMORY's example programs, contains a simple password program that listens for a command to start from Chipwhisperer in an infinite loop.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See: <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdlib.h>
#include "hal.h"
#include "simpleserial.h"

#define PASS_SUCCESS 1 // Password Success; when a string has not failed the password check.
#define PASS_FAILURE 0 // Password failure; when a string has failed the password check.

/// @brief A blank function that may used as a halting symbol; A point in the code to show a fault has occurred. E.g., an instruction skip allowed unreachable code—like this function—to be executed.
uint8_t __attribute__((noinline)) super_secret_function()
{
    return (uint8_t)PASS_SUCCESS; // Return PASS_SUCCESS since we "passed" the password check.
}

#if SS_VER == SS_VER_2_1
uint8_t password(uint8_t cmd, uint8_t scmd, uint8_t len, uint8_t *pw)
#else
uint8_t password(uint8_t *pw, uint8_t len) // Alternate header used if using simple_serial.1.x
#endif
{
    // Enables ADC counter. This is how we count clock cycles since the ADC samples 4 times each cycle—by default, anyway. Takes roughly 45 cycles of overhead on the ICE40 with a Neorv32 flashed.
    trigger_high();

    // Make sure to include overhead of adding variables inside trigger to keep timing consistent.
    char passwd[] = "touch"; // Password coming in should be "00000" by default.
    char badPasswd[] = "00000";
    char passok = PASS_SUCCESS; // Default value since an option to pass is to skip the for-loop.
    int cnt;                    // Loop counter.

    // Simple test - doesn't check for too-long password!
    for (cnt = 0; cnt < 5; cnt++)
    {
        if (badPasswd[cnt] != passwd[cnt])
        {
            passok = PASS_FAILURE;
        }
    }

    // If the above code somehow fails, we should pass this if-condition.
    if (passok)
    {
        passok = super_secret_function(); // We "should" never reach this line; function returns PASS_SUCCESS.
    }
    
    trigger_low(); // Disables ADC counter.
    simpleserial_put('r', 1, (uint8_t *)&passok);
    return 0x0; // simpleserial_put(...) talks to the outside world, so no need ot return passok here.
}

// #pragma GCC pop_options

int main(void)
{
    // Neorv32 platform init is empty, no need to call it.
    // platform_init();
    // The 3 functions below just call neorv32.h functions.
    init_uart();
    trigger_setup();
    simpleserial_init();

// Set callback function(s).
#if SS_VER == SS_VER_2_1
    simpleserial_addcmd(0x01, 5, password);
#else
    simpleserial_addcmd('p', 5, password);
#endif

    while (1)
        // Looks for input coming in from Chipwhisperer simpleserial, calls callback function on input.
        simpleserial_get();
}
