/*
 * Copyright (c) 2005, Kohsuke Ohtani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * mutex.c - test priority inheritance of mutex.
 */

#include <sys/prex.h>
#include <stdio.h>

static mutex_t mtx_A = MUTEX_INITIALIZER;
static mutex_t mtx_B, mtx_C;

int main(int argc, char* argv[])
{
    int error;

    printf("Mutex test program\n");

    /*
     * Initialize only B
     */
    mutex_init(&mtx_B);

    /*
     * Lock test
     */
    error = mutex_lock(&mtx_A);
    printf("1) Lock mutex A: error=%d\n", error);

    error = mutex_lock(&mtx_B);
    printf("2) Lock mutex B: error=%d\n", error);

    /* Error: Mutex C is not initialized. */
    error = mutex_lock(&mtx_C);
    printf("3e) Lock mutex C: error=%d\n", error);

    /*
     * Unlock test
     */
    error = mutex_unlock(&mtx_A);
    printf("4) Unlock mutex A: error=%d\n", error);

    error = mutex_unlock(&mtx_B);
    printf("5) Unlock mutex B: error=%d\n", error);

    /* Error: Mutex C is not initialized. */
    error = mutex_unlock(&mtx_C);
    printf("6e) Unlock mutex B: error=%d\n", error);

    /* Error: B is not locked. */
    error = mutex_unlock(&mtx_B);
    printf("7e) Unlock mutex B: error=%d\n", error);

    /*
     * Destoroy mutex.
     */
    mutex_destroy(&mtx_B);

    /* Error: Mutex B is destoroyed. */
    error = mutex_lock(&mtx_B);
    printf("8e) Lock mutex B: error=%d\n", error);

    /*
     * Double lock test
     */
    error = mutex_lock(&mtx_A);
    printf("9) Lock mutex A: error=%d\n", error);

    /* Erorr: Mutex A is already locked */
    error = mutex_lock(&mtx_A);
    printf("10e) Lock mutex A: error=%d\n", error);

    error = mutex_unlock(&mtx_A);
    printf("11) Unlock mutex A: error=%d\n", error);

    printf("Test completed...\n");
    return 0;
}
