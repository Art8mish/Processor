
#include "include/asm.h"

#define CHECK_AND_WRITE(cmd, str_code, code_num)       \
            if (stricmp(cmd, str_code) == 0)           \
            {                                          \
                code[field->pc] = code_num;            \
            }


#define PRINT_ARG()                                                 \
    if (*code & MEMORY_CODE)                                        \
        printf("RAM : ");                                           \
    if ((*code & REGISTER_CODE) && (*code & IMMEDIATE_CONST_CODE))  \
    {                                                               \
        printf("%d (num) & %d (reg)", *(code+1), *(code+2));        \
        two_arg = true;                                             \
    }                                                               \
    else if (*code & REGISTER_CODE)                                 \
        printf("%d (reg)", *(code + 1));                            \
    else if (*code & IMMEDIATE_CONST_CODE)                          \
        printf("%d (num)", *(code + 1));                            \
    else                                                            \
        printf("ERROR/label : %d", *(code+1));                      \
    printf("\n");                                                   \
                                                                    \
    if (two_arg)                                                    \
        code++;                                                     \
                                                                    \
    code++

#define SKIP_DIGIT()                                                \
            while(isdigit(**buf)) {(*buf)++;}

const char *SIGNATURE = "DP";

int ProccessMainArgument(int argc, char *argv[], const char **input_file_name, const char **output_file_name)
{
    ERROR_CHECK(            argv == NULL, PTR_NULL);
    ERROR_CHECK( input_file_name == NULL, PTR_NULL);
    ERROR_CHECK(output_file_name == NULL, PTR_NULL);


    if (argc == 1)
    {
         *input_file_name =  INPUT_FILE_NAME;
        *output_file_name = OUTPUT_FILE_NAME;
    }

    else if (argc == 2)
    {
         *input_file_name = argv[1];
        *output_file_name = OUTPUT_FILE_NAME;
    }

    else if (argc == 3)
    {
         *input_file_name = argv[1];
        *output_file_name = argv[2];
    }

    else
    {
        return TOO_MANY_MAIN_ARGS_ERROR;
    }

    return SUCCESS;
}

int ReadUserFile(const char *user_file_name, struct AsmField *field)
{
    ERROR_CHECK(user_file_name == NULL, PTR_NULL);
    ERROR_CHECK(         field == NULL, PTR_NULL);

    //count file_size
    int countsize_err = CountSize(user_file_name, &(field->chars_count));
    ERROR_CHECK(countsize_err, COUNTSIZE_ERROR);

    //read file
    FILE *input_file = fopen(user_file_name, "r");
    READFILE_ERROR_CHECK(input_file == NULL, USER_FILE_OPENING_ERROR, input_file);

    field->char_buffer = (char*) calloc(field->chars_count + 1, sizeof(char));
    READFILE_ERROR_CHECK(field->char_buffer == NULL, CALLOC_ERROR, input_file);

    fread(field->char_buffer, sizeof(char), field->chars_count, input_file);

    int fread_error_check = ferror(input_file);
    READFILE_ERROR_CHECK(fread_error_check, FREAD_ERROR, input_file);

    fclose(input_file);

    //count lines in file
    int countlines_err = CountLines(field, &(field->lines_count));

    ERROR_CHECK(countlines_err, COUNTLINES_ERROR);

    field->code_buffer = (int*) calloc(field->lines_count * MAX_LINE_SIZE, sizeof(int));
    field->code_buffer += HEADER_SIZE;

    return SUCCESS;
}

