#ifndef _DEBUG_PRINT_H
#define _DEBUG_PRINT_H

#define DEBUG_PRINT 0
#define debug_print(fmt, ...)                                       \
    do                                                              \
    {                                                               \
        if (DEBUG_PRINT)                                            \
            fprintf(stderr, "-------%s: %d: %s():---" fmt "----\n", \
                    __FILE__, __LINE__, __func__, ##__VA_ARGS__);   \
    } while (0)

#endif
