#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
//#include <TXlib.h>
#include <string.h>
#include <sys/stat.h>
#include "sorting_func.h"
#include "onegin_io.h"


#define SOFT_ASSERT_MODE

#ifdef SOFT_ASSERT_MODE

#define SOFT_ASSERT(condition)                                            \
            do                                                            \
            {                                                             \
                if (!(condition))                                         \
                    printf("Error in %s; file: %s; num of line: %d \n",   \
                           #condition, __FILE__, __LINE__);               \
            } while(false)
#else

#define SOFT_ASSERT(condition)

#endif

#define Assert(condition, error_type)       \
        SOFT_ASSERT(condition);             \
        do                                  \
        {                                   \
            if (!(condition))               \
            {                               \
                /*error_code = error_type;*/\
                return error_type;          \
            }                               \
        } while(false)

enum Errors
{
    POINTER_IS_NULL    = -1,
    OPENING_FILE_ERROR = -2,
    STAT_ERROR         = -3
};


struct WorkingField
{
    int    chars_amount;
    char  *chars_buffer;
    char **pointers_buffer;
    int    lines_amount;
};


#endif // LIBRARIES_H_INCLUDED
