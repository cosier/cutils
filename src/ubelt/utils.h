#ifndef MIDI_UTILS_H
#define MIDI_UTILS_H

#ifdef WINDOWS_OS
#include <Windows.h>
#endif

#include "colors.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <assert.h>

#include <sys/types.h>
#include <stdint.h>

#ifdef WINDOWS_OS
#include <Shlobj.h>
#endif

#ifdef UNIX_OS
#include <pwd.h>
#include <unistd.h>
#endif

#ifdef APPLE_OS
#include <mach/mach_time.h>
#include <mach/mach.h>
#endif

extern bool dm_driver_debug_mode;

typedef struct ub_tokens {
    char** tokens;
    int count;
} ub_tokens;

void ub_debug(const char* format, ...);
void ub_error(char* format, ...);
void ub_print(const char* format, ...);

void ub_clear(int lines);
int64_t ub_micros();

int ub_count_lines(char* input);
bool ub_contains_bit(unsigned val, unsigned bitindex);

ub_tokens* ub_tokenize(const char* src, char delim);
void ub_cat(char** buf, char* src);

char* ub_home_dir();

//////////////////////////////////////////////////////////
// HERE BE APPLES
#ifdef APPLE_OS
#include <CoreFoundation/CoreFoundation.h>

char* cf_string_ref_to_chars(CFStringRef string);
CFStringRef char_to_cf_string_ref(char* c);
#endif

#endif
