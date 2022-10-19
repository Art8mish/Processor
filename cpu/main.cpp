
#include "cpu.h"

static const char * const CODE_INPUT_FILE   = "../io/asm_output";


int main()
{
    struct CpuField field = {};

    CpuFieldCtor(&field);


    int read_code_err_check = ReadCode(CODE_INPUT_FILE, &field);

    ERROR_CHECK(read_code_err_check, READ_CODE_ERROR);

    //DumpProcessor(&field);
    /*
    //dumpcode
    printf("\nCode: \n");
    int *code = field.code_buffer;
    for(int i = 0; i < field.op_count; i++)
    {
        printf(" %d ", *code++);
    }
    printf("\n");
    //
    */
    int perform_code_err_check = ExecuteCode(&field);

    ERROR_CHECK(perform_code_err_check, PERFORM_CODE_ERROR);

    StackDtor(&field->stk);
    StackDtor(&field->ret_adr);

    free(field->code_buffer);
    free(field->Ram);

    CpuFieldDtor(&field);

    return SUCCESS;
}

