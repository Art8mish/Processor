
#include "../include/disasm.h"

int main()
{
    struct DisAsmField field = {};

    DisAsmFieldCtor(&field);

    int read_code_err_check = ReadCode(CODE_INPUT_FILE, &field);
    ERROR_CHECK(read_code_err_check, READ_CODE_ERROR);

    int err_check = DisAsmCode(&field);
    ERROR_CHECK(err_check, DISASM_CODE_ERROR);

    DisAsmFieldDtor(&field);

    printf("SUCCESFUL END.\n");

    return SUCCESS;
}

int DisAsmCode(struct DisAsmField *field)
{
    FILE *log_file = fopen(LOG_FILE_NAME, "w");

    int *code = field->code_buffer;

    field->pc = 0;

    while(code[field->pc] != HLT_CODE)
    {
        switch(code[field->pc] & 0xFFFF)
        {

            #define DEF_CMD(name, num, arg, cpu_code, err_check)           \
                        case name##_CODE : fprintf(log_file, #name);       \
                                           if (arg)                        \
                                           {                               \
                                               fprintf(log_file, " ");     \
                                               PrintArg(field, log_file);  \
                                           }                               \
                                           break;

            #include "../../cmd.h"

            #undef DEF_CMD

            default : printf(" # DisAsmCode(): ERROR: code = %d. \n", code[field->pc] & (int)0xFFFF);
                      return SYNTAX_ERROR;
                      break;
        }

        fprintf(log_file, "\n");
        field->pc++;
    }

    fprintf(log_file, "HLT\n");

    fclose(log_file);

    return SUCCESS;
}
/*
int ReadCode(const char *code_file_name, struct DisAsmField *field)
{
    ERROR_CHECK(code_file_name == NULL, NULL_PTR_ERROR);
    ERROR_CHECK(         field == NULL, NULL_PTR_ERROR);

    FILE *input_file = fopen(code_file_name, "r");

    FILE_ERROR_CHECK(input_file == NULL, OPENING_FILE_ERROR, input_file);

    //read header
    int  header[HEADER_SIZE] = {};

    char signature[SIGNATURE_LENGTH] = {};

    fread(header, sizeof(int), HEADER_SIZE, input_file);
    int fread_err_check = ferror(input_file);

    FILE_ERROR_CHECK(fread_err_check, FREAD_ERROR, input_file);

    signature[0] = *((char*)header);
    signature[1] = *((char*)header + 1);
    signature[2] = '\0';

    FILE_ERROR_CHECK(strcmp(signature, SIGNATURE), WRONG_SIGNATURE_ERROR, input_file);

    int version  = (int)header[1];

    FILE_ERROR_CHECK(version != VERSION, WRONG_VERSION_ERROR, input_file);

    field->op_count = (int)header[2];

    field->code_buffer = (int*) calloc(field->op_count, sizeof(int));

    fread(field->code_buffer, sizeof(int), field->op_count, input_file);
    fread_err_check = ferror(input_file);

    FILE_ERROR_CHECK(fread_err_check, FREAD_ERROR, input_file);

    fclose(input_file);

    return SUCCESS;
}*/

int PrintArg(struct DisAsmField *field, FILE *out_file)
{
    ERROR_CHECK(field == NULL, NULL_PTR_ERROR);

    int cmd = field->code_buffer[field->pc];

    if (cmd & MEMORY_CODE)
        fprintf(out_file, "[");

    if (cmd & IMMEDIATE_CONST_CODE)
        fprintf(out_file, "%d", field->code_buffer[++field->pc]);

    if ((cmd & IMMEDIATE_CONST_CODE) && (cmd & REGISTER_CODE))
         fprintf(out_file, "+");

    if (cmd & REGISTER_CODE)
    {
        field->pc++;
        if (field->code_buffer[field->pc]      ==  FIRST_REG)
            fprintf(out_file, "rax");
        else if (field->code_buffer[field->pc] == SECOND_REG)
            fprintf(out_file, "rbx");
        else if (field->code_buffer[field->pc] ==  THIRD_REG)
            fprintf(out_file, "rcx");
        else if (field->code_buffer[field->pc] == FOURTH_REG)
            fprintf(out_file, "rdx");
        else
            return WRONG_REG_ERROR;

    }

    if (cmd & MEMORY_CODE)
        fprintf(out_file, "]");

    return SUCCESS;
}

int DisAsmFieldCtor(struct DisAsmField *field)
{
    ERROR_CHECK(field == NULL, NULL_PTR_ERROR);

    *field = {};

    return SUCCESS;
}

int DisAsmFieldDtor(struct DisAsmField *field)
{
    ERROR_CHECK(field == NULL, NULL_PTR_ERROR);

    free(field->code_buffer);

    return SUCCESS;
}
