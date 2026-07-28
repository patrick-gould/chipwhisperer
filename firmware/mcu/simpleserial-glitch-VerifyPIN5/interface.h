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

/*$
    @authors = SERTIF Consortium
    @feature = N/A
    @target = N/A
    @countermeasure = N/A
    @difficulties = N/A
    @purpose = shared interface for examples
*/

#ifndef H_INTERFACE
#define H_INTERFACE

#include "types.h"

/* defines what happens when a fault has been detected
    implementation defined
*/
void countermeasure(void);

/* initializes global variables at the beginning of an example
    implementation-defined
    example-defined
*/
void initialize(void);

/* determines whether an attack is successful
    example-defined
*/
BOOL oracle(void);

#endif // H_INTERFACE