int AssemblyUserCode(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    int  *code = field->code_buffer;
    char *buf  = field->char_buffer;

    char cmd[MAX_WORD_LEN] = {};

    field->pc = 0;                  //needed for second assembly

    while (strcmp(cmd, "hlt"))
    {
        char *is_label = NULL;

        while(isspace(*buf)) { buf++; }

        sscanf(buf, "%s", cmd);
        printf("cmd : %s\n", cmd);
        buf += strlen(cmd) + 1;

        //find label
        is_label = strchr(cmd, ':');

        //process label
        if (is_label)
        {
            int process_label_err = ProcessLabel(field, cmd);

            ERROR_CHECK(process_label_err, PROCESS_LABEL_ERROR);
        }

        #define DEF_CMD(name, num, arg, cpu_code, err_check)          \
                if (stricmp(cmd, #name) == 0)                         \
                {                                                     \
                    if (arg)                                          \
                    {                                                 \
                        code[field->pc] |= name##_CODE;               \
                        int readarg_err = ReadArg(field, &buf);       \
                        ERROR_CHECK(readarg_err, SYNTAX_ERROR);   \
                    }                                                 \
                    else                                              \
                        code[field->pc] = name##_CODE;                \
                }                                                     \

        #include "../cmd.h"

        #undef DEF_CMD

        if (!is_label)
            field->pc++;
    }

    return SUCCESS;
}

int WriteHeader(struct AsmField *field, FILE *output_file)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    int header[HEADER_SIZE] = {};

    *((char*)header)     = SIGNATURE[0];
    *((char*)header + 1) = SIGNATURE[1];

             header[1]   = ASM_VERSION;
             header[2]   = field->pc;

    fwrite(header, HEADER_SIZE, sizeof(int), output_file);

    return SUCCESS;
}

int ProcessLabel(struct AsmField *field, char *cmd)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(cmd   == NULL, PTR_NULL);

    int cmd_len = strlen(cmd);
    int arg     = 0;

    ERROR_CHECK(cmd[cmd_len - 1] != ':', SYNTAX_ERROR);

    cmd[cmd_len - 1] = '\0';

    FindLabelValue(field, cmd, &arg);

    if (arg == LABEL_VALUE_POISON)
    {
        for (int i = 0; i <= (int)LABELS_AMOUNT; i++)
        {
            ERROR_CHECK(i == LABELS_AMOUNT, LABEL_OVERFLOW_ERROR);

            if (field->labels[i].value == LABEL_VALUE_POISON
                && strcmp(field->labels[i].name, "") == 0)
            {
                strcpy(field->labels[i].name, cmd);

                field->labels[i].value = field->pc;

                break;
            }
        }
    }


    return SUCCESS;
}

int CountSize(const char *file_name, size_t *file_size)
{
    ERROR_CHECK(file_name == NULL, PTR_NULL);


    FILE *file = fopen(file_name, "r");
    READFILE_ERROR_CHECK(file == NULL, OPENING_FILE_ERROR, file);

    fseek(file, 0, SEEK_END);

    size_t size_value = ftell(file);

    fclose (file);

    *file_size = size_value;

    return SUCCESS;
}

int CountLines(struct AsmField *field, size_t *value)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    size_t lines_count = 0;
    char *ch = field->char_buffer;


    for (int i = 0; i < (int)field->chars_count; i++)
    {
        if (*ch == EOF)
            break;

        if (*ch == '\n')
            lines_count++;

        ch++;
    }

    *value = lines_count + 1;

    return SUCCESS;
}

int ReadArg(struct AsmField *field, char **buf)
{
    char str[MAX_WORD_LEN] = {};

    int  arg  = 0;

    bool label   = false;
    bool is_plus = false;
    bool memory  = false;

    if (**buf == '[')
    {
        memory = true;
        field->code_buffer[field->pc] |= MEMORY_CODE;
        (*buf)++;
    }

    sscanf(*buf, "%s", str);

    char  *plus_ptr = strchr(str, '+');
    char *label_ptr = strchr(str, ':');

    if (plus_ptr)
    {
        is_plus = true;
        field->code_buffer[field->pc] |= REGISTER_CODE;
        field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;
    }

    if (label_ptr)
        label = true;

    ERROR_CHECK(label && is_plus, SYNTAX_ERROR);


    if (is_plus)
    {
        int read_plus_construct_err = ReadPlusConstruction(field, buf);

        ERROR_CHECK(read_plus_construct_err, READ_PLUS_CONSTRUCT_ERROR);
    }

    //read label
    else if (label)
    {
        int read_label_err = ReadLabel(field, buf);

        ERROR_CHECK(read_label_err, READ_LABEL_ERROR);
    }

    //read register
    else if (**buf == 'r')
    {
        field->code_buffer[field->pc] |= REGISTER_CODE;

        int isreg_err = IsReg(*buf, &arg);
        ERROR_CHECK(isreg_err, ISREG_ERROR);

        field->code_buffer[++field->pc] = arg;

        (*buf) += REGISTER_LENGTH; //register_length
    }

    //read digit
    else if (isdigit(**buf) || **buf == '-')
    {
        int read_digit_err = ReadDigit(field, buf);
        ERROR_CHECK(read_digit_err, READ_DIGIT_ERROR);
    }

    else
        return SYNTAX_ERROR;

    if (memory)
    {
        ERROR_CHECK(**buf != ']', SYNTAX_ERROR);
        (*buf)++;
    }

    return SUCCESS;
}

