/*
 *  Copyright (c) 2024 Intel Corporation. All rights reserved.
 *  This software is available to you under the BSD license below:
 *
 *      Redistribution and use in source and binary forms, with or
 *      without modification, are permitted provided that the following
 *      conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <shmem.h>
#include <shmemx.h>

#define NELEM 10

long src[NELEM];
long dest[NELEM];
long expected[NELEM];

int main(void)
{
    int me;
    int errors = 0;

    shmem_init();

    me = shmem_my_pe();

    for (int i = 0; i < NELEM; i++) {
        src[i] = me + i;
    }
    
    for (int i = 0; i < NELEM; i++) {
        expected[i] = (me + i) * ((me + 1 + i)) / 2 - (i * (i - 1)) / 2;
    }
    
    shmem_barrier_all();

    shmemx_long_sum_inscan(SHMEM_TEAM_WORLD, dest, src, NELEM);
    
    /* Validate inscan */
    for (int j = 0; j < NELEM; j++) {        
        if (dest[j] != expected[j]) {
            printf("%d: Expected dest[%d] = %ld, got %ld\n", me, j, expected[j], dest[j]);
            errors++;
        }
    }

    shmem_finalize();

    return errors != 0;
}

