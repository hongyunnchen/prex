/*-
 * Copyright (c) 2007, Kohsuke Ohtani
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
 * args.c - check arguments for main() routine.
 */

#include <sys/prex.h>
#include <stdio.h>
#include <stdlib.h>

extern char** environ;

int main(int argc, char* argv[])
{
    int i;
    char** envp = environ;

    printf("argument test\n");

    /*
     * Print all arguments that were input by user.
     */
    printf("argc=%d argv=%x\n", argc, (unsigned int)argv);
    printf("Dump args:\n");
    for (i = 0; i < argc; i++) {
        /* printf("argv[%d]: 0x%x\n", i, (unsigned int)argv[i]); */
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    printf("Dump envs:\n");
    for (i = 0; envp[i] != NULL; i++) {
        /* printf("envp[%d]: 0x%x\n", i, (unsigned int)envp[i]); */
        printf("envp[%d]: %s\n", i, envp[i]);
    }
    exit(0);
}
