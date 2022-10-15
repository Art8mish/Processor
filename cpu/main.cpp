
#include "cpu.h"

static const char * const CODE_INPUT_FILE   = "../io/asm_output";
static const char * const LOG_FILE_NAME     = "../io/cpu_out.txt";

int main()
{
    struct CpuField field = {};
    STACKCTOR(&field.stk, 10);      //initialize stacks
    STACKCTOR(&field.ret_adr, 10);

    int read_code_err_check = ReadCode(CODE_INPUT_FILE, &field);

    CPU_ERROR_CHECK(read_code_err_check, READ_CODE_ERROR);

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

    CPU_ERROR_CHECK(perform_code_err_check, PERFORM_CODE_ERROR);

    StackDtor(&field.stk);
    StackDtor(&field.ret_adr);

    return SUCCESS;
}


int ReadCode(const char *code_file_name, struct CpuField *field)
{
    CPU_ERROR_CHECK(code_file_name == NULL, PTR_NULL);
    CPU_ERROR_CHECK(         field == NULL, PTR_NULL);

    FILE *input_file = fopen(code_file_name, "r");

    FILE_ERROR_CHECK(input_file == NULL, OPENING_FILE_ERROR, input_file);

    int read_header_err = ReadHeader(input_file, field);
    FILE_ERROR_CHECK(read_header_err, READ_HEADER_ERROR, input_file);

    fread(field->code_buffer, sizeof(int), field->op_count, input_file);
    int fread_err_check = ferror(input_file);

    FILE_ERROR_CHECK(fread_err_check, FREAD_ERROR, input_file);

    fclose(input_file);

    return SUCCESS;
}

int ReadHeader(FILE *input_file, struct CpuField *field)
{
    ASM_ERROR_CHECK(input_file == NULL, PTR_NULL);
    ASM_ERROR_CHECK(     field == NULL, PTR_NULL);

    int  header[HEADER_SIZE] = {};

    fread(header, sizeof(int), HEADER_SIZE, input_file);
    int fread_err_check = ferror(input_file);

    ASM_ERROR_CHECK(fread_err_check, FREAD_ERROR);

    ASM_ERROR_CHECK(strncmp((char*)header, SIGNATURE, 2), WRONG_SIGNATURE_ERROR);

    int version  = header[1];

    ASM_ERROR_CHECK(version != CPU_VERSION, WRONG_VERSION_ERROR);

    field->op_count = header[2];

    //printf("Version: %d; field.op_count = %d\n", version, field->op_count);

    field->code_buffer = (int*) calloc(field->op_count, sizeof(int));

    ASM_ERROR_CHECK(field->code_buffer == NULL, CALLOC_ERROR);

    return SUCCESS;
}

int DumpProcessor(struct CpuField *field)
{
    CPU_ERROR_CHECK(field == NULL, PTR_NULL);
    CPU_ERROR_CHECK(&field->stk == NULL, PTR_NULL);

    FILE *log_file = fopen(LOG_FILE_NAME, "a");

    fprintf(log_file, "\nProcessor Dump:\n");

    //print commands
    for (int i = 0; i < field->op_count; i++)
        fprintf(log_file, "%-8d ", i+1);
    fprintf(log_file, "\n");

    int *code = field->code_buffer;
    for (int i = 0; i < field->op_count; i++)
        fprintf(log_file, "%-8X ", *code++);
    fprintf(log_file, "\n");


    //print pc
    for (int i = 0; i < field->pc * 9; i++)
        fprintf(log_file, " ");
    fprintf(log_file, "^\n");
    for (int i = 0; i < field->pc * 9; i++)
        fprintf(log_file, " ");
    fprintf(log_file, "pc = %d\n", field->pc);

    //print stack
    fprintf(log_file, "Stack:\n");
    fprintf(log_file, "{\n");
    for (int i = 0; i < field->stk.data_size; i++)
        fprintf(log_file, "\t[%02d] = " ELEM_T_SPECIFIER "\n", i, field->stk.data[i]);
    fprintf(log_file, "}\n\n");

    //print Regs
    fprintf(log_file, "Regs[]:\n");
    fprintf(log_file, "{\n");
    for (int i = 0; i < REGS_AMOUNT; i++)
        fprintf(log_file, "\tRegs[%02d] = %d\n", i, field->Regs[i]);
    fprintf(log_file, "}\n\n");

    //print RAM
    fprintf(log_file, "Ram[]:\n\n");
    //for (int i = 0; i < RAM_SIZE; i++)
    //    fprintf(log_file, "%-8d ", i);
    //fprintf(log_file, "\n");

    for (int i = 0; i < RAM_SIZE; i++)
    {
        if ((i > 0) && (i % 100 == 0))
            fprintf(log_file, "\n");
        fprintf(log_file, "%-2d ", field->Ram[i]);
    }
    fprintf(log_file, "\n");


    fclose(log_file);

    return SUCCESS;
}

