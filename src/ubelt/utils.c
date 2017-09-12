#include "utils.h"

#ifdef __APPLE__
char* cf_string_ref_to_chars(CFStringRef string) {
    if (string == NULL) {
        return NULL;
    }

    CFIndex length = CFStringGetLength(string);
    CFIndex maxSize =
        CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;

    // Allocate memory
    char* buffer = (char*)malloc(maxSize);

    // Attempt conversion into the buffer
    if (CFStringGetCString(string, buffer, maxSize, kCFStringEncodingUTF8)) {
        return buffer;
    }

    // Could not convert CFStringRef, free then return NULL.
    free(buffer);
    return NULL;
}

CFStringRef char_to_cf_string_ref(char* c) {
    return CFStringCreateWithCStringNoCopy(NULL, c, kCFStringEncodingUTF8,
                                           NULL);
}

#endif

#ifdef _DEBUG_
static FILE* LOG_FILE = NULL;
#endif

void util_print(const char* format, ...) {
#ifdef _DEBUG_
    va_list* ap = malloc(sizeof(va_list));
    char* fmt = malloc(sizeof(char*) * 64);

    va_start(*ap, format);
    sprintf(fmt, "%s%s%s", RED, format, RESET);
    vfprintf(stderr, fmt, *ap);
    va_end(*ap);

    free(ap);
    free(fmt);
#else
    if (format != NULL) {
      return;
    }
#endif
}

void util_debug(const char* format, ...) {
#ifdef _DEBUG_
  if (LOG_FILE == NULL) {
    struct passwd* pw = getpwuid(getuid());
    char* file = pw->pw_dir;
    int size = sizeof(char*) * 128;
    char* file_log = malloc(size);

    snprintf(file_log, size, "%s/.midi-mapper.log", file);
    LOG_FILE = fopen(file_log, "a");
    free(file_log);

    if (LOG_FILE == NULL) {
      util_error("Failed to write to log file");
      exit(EXIT_FAILURE);
    }
  }

  va_list* ap = malloc(sizeof(va_list));
  va_start(*ap, format);
  vfprintf(LOG_FILE, format, *ap);
  va_end(*ap);

  free(ap);
  fflush(LOG_FILE);
  /* fclose(LOG_FILE); */
  /* LOG_FILE = NULL; */
#else
    if (format != NULL) {
      return;
    }
#endif
}

int util_count_lines(char* input) {
    int i = 0;
    int lines = 1;
    while (input[i] != '\0') {
        ++i;
        if (input[i] == '\n') {
            lines++;
        }
    }
    return lines;
}

void util_clear(int lines) {
    for (int i = 0; i < lines; ++i) {
        printf("\33[2K\r");
        printf("\33[1A\r");
        printf("\33[2K\r");
    }
}

int64_t util_micros() {
#ifdef __linux__
    struct timespec tms;
    timespec_get(&tms, TIME_UTC);
    int64_t micros = tms.tv_sec * 1000000;
    return micros += tms.tv_nsec / 1000;

#elif __APPLE__
    static mach_timebase_info_data_t timebase_info;
    if (timebase_info.denom == 0) {
      // Zero-initialization of statics guarantees that denom will be 0 before
      // calling mach_timebase_info.  mach_timebase_info will never set denom to
      // 0 as that would be invalid, so the zero-check can be used to determine
      // whether mach_timebase_info has already been called.  This is
      // recommended by Apple's QA1398.
      mach_timebase_info(&timebase_info);
    }

    /* uint64_t tick_value = mach_absolute_time(); */
    /* uint64_t value_diff = tick_value - prev_tick_value; */
    uint64_t result = mach_absolute_time();

    /* To prevent overflow */
    result /= 1000;

    result *= timebase_info.numer;
    result /= timebase_info.denom;

    // return microseconds
    return result;
#endif
}

void util_error(char* format, ...) {
    va_list ap;
    va_start(ap, format);
    char* fmt = malloc(sizeof(char*) * 64);
    sprintf(fmt, "%s%s%s", RED, format, RESET);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    putc('\n', stderr);
}

bool util_contains_bit(unsigned val, unsigned bitindex) {
    return (val & (1 << bitindex)) != 0;
}

int util_tokenize(char* src, char* delim, char** result) {
    char* container = NULL;
    char* token = NULL;
    int i = 0;
    do {
        token = strtok_r(src, delim, &container);
        src = NULL;
        if (token != NULL) {
            result[i] = strdup(token);
            i++;
        }
    } while (token != NULL);
    free(token);

    return i;
}

/**
 * There be fast cats here, use at your own risk!
 * Appends a src string onto the tail of an existing buffer;
 *
 * Take caution to keep a handle onto the original starting buffer point,
 * ise you will lose it due to pointer arithmetic.
 *
 * When you need fast appending of cats, create a char* on the stack and
 * continually use the return value of this function to track the end.
 * Then once you are done with fast cat appendages, you may discard the stack
 * pointer.
 */
void util_cat(char** orig, char* src) {
    // printf("dm_cat: appending(%s) to buf(%s)\n", src, buf);
    char* buf = *orig;
    assert(buf != NULL);

    while (*buf) {
        buf++;
    }

    while ((*buf++ = *src++)) {};

    *orig = --buf;
    // return --buf;
}

const char *util_home_dir() {
    const char *homedir;

    // Check environment variable first,
    // then fallback to passwd definition.
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return homedir;
}
