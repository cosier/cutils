#include "utils.h"

#ifdef APPLE_OS
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

#ifdef APP_NAME
#define APP_LOG_NAME APP_NAME
#else
#define APP_LOG_NAME "debug_belt"
#endif

#endif

void ub_print(const char* format, ...) {
#ifdef _DEBUG_
    va_list* ap = malloc(sizeof(va_list));
    char* fmt = malloc(sizeof(char) * 64);

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

void ub_debug(const char* format, ...) {
#ifdef _DEBUG_
  if (LOG_FILE == NULL) {
    char* file = ub_home_dir();
    int size = sizeof(char*) * 128;
    char* file_log = malloc(size);

    snprintf(file_log, size, "%s/." APP_LOG_NAME ".log", file);
    LOG_FILE = fopen(file_log, "a");
    free(file_log);

    if (LOG_FILE == NULL) {
      ub_error("Failed to write to log file");
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

int ub_count_lines(char* input) {
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

void ub_clear(int lines) {
    for (int i = 0; i < lines; ++i) {
        printf("\33[2K\r");
        printf("\33[1A\r");
        printf("\33[2K\r");
    }
}

int64_t ub_micros() {
#ifdef LINUX_OS
    struct timespec tms;
    timespec_get(&tms, TIME_UTC);
    int64_t micros = tms.tv_sec * 1000000;
    return micros += tms.tv_nsec / 1000;

#elif APPLE_OS
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
#else
    return 0;
#endif
}

void ub_error(char* format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t size = 64 * sizeof(char);
    char* fmt = calloc(64, sizeof(char));
    snprintf(fmt, size, "%s%s%s", RED, format, RESET);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    putc('\n', stderr);
}

bool ub_contains_bit(unsigned val, unsigned bitindex) {
    return (val & (1 << bitindex)) != 0;
}

ub_tokens* ub_tokenize(const char* src, char delim) {
    int buf_size = strlen(src);
    char** buf = malloc(sizeof(char*) * buf_size + 10);
    int tokens = 0;
    const char* cursor = src;

    // printf("ub_token_split(%s) on %c\n", cursor, delim);

    for (int i = 0; i < buf_size; ++i) {
        int token_tracker = 0;
        char* token = NULL;

        if (!*cursor) {
            break;
        }

        while (*cursor) {
            if (*cursor == ' ') {
                // skip whitespace in the parser
                ++cursor;
            } else if (*cursor == delim) {
                // printf("cursor(%c) == delim(%c)\n", *cursor, delim);
                // printf("token finished: %s\n", token);
                ++cursor;
                break;
            } else {
                int ap_size = sizeof(char) * 4;
                char* appendage = malloc(ap_size);
                if (token != NULL) {
                    snprintf(appendage, ap_size, "%s%c", token, *cursor);
                    free(token);
                } else {
                    snprintf(appendage, ap_size, "%c", *cursor);
                }

                token = appendage;
                ++token_tracker;
                ++cursor;
            }
        }

        if (token_tracker > 0) {
            // printf("processed buf[%d] = token: %s\n", tokens, token);
            if (tokens < buf_size) {
                buf[tokens] = token;
                ++tokens;
            }
        }
    }

    // printf("token_split done, found(%d) tokens\n\n", tokens);
    // return buf;
    ub_tokens* result = malloc(sizeof(ub_tokens));

    char** mbuf = malloc(sizeof(char*) * tokens + 1);
    for (int b = 0; b < tokens; ++b) {
        mbuf[b] = buf[b];
    }

    free(buf);

    result->count = tokens;
    result->tokens = mbuf;
    // free(buf);

    return result;
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
void ub_cat(char** orig, char* src) {
    // printf("ub_cat: appending(%s) to buf(%s)\n", src, buf);
    char* buf = *orig;
    assert(buf != NULL);

    while (*buf) {
        buf++;
    }

    while ((*buf++ = *src++)) {};

    *orig = --buf;
    // return --buf;
}

char *ub_home_dir() {
#ifdef WINDOWS_OS
    // Check environment variable first,
    // then fallback to passwd definition.
    char *ret = calloc(MAX_PATH, sizeof(char*));
    WCHAR* path = calloc(MAX_PATH, sizeof(WCHAR*));

    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path))) {
        sprintf(ret, "%ws", path);
        return ret;
    } else
    {
        return NULL;
    }
#endif
#ifdef UNIX_OS
    const char *homedir;

    // Check environment variable first,
    // then fallback to passwd definition.
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return strdup(homedir);
#endif

    return NULL;
}
