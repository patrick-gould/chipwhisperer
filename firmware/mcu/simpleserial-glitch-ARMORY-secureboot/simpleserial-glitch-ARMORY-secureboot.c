/**
    Author: [omitted]

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

#include <stdbool.h>

typedef uint8_t u8;
typedef uint32_t u32;

void verify_and_run_firmware(u8 *, u32,  u8 *);
void report_error();
void execute_firmware();


#define CH(x, y, z)     ((x & y) ^ ( (~x) & z))
#define MAJ(x, y, z)    ((x & y) ^ (x & z) ^ (y & z))
#define ROTR(x, n)      ((x >> n) | (x << (32-n)))
#define BSIG0(x)        (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x)        (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x)        (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3))
#define SSIG1(x)        (ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10))

// test harness definitions
int testRunner(uint8_t);
#define PASS_SUCCESS 1 // If a glitch successfully happened.
#define PASS_FAILURE 0 // Normal or corrupted run.
uint8_t glitch_result = (uint8_t) PASS_FAILURE; // Holds if super_secret_function was ever called. 



/// @brief A blank function that may used as a halting symbol; This is point in the code to show a fault has occurred. E.g., an instruction skip allowed unreachable code—like this function—to be executed.
uint8_t __attribute__((noinline)) super_secret_function()
{
    glitch_result = PASS_SUCCESS;
    return PASS_SUCCESS;
}


/// @brief A simple test driver: raises trigger, invokes code under test, lowers trigger, tests success.
#if SS_VER == SS_VER_2_1
uint8_t bigNum(uint8_t cmd, uint8_t scmd, uint8_t dlen, uint8_t *data)
#else
uint8_t password(void) // Alternate header used if using simple_serial.1.x
#endif
{
    // Enables ADC counter. This is how we count clock cycles since the ADC samples 4 times each cycle—by default, anyway. This operation takes roughly 45 cycles of overhead on the ICE40 with a Neorv32 flashed.
    trigger_high();

    testRunner(0); // Pass in 0 to trick compiler into keeping the impossible if-statement inside of test_runner()

    trigger_low(); // Disables ADC counter.
    
    simpleserial_put('r', 1, (uint8_t *)&glitch_result); // Communicate result with python.

    return 0x0; // simpleserial_put(...) talks to the outside world; we have no need to return anything here.
}

static const u32 constants[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
u32 input_length = 128;
u8 input[128] = {
        0x67, 0x45, 0x8b, 0x6b, 0xc6, 0x23, 0x7b, 0x32, 0x69, 0x98, 0x3c, 0x64, 0x73, 0x48, 0x33, 0x66,
        0x51, 0xdc, 0xb0, 0x74, 0xff, 0x5c, 0x49, 0x19, 0x4a, 0x94, 0xe8, 0x2a, 0xec, 0x58, 0x55, 0x62,
        0x29, 0x1f, 0x8e, 0x23, 0xcd, 0x7c, 0xe8, 0x46, 0xba, 0x58, 0x1b, 0x3d, 0xab, 0xd7, 0x7e, 0x50,
        0xf2, 0x41, 0xb1, 0x2e, 0xfb, 0x1e, 0xb7, 0x41, 0xe3, 0xa9, 0xe2, 0x79, 0x46, 0xe1, 0x45, 0x75,
        0x7c, 0x00, 0x5f, 0x51, 0xc2, 0x62, 0xd0, 0x5b, 0x54, 0x08, 0x20, 0x12, 0xf8, 0x27, 0xb1, 0x4d,
        0x1b, 0x23, 0x16, 0x02, 0xe8, 0xe9, 0x16, 0x1f, 0xe7, 0xcd, 0x90, 0x11, 0x8d, 0x43, 0xef, 0x66,
        0x76, 0x0f, 0x0e, 0x14, 0x5a, 0x25, 0x52, 0x33, 0x2e, 0xf9, 0x9c, 0x10, 0x63, 0x72, 0xed, 0x0d,
        0x33, 0xc2, 0xdc, 0x7f, 0x9f, 0xd7, 0xef, 0x1b, 0xc9, 0xc4, 0xa7, 0x41, 0x9a, 0x07, 0x68, 0x6b,
        };
u8 correct_hash[32] = {
        0xf2, 0x60, 0x9f, 0x26, 0x98, 0x6b, 0x3f, 0xd2, 0xdd, 0x21, 0x8b, 0x1d, 0xb6, 0x37, 0x41, 0x70,
        0x0b, 0x02, 0xe3, 0x27, 0x42, 0x9a, 0x2b, 0xb5, 0xb0, 0xbe, 0xfb, 0x76, 0x02, 0x45, 0x2b, 0xbe,
        };


int __attribute__((noinline)) testRunner(uint8_t zero){

    // Call test code
    int retVal;                             // Return value. We consider value of "fail" to be a 

    verify_and_run_firmware(input, input_length, correct_hash);

    // run our glitch check
    if(zero){
        super_secret_function();
    }
    
    return retVal;
}


// requires input_length to be multiple of 64
void sha256(u8* input, u32 input_length, u8 digest[32])
{
    u32 state[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    for (int block = 0; block < input_length; block += 64)
    {
        u32 W[64];
        for (int i = 0; i < 16; ++i)
        {
            W[i] = (input[block + 4 * i] << 24) | (input[block + 4 * i + 1] << 16) | (input[block + 4 * i + 2] << 8) | input[block + 4 * i + 3];
        }
        for (u32 i = 16; i < 64; ++i)
        {
            W[i] = SSIG1(W[i - 2]) + W[i - 7] + SSIG0(W[i - 15]) + W[i - 16];
        }

        u32 tmp[8] = {state[0], state[1], state[2], state[3], state[4], state[5], state[6], state[7]};

        for (int t = 0; t < 64; ++t)
        {
            u32 T1 = tmp[7] + BSIG1(tmp[4]) + CH(tmp[4], tmp[5], tmp[6]) + constants[t] + W[t];
            u32 T2 = BSIG0(tmp[0]) + MAJ(tmp[0], tmp[1], tmp[2]);
            for (int i = 7; i > 0; --i)
            {
                tmp[i] = tmp[i - 1];
            }
            tmp[0] = T1 + T2;
            tmp[4] += T1;
        }

        for (int i = 0; i < 8; ++i)
        {
            state[i] += tmp[i];
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        digest[4 * i] = (u8)(state[i] >> 24);
        digest[4 * i + 1] = (u8)(state[i] >> 16);
        digest[4 * i + 2] = (u8)(state[i] >> 8);
        digest[4 * i + 3] = (u8)(state[i]);
    }
}

void verify_and_run_firmware(u8* firmware, u32 firmware_length, u8 correct_hash[32])
{
    u8 hash[32];
    sha256(firmware, firmware_length, hash);

    for (u32 i = 0; i < 32; ++i)
    {
        if (hash[i] != correct_hash[i])
        {
            //report_error();
        }
    }

    //execute_firmware(); // Just calls an infinite loop which was useful for ARMORY's analysis but not for ours. Cut it.
    return; 
}

__attribute__((noreturn, noinline)) void report_error()
{
    while (1)
        ; // No need to do an infinite loop here. Was Useful for ARMORY, not us.
}

// self calling makes it easy to detect whether control-flow countermeasure raised an error by simply waiting for the second call of the function
__attribute__((noreturn, noinline)) void execute_firmware()
{
    //execute_firmware(); 
    // Remove recursive call. This was useful for ARMORY in software simulation.
    // On hardware, this will cause issues.
}


/// @brief  Waits for signal from Chipwhisperer and invokes callback to test function.
/// @return none. Program is an infinite loop.
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
    simpleserial_addcmd(0x01, 5, bigNum);
#else
    simpleserial_addcmd('p', 5, bigNum);
#endif

    while (1)
        // Looks for input coming in from Chipwhisperer simpleserial, calls callback function on input.
        simpleserial_get();
}
