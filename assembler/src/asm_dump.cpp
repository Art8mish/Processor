
#include "../include/asm.h"

int DumpCode(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    int *code = field->code_buffer;

    printf("\n\nDUMP\n");

    //print Code
    printf("\nCode: \n");
    for (int i = 0; i < field->pc; i++)
        printf("%d ", code[i]);
    printf("\n\n");

    bool two_arg = false;

    //print commands
    while (*code != HLT_CODE)
    {
        #define DEF_CMD(name, num, arg, cpu_code, err)      \
            else if ((*code & 0xFFFF)  == name##_CODE)      \
            {                                               \
                if (arg)                                    \
                {                                           \
                    printf("%x(0x) %-4s : ", *code, #name); \
                    PRINT_ARG();                            \
                }                                           \
                else                                        \
                    printf("%5d     %-4s\n", *code, #name); \
            }

        if (false) {}

        #include "../../cmd.h"

        else
            printf("UNKNOWN COMMAND");

        code++;
        two_arg = false;

        #undef DEF_CMD
    }

    //print labels
    printf("\nlabels: \n");
    for (unsigned int i = 0; i < LABELS_AMOUNT; i++)
        printf("labels[%d].name = %s, labels[%d].value = %d\n", i, field->labels[i].name, i, field->labels[i].value);

    return SUCCESS;
}

int DumpField(struct AsmField *field)
{
    printf("\nStruct Asm : char_buffer[%p], chars_count %d, code_buffer[%p], lines_count %d\n",
            field->char_buffer, field->chars_count,
            field->code_buffer, field->lines_count);

    printf("\n");
    printf("%s", field->char_buffer);
    printf("\n");

    return SUCCESS;
}

