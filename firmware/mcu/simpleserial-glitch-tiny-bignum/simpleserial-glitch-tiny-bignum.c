/**
    Author: Patrick T. Gould,
    The Ohio State University - Department of Computer Science and Engineering.

    Calls and tests timing for a small implementation of big number arithmetic. Code under test from https://github.com/kokke/tiny-bignum-c.

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
#include "bn.h"

void testRunner(uint8_t);


#define PASS_SUCCESS 1 // If a glitch successfully happened.
#define PASS_FAILURE 0 // Normal or corrupted run.
uint8_t glitch_result = (uint8_t) PASS_FAILURE; // Holds if super_secret_function was ever called. 



/// @brief A blank function that may used as a halting symbol; A point in the code to show a fault has occurred. E.g., an instruction skip allowed unreachable code—like this function—to be executed.
uint8_t __attribute__((noinline)) super_secret_function()
{
    glitch_result = PASS_SUCCESS;
    return PASS_SUCCESS;
}


#if SS_VER == SS_VER_2_1
uint8_t aes(uint8_t cmd, uint8_t scmd, uint8_t dlen, uint8_t *data)
#else
uint8_t aes(void) // Alternate header used if using simple_serial.1.x
#endif
{
    // Enables ADC counter. This is how we count clock cycles since the ADC samples 4 times each cycle—by default, anyway. Takes roughly 45 cycles of overhead on an ICE40 loaded with a Neorv32 softcore.
    trigger_high();

    testRunner(0);

    trigger_low(); // Disables ADC counter.
    
    //simpleserial_put('r', 1, (uint8_t *)&glitch_result); // Communicate result with python.
    simpleserial_put('r', 1, (uint8_t *)&glitch_result);
    return 0x0; // simpleserial_put(...) talks to the outside world; we have no need to return anything here.
}

void __attribute__((noinline)) testRunner(uint8_t zero){

    // Call test code
    main_Factorial();
    // main_golden();
    // main_rsa();

    // run our glitch check
    if(zero){
        super_secret_function();
    }
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
