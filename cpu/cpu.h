#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <TXlib.h>
#include <sys/stat.h>

#ifndef STACK_LIB_INCLUDED
#define STACK_LIB_INCLUDED

#include "../../Stack/libraries.h"
#include "../../Stack/construct_functions.cpp"
#include "../../Stack/pop_push_functions.cpp"
#include "../../Stack/verification_functions.cpp"
#include "../assembler/include/asm.h"

#endif

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
                if (condition)                                                    \
                    printf("Error in %s; file: %s; num of line: %d \n",           \
                           #condition, __FILE__, __LINE__);                       \
            } while(false)

#else

#define SOFT_ASSERT(condition) ;

#endif



#define CPU_ERROR_CHECK(cond, rtrn)                         \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                    return rtrn;                            \
            } while(false)

#define FILE_ERROR_CHECK(cond, rtrn, closing_file)          \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                if (cond)                                   \
                {                                           \
                    fclose(closing_file);                   \
                    return rtrn;                            \
                }                                           \
            } while(false)


#define GETKEY(arg)                                         \
            (arg & 0xFFFF)

#define PI 3.14159265

const size_t CPU_VERSION      = 1;

const size_t SIGNATURE_LENGTH = 3;
const size_t RAM_SIZE         = 10000;
const size_t REGS_AMOUNT      = 5;

const char * const SIGNATURE         = "DP";

//static const char *const CODE_INPUT_FILE   = "C:/Program Files/CodeBlocks/CBProjects/Processor/io/asm_output";
//static const char *const LOG_FILE_NAME     = "C:/Program Files/CodeBlocks/CBProjects/Processor/io/cpu_out.txt";

enum CpuError
{
        WRONG_SIGNATURE_ERROR = 31,
          WRONG_VERSION_ERROR = 32,
              READ_CODE_ERROR = 33,
    INCORRECT_USER_CODE_ERROR = 34,
           PERFORM_CODE_ERROR = 35,
     INCORRECT_RAM_CELL_ERROR = 36,
       DIVISION_BY_ZERO_ERROR = 37,
                  EMPTY_ERROR = 39
};


struct CpuField
{
    int    *code_buffer = NULL;
    size_t  op_count      = 0;
    unsigned int pc       = 0;
    struct Stack stk      = {};
    struct Stack ret_adr  = {};
    int Regs[REGS_AMOUNT] = {0, 0, 0, 0, 0};
    int  Ram[RAM_SIZE]    = {};
};

int ReadCode(const char *code_file_name, struct CpuField *field);

int ReadHeader(FILE *input_file, struct CpuField *field);

int GetArg(struct CpuField *field, int *value);

int GetPtrArg(struct CpuField *field, int **val);

int DumpProcessor(struct CpuField *field);

int ExecuteCode(struct CpuField *field);


#endif // CPU_H_INCLUDED
