#ifndef ASM_H_INCLUDED
#define ASM_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <TXlib.h>
#include <sys/stat.h>
#include "../../../Stack/libraries.h"

#ifdef SOFT_ASSERT
#undef SOFT_ASSERT
#endif

#ifndef ASSERT_MODE
#define ASSERT_MODE
#endif

#ifdef ASSERT_MODE

#define SOFT_ASSERT(condition)                                                    \
            do                                                                    \
            {                                                                     \
                if (condition)                                                 \
                    printf("Error in %s; file: %s; num of line: %d \n",           \
                           #condition, __FILE__, __LINE__);                       \
            } while(false)

#else

#define SOFT_ASSERT(condition) ;

#endif


#define ASM_ERROR_CHECK(cond, rtrn)                         \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                    return rtrn;                            \
            } while(false)

#define READFILE_ERROR_CHECK(cond, rtrn, closing_file)      \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                {                                           \
                    fclose(closing_file);                   \
                    return rtrn;                            \
                }                                           \
            } while(false)


const size_t ASM_VERSION        = 1;

const size_t MAX_WORD_LEN       = 34;
const size_t HEADER_SIZE        = 3;
const size_t ARG_IN_LINE_AMOUNT = 3;
const size_t LABELS_AMOUNT      = 10;
const size_t REGISTER_LENGTH    = 3;

const int   LABEL_VALUE_POISON  = -1;


enum AsmError
{
          USER_FILE_OPENING_ERROR = 13,
          USER_FILE_CLOSING_ERROR = 14,
    OUTPUT_ASM_FILE_OPENING_ERROR = 15,
    OUTPUT_ASM_FILE_CLOSING_ERROR = 16,
                      FREAD_ERROR = 17,
                       STAT_ERROR = 18,
                  COUNTSIZE_ERROR = 19,
                 COUNTLINES_ERROR = 20,
             UNKNOWN_COMAND_ERROR = 21,
                     NO_HLT_ERROR = 22,
                     SYNTAX_ERROR = 23,
             LABEL_OVERFLOW_ERROR = 24,
    NO_LABEL_WITH_THIS_NAME_ERROR = 25,
           FIND_LABEL_VALUE_ERROR = 26,
              PROCESS_LABEL_ERROR = 27,
                READ_HEADER_ERROR = 28,
                 READ_LABEL_ERROR = 29,
};

#define DEF_CMD(name, num, arg, code, err_check) \
            name##_CODE = num,
enum Code
{
    #include "../../cmd.h"
};

#undef DEF_CMD

enum CmdConst
{
    IMMEDIATE_CONST_CODE = 1 << 16,
           REGISTER_CODE = 1 << 17,
             MEMORY_CODE = 1 << 18,
};


struct Label
{
    int  value = LABEL_VALUE_POISON;
    char name[MAX_WORD_LEN]= {};
};

struct AsmField
{
    char   *char_buffer = NULL;
    size_t  chars_count = 0;
    int    *code_buffer = NULL;
    size_t  lines_count = 0;
    int     pc          = 0;
    struct  Label labels[LABELS_AMOUNT] = {{}};
};

int ReadUserFile(const char *user_file_name, struct AsmField *field);

int AssemblyUserCode(struct AsmField *field);

int WriteCode(struct AsmField *field, const char *output_file_name);

int CountSize(const char *file_name, size_t *file_size);
int CountLines(struct AsmField *field, size_t *value);

int ReadArg(struct AsmField *field, char **buf);

int PrintHeader(struct AsmField *field);

int ReadLabel(struct AsmField *field, char **buf);

int FindLabelValue(struct AsmField *field, char *label_name, int *arg);
int ProcessLabel(struct AsmField *field, char *cmd);

int InitializeLabels(struct AsmField *field);

int DumpCode(struct AsmField *field);

int DumpField(struct AsmField *field);

#endif // ASM_H_INCLUDED
