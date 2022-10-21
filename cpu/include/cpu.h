#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
//#include <TXlib.h>

#include "../../stack_functions/include/libraries.h"
#define STACK_LIB_INCLUDED

#include "../../errors.h"

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



#define ERROR_CHECK(cond, rtrn)                             \
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
const size_t HEADER_SIZE      = 3;

const char * const SIGNATURE  = "DP";

static const char * const LOG_FILE_NAME = "../io/cpu_out.txt";

#define DEF_CMD(name, num, arg, code) \
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


struct CpuField
{
    int    *code_buffer   = NULL;
    size_t  op_count      = 0;
    unsigned int pc       = 0;
    struct Stack stk      = {};
    struct Stack ret_adr  = {};
    int Regs[REGS_AMOUNT] = {0, 0, 0, 0, 0};
    int  Ram[RAM_SIZE]    = {};
};



int CpuFieldCtor(struct CpuField *field);
int CpuFieldDtor(struct CpuField *field);

int ReadCode(const char *code_file_name, struct CpuField *field);

int ReadHeader(FILE *input_file, struct CpuField *field);


int ExecuteCode(struct CpuField *field);
int GetPtrArg(struct CpuField *field, int **val);

int DumpProcessor(struct CpuField *field);

int DumpCommands(struct CpuField *field, FILE *dump_file);
int DumpPc(struct CpuField *field, FILE *dump_file);
int DumpStack(struct CpuField *field, FILE *dump_file);
int DumpRegs(struct CpuField *field, FILE *dump_file);
int DumpRam(struct CpuField *field, FILE *dump_file);

#endif // CPU_H_INCLUDED