int ReadDigit(struct AsmField *field, char **buf)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(field == NULL, PTR_NULL);

    int sign = 1;
    int arg  = 0;

    field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;

    if (**buf == '-')
    {
        sign = -1;
        (*buf)++;
    }

    sscanf(*buf, "%d", &arg);

    printf("arg : %d\n", arg);

    arg *= sign;

    field->code_buffer[++field->pc] = arg;

    SKIP_DIGIT();

    return SUCCESS;
}

int ReadPlusConstruction(struct AsmField *field, char **buf)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(field == NULL, PTR_NULL);

    int arg = 0;

    sscanf(*buf, "%d", &arg);

    field->code_buffer[++(field->pc)] = arg;

    SKIP_DIGIT();

    ERROR_CHECK(**buf != '+', SYNTAX_ERROR);

    (*buf)++;

    int isreg_err = IsReg(*buf, &arg);
    ERROR_CHECK(isreg_err, ISREG_ERROR);

    field->code_buffer[++(field->pc)] = arg;

    (*buf) += REGISTER_LENGTH;

    return SUCCESS;
}

int IsReg(char *buf, int *arg)
{
    ERROR_CHECK(buf == NULL, PTR_NULL);
    ERROR_CHECK(arg == NULL, PTR_NULL);

    ERROR_CHECK(*(buf + 1) < 'a' || *(buf + 1) > 'd', SYNTAX_ERROR);
    ERROR_CHECK(*(buf + 2) != 'x', SYNTAX_ERROR);

    *arg = *(buf+1) - 'a' + 1;

    return SUCCESS;
}

int ReadLabel(struct AsmField *field, char **buf)
{
    ERROR_CHECK(field == NULL, PTR_NULL);
    ERROR_CHECK(buf   == NULL, PTR_NULL);

    int arg = 0;
    char label_name[MAX_WORD_LEN] = {};

    ERROR_CHECK(**buf != ':', SYNTAX_ERROR);

    field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;

    (*buf)++;

    sscanf(*buf, "%s", label_name);

    int check_err = FindLabelValue(field, label_name, &arg);

    ERROR_CHECK(check_err, FIND_LABEL_VALUE_ERROR);

    //printf("arg = %d, file: %s, line: %d\n", arg, __FILE__, __LINE__);
    field->code_buffer[++(field->pc)] = arg;
    (*buf) += strlen(label_name) + 1;


    return SUCCESS;
}

int FindLabelValue(struct AsmField *field, char *label_name, int *arg)
{
    ERROR_CHECK(field      == NULL, PTR_NULL);
    ERROR_CHECK(label_name == NULL, PTR_NULL);
    ERROR_CHECK(arg        == NULL, PTR_NULL);

    for (int i = 0; i <= (int)LABELS_AMOUNT; i++)
    {
        if (i == LABELS_AMOUNT)
            *arg = LABEL_VALUE_POISON;

        if(strcmp(field->labels[i].name, label_name) == 0)
        {
            *arg = field->labels[i].value;

            break;
        }
    }

    return SUCCESS;
}

int InitializeLabels(struct AsmField *field)
{
    const char *label_poison_name = "";
    int label_name_len = strlen(label_poison_name);

    for (unsigned int i = 0; i < LABELS_AMOUNT; i++)
    {
        for (int j = 0; j < label_name_len; j++)
            field->labels[i].name[j] = label_poison_name[j];

        field->labels[i].value = LABEL_VALUE_POISON;
    }

    return SUCCESS;
}

int WriteCode(struct AsmField *field, const char *output_file_name)
{
    ERROR_CHECK(           field == NULL, PTR_NULL);
    ERROR_CHECK(output_file_name == NULL, PTR_NULL);

    FILE *output_file = fopen(output_file_name, "w");

    int write_header_err = WriteHeader(field, output_file);
    ERROR_CHECK(write_header_err, WRITE_HEADER_ERROR);

    fwrite(field->code_buffer, field->pc, sizeof(int), output_file);

    fclose(output_file);

    printf("\nCODE WRITED SUCCESSFULLY\n");

    return SUCCESS;
}

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

        #include "../cmd.h"

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

int AsmFieldCtor(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    *field = {};

    InitializeLabels(field);

    return SUCCESS;
}

int AsmFieldDtor(struct AsmField *field)
{
    ERROR_CHECK(field == NULL, PTR_NULL);

    free(field->char_buffer);
    free(field->code_buffer);

    return SUCCESS;
}
