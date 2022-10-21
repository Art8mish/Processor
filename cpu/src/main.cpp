
#include "../include/cpu.h"

static const char * const CODE_INPUT_FILE   = "../io/asm_output";


int main()
{
    struct CpuField field = {};

    CpuFieldCtor(&field);

    int read_code_err = ReadCode(CODE_INPUT_FILE, &field);
    ERROR_CHECK(read_code_err, READ_CODE_ERROR);

    //DumpProcessor(&field);
    /*
    //dumpcode
    printf("\nCode: \n");
    int *code = field.code_buffer;
    for(int i = 0; i < field.op_count; i++)
    {
        printf(" %d ", *code++);
    }
    printf("\n");*/
    
    int execute_code_err = ExecuteCode(&field);
    ERROR_CHECK(execute_code_err, EXECUTE_CODE_ERROR);

    CpuFieldDtor(&field);

    printf("SUCCESSFUL END\n");

    return SUCCESS;
}

