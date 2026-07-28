/**
    Author: [omitted]

    This file, adapted from tiny-aes, contains a simple and small version of AES.

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
#include <string.h>
#include "hal.h"
#include "simpleserial.h"


#define BOOL_TRUE 0xAA
#define PASS_SUCCESS 1 // If a glitch successfully happened.
#define PASS_FAILURE 0 // Normal or corrupted run.
uint8_t glitch_result = (uint8_t) PASS_FAILURE; // Holds if fault was successful.


void testRunner(uint8_t);   // Common point to start code-under-test.
int verifyPIN_main();       // old main for verifyPIN.

#if SS_VER == SS_VER_2_1
uint8_t aes(uint8_t cmd, uint8_t scmd, uint8_t dlen, uint8_t *data)
#else
uint8_t aes(void) // Alternate header used if using simple_serial.1.x
#endif
{
    // Enables ADC counter. This is how we count clock cycles since the ADC samples 4 times each cycle—by default, anyway. Takes roughly 45 cycles of overhead on an ICE40 loaded with a Neorv32 softcore.
    glitch_result = PASS_FAILURE;

    trigger_high();

    testRunner(0);

    trigger_low(); // Disables ADC counter.
    
    // VerifyPin returns BOOL_TRUE (0xAA) on a good glitch.
    if(glitch_result == BOOL_TRUE) glitch_result = PASS_SUCCESS;

    simpleserial_put('r', 1, (uint8_t *)&glitch_result); // Communicate result with python.

    return 0x0; // simpleserial_put(...) talks to the outside world; we have no need to return anything here.
}

void __attribute__((noinline)) testRunner(uint8_t zero){

    // Call test code
    glitch_result = verifyPIN_main();
}

int main(void)
{
    // Neorv32 platform init is empty, no need to call it.
    platform_init();
    // The 3 functions below just call neorv32.h functions.
    init_uart();
    trigger_setup();
    simpleserial_init();

// Set callback function(s).
#if SS_VER == SS_VER_2_1
    simpleserial_addcmd(0x01, 5, aes);
#else
    simpleserial_addcmd('p', 5, aes);
#endif

    while (1)
        // Looks for input coming in from Chipwhisperer simpleserial, calls callback function on input.
        simpleserial_get();
}