int ExecuteCode(struct CpuField *field)
{
    FILE *log_file = fopen(LOG_FILE_NAME, "a");

    int *code = field->code_buffer;
    int  first_value = 0;
    int second_value = 0;
    int        value = 0;
    int      *pt_val = &value;

    field->pc = 0;

    while(code[field->pc] != HLT_CODE)
    {
        switch(GETKEY(code[field->pc]))
        {

            #define DEF_CMD(name, num, arg, cpu_code, err_check)    \
                case name##_CODE :  if (arg == 1)                   \
                                        GetArg(field, &value);      \
                                    else if (arg == 2)              \
                                        GetPtrArg(field, &pt_val);  \
                                    err_check                       \
                                    cpu_code                        \
                                    break;


            #include "../cmd.h"

            default : printf(" # PerformCode(): ERROR: code = %d. \n", GETKEY(code[field->pc]));
                      break;

            #undef DEF_CMD
        }

        value = 0;
        pt_val = &value;
        field->pc++;
    }

    fclose(log_file);

    return SUCCESS;
}

int GetArg(struct CpuField *field, int *value)
{
    CPU_ERROR_CHECK(field == NULL, PTR_NULL);
    CPU_ERROR_CHECK(value == NULL, PTR_NULL);

    int val = 0;
    int cmd = field->code_buffer[field->pc];

    if (cmd & IMMEDIATE_CONST_CODE)
        val += field->code_buffer[++field->pc];

    if (cmd & REGISTER_CODE)
        val += field->Regs[field->code_buffer[++field->pc]];

    if (cmd & MEMORY_CODE)
    {
        CPU_ERROR_CHECK(val < 0 || val >= RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        val = field->Ram[val];
        printf("catch_memory_code, value = %d\n", val);
    }

    *value = val;

    return SUCCESS;
}

int GetPtrArg(struct CpuField *field, int **val)
{
    CPU_ERROR_CHECK(field == NULL, PTR_NULL);
    CPU_ERROR_CHECK(  val == NULL, PTR_NULL);

    int cmd = field->code_buffer[field->pc];

    CPU_ERROR_CHECK(cmd & IMMEDIATE_CONST_CODE && !(cmd & MEMORY_CODE), SYNTAX_ERROR);

    if (cmd & REGISTER_CODE && !(cmd & MEMORY_CODE))
    {
        field->pc++;

        CPU_ERROR_CHECK(field->code_buffer[field->pc] < 0 ||
                        field->code_buffer[field->pc] >= REGS_AMOUNT, SYNTAX_ERROR);

        *val = &field->Regs[field->code_buffer[field->pc]];
    }

    if (cmd & IMMEDIATE_CONST_CODE)
    {
        **val += field->code_buffer[++field->pc];
    }

    if ((cmd & REGISTER_CODE) && (cmd & MEMORY_CODE))
    {
        field->pc++;

        CPU_ERROR_CHECK(field->code_buffer[field->pc] < 0 || field->code_buffer[field->pc] > REGS_AMOUNT, SYNTAX_ERROR);

        **val += field->Regs[field->code_buffer[field->pc]];
    }

    if (cmd & MEMORY_CODE)
    {
        CPU_ERROR_CHECK(**val < 0 || **val >= RAM_SIZE, INCORRECT_RAM_CELL_ERROR);
        *val = &field->Ram[**val];
    }

    return SUCCESS;
}
