
#include "../include/cpu.h"


int CpuFieldCtor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    *field = {};

    STACKCTOR(&field->stk, 10);      //initialize stacks
    STACKCTOR(&field->ret_adr, 10);

    return SUCCESS;
}

int CpuFieldDtor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    StackDtor(&field->stk);
    StackDtor(&field->ret_adr);

    free(field->code_buffer);

    return SUCCESS;
}


int ExecuteCode(struct CpuField *field)
{
    FILE *log_file = fopen(LOG_FILE_NAME, "a");

    int      arg_val = 0;
    int *ptr_arg_val = &arg_val;

    int *code = field->code_buffer;

    int  lhs  = 0;
    int  rhs  = 0;

    field->pc = 0;
    while(code[field->pc] != HLT_CODE)
    {
        switch(GETKEY(code[field->pc]))
        {

            #define DEF_CMD(name, num, arg, cpu_code, err_check)         \
                case name##_CODE :  if (arg == 1)                        \
                                    {                                    \
                                        GetPtrArg(field, &ptr_arg_val);  \
                                    }                                    \
                                    err_check                            \
                                    cpu_code                             \
                                    break;


            #include "../../cmd.h"

            default : printf(" # PerformCode(): ERROR: code = %d. \n", GETKEY(code[field->pc]));
                      break;

            #undef DEF_CMD
        }

        arg_val = 0;
        ptr_arg_val = &arg_val;
        field->pc++;
    }

    fclose(log_file);

    return SUCCESS;
}

/*
int GetArg(struct CpuField *field, int *value)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(value == NULL, PTR_NULL);

    int val = 0;
    int cmd = field->code_buffer[field->pc];

    if (cmd & IMMEDIATE_CONST_CODE)
        val += field->code_buffer[++field->pc];

    if (cmd & REGISTER_CODE)
        val += field->Regs[field->code_buffer[++field->pc]];

    if (cmd & MEMORY_CODE)
    {
        ERROR_CHECK(val < 0 || val >= (int)RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        val = field->Ram[val];
        //printf("catch_memory_code, value = %d\n", val);
    }

    *value = val;

    return SUCCESS;
}*/

int GetPtrArg(struct CpuField *field, int **val)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(  val == NULL, PTR_NULL);

    int cmd = field->code_buffer[field->pc];

    if (cmd & REGISTER_CODE && !(cmd & MEMORY_CODE))
    {
        field->pc++;

        ERROR_CHECK(field->code_buffer[field->pc] < 0 ||
                        field->code_buffer[field->pc] >= (int)REGS_AMOUNT, SYNTAX_ERROR);

        *val = &field->Regs[field->code_buffer[field->pc]];
    }

    if (cmd & IMMEDIATE_CONST_CODE)
    {
        **val += field->code_buffer[++field->pc];
    }

    if ((cmd & REGISTER_CODE) && (cmd & MEMORY_CODE))
    {
        field->pc++;

        ERROR_CHECK(field->code_buffer[field->pc] < 0 || field->code_buffer[field->pc] > (int)REGS_AMOUNT, SYNTAX_ERROR);

        **val += field->Regs[field->code_buffer[field->pc]];
    }

    if (cmd & MEMORY_CODE)
    {
        ERROR_CHECK(**val < 0 || **val >= (int)RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        *val = &field->Ram[**val];
    }

    return SUCCESS;
}


int DumpProcessor(struct CpuField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(&field->stk == NULL, PTR_NULL);

    FILE *log_file = fopen(LOG_FILE_NAME, "a");

    fprintf(log_file, "\nProcessor Dump:\n");

    //print commands
    for (int i = 0; i < (int)field->op_count; i++)
        fprintf(log_file, "%-8d ", i+1);
    fprintf(log_file, "\n");

    int *code = field->code_buffer;
    for (int i = 0; i < (int)field->op_count; i++)
        fprintf(log_file, "%-8X ", *code++);
    fprintf(log_file, "\n");


    //print pc
    for (int i = 0; i < (int)field->pc * 9; i++)
        fprintf(log_file, " ");
    fprintf(log_file, "^\n");
    for (int i = 0; i < (int)field->pc * 9; i++)
        fprintf(log_file, " ");
    fprintf(log_file, "pc = %d\n", field->pc);

    //print stack
    fprintf(log_file, "Stack:\n");
    fprintf(log_file, "{\n");
    for (int i = 0; i < (int)field->stk.data_size; i++)
        fprintf(log_file, "\t[%02d] = " ELEM_T_SPECIFIER "\n", i, field->stk.data[i]);
    fprintf(log_file, "}\n\n");

    //print Regs
    fprintf(log_file, "Regs[]:\n");
    fprintf(log_file, "{\n");
    for (int i = 0; i < (int)REGS_AMOUNT; i++)
        fprintf(log_file, "\tRegs[%02d] = %d\n", i, field->Regs[i]);
    fprintf(log_file, "}\n\n");

    //print RAM
    fprintf(log_file, "Ram[]:\n\n");
    //for (int i = 0; i < RAM_SIZE; i++)
    //    fprintf(log_file, "%-8d ", i);
    //fprintf(log_file, "\n");

    for (int i = 0; i < (int)RAM_SIZE; i++)
    {
        if ((i > 0) && (i % 100 == 0))
            fprintf(log_file, "\n");
        fprintf(log_file, "%-2d ", field->Ram[i]);
    }
    fprintf(log_file, "\n");


    fclose(log_file);

    return SUCCESS;
}

