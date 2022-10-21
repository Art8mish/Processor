
#include "../include/cpu.h"

int main()
{
    struct CpuField field = {};

    CpuFieldCtor(&field);

    int read_code_err = ReadCode(CODE_INPUT_FILE, &field);
    ERROR_CHECK(read_code_err, READ_CODE_ERROR);
    
    int execute_code_err = ExecuteCode(&field);
    ERROR_CHECK(execute_code_err, EXECUTE_CODE_ERROR);

    CpuFieldDtor(&field);

    printf("\nSUCCESSFUL END\n");

    return SUCCESS;
}

