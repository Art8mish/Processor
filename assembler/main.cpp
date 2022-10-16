
#include "include/asm.h"

//const char * const       USER_FILE_NAME = "../io/user_file.txt";
//const char * const       USER_FILE_NAME = "../io/krujochek.txt";
const char * const       USER_FILE_NAME = "../io/quadratic.txt";
//const char * const       USER_FILE_NAME = "../io/factorial.txt";

const char * const OUTPUT_ASM_FILE_NAME = "../io/asm_output";

int main()
{
    struct AsmField main_field = {};

    InitializeLabels(&main_field);

    int read_err_check = ReadUserFile(USER_FILE_NAME, &main_field);
    ASM_ERROR_CHECK(read_err_check, 1);

    DumpField(&main_field);

//First Assembly

    int convert_err_check = AssemblyUserCode(&main_field);
    ASM_ERROR_CHECK(convert_err_check, 2);

    DumpCode(&main_field);

//Second Assembly

    convert_err_check = AssemblyUserCode(&main_field);
    ASM_ERROR_CHECK(convert_err_check, 2);

    DumpCode(&main_field);


    WriteCode(&main_field, OUTPUT_ASM_FILE_NAME);

    free(main_field.char_buffer);
    free(main_field.code_buffer);

    printf("\nSUCCEFUL END.");

    getchar();

    return SUCCESS;
}


