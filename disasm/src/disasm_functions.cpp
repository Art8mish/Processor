
#include "../include/disasm.h"

int DisAsmCode(struct DisAsmField *field)
{
    FILE *log_file = fopen(LOG_FILE_NAME, "w");

    int *code = field->code_buffer;

    field->pc = 0;

    while(code[field->pc] != HLT_CODE)
    {
        switch(code[field->pc] & 0xFFFF)
        {

            #define DEF_CMD(name, num, arg, cpu_code)                      \
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

        switch (field->code_buffer[field->pc])
        {
            case  FIRST_REG: fprintf(out_file, "rax");
                            break;
            case SECOND_REG: fprintf(out_file, "rbx");
                            break;
            case  THIRD_REG: fprintf(out_file, "rcx");
                            break;
            case FOURTH_REG: fprintf(out_file, "rdx");
                            break;

            default: return WRONG_REG_ERROR;
                     break;
        }
    }

    if (cmd & MEMORY_CODE)
        fprintf(out_file, "]");

    return SUCCESS;
}

int DisAsmFieldCtor(struct DisAsmField *field)
{
    ERROR_CHECK(field == NULL, NULL_PTR_ERROR);

    *field = (struct DisAsmField){};

    field->code_buffer = NULL;
    field->op_count    = 0;
    field->pc          = 0;

    return SUCCESS;
}

int DisAsmFieldDtor(struct DisAsmField *field)
{
    ERROR_CHECK(field == NULL, NULL_PTR_ERROR);
    
    ERROR_CHECK(field->code_buffer == NULL, NULL_PTR_ERROR);
    free(field->code_buffer);

    return SUCCESS;
}