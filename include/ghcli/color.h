/*
 * Copyright 2022 Nico Sonack <nsonack@outlook.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#include <sn/sn.h>

#define GHCLI_256COLOR_DONE   0x3F0FAF00
#define GHCLI_256COLOR_OPEN   0x04FF0100

enum {
    GHCLI_COLOR_BLACK,
    GHCLI_COLOR_RED,
    GHCLI_COLOR_GREEN,
    GHCLI_COLOR_YELLOW,
    GHCLI_COLOR_BLUE,
    GHCLI_COLOR_MAGENTA,
    GHCLI_COLOR_CYAN,
    GHCLI_COLOR_WHITE,
    GHCLI_COLOR_DEFAULT,
};

const char *ghcli_setcolor256(uint32_t colorcode);
const char *ghcli_resetcolor(void);
const char *ghcli_setcolor(int color);
const char *ghcli_state_color_sv(sn_sv state);
const char *ghcli_state_color_str(const char *it);
const char *ghcli_setbold(void);
const char *ghcli_resetbold(void);

#endif /* COLOR_H */
