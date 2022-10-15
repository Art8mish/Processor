
#include "include/asm.h"

#define CHECK_AND_WRITE(cmd, str_code, code_num)       \
            if (!stricmp(cmd, str_code))               \
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


int ReadUserFile(const char *user_file_name, struct AsmField *field)
{
    ASM_ERROR_CHECK(user_file_name == NULL, PTR_NULL);
    ASM_ERROR_CHECK(         field == NULL, PTR_NULL);

    //count file_size
    int countsize_err = CountSize(user_file_name, &(field->chars_count));
    ASM_ERROR_CHECK(countsize_err, COUNTSIZE_ERROR);

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

    ASM_ERROR_CHECK(countlines_err, COUNTLINES_ERROR);

    field->code_buffer = (int*) calloc(field->lines_count * ARG_IN_LINE_AMOUNT + HEADER_SIZE, sizeof(int));
    field->code_buffer += HEADER_SIZE;

    return SUCCESS;
}

int AssemblyUserCode(struct AsmField *field)
{
    ASM_ERROR_CHECK(field == NULL, PTR_NULL);

    int  *code = field->code_buffer;
    char *buf  = field->char_buffer;

    char cmd[MAX_WORD_LEN] = {};

    bool label = false;

    field->pc = 0;

    while (field->pc < field->lines_count * ARG_IN_LINE_AMOUNT)
    {
        label = false;

        while(isspace(*buf)) { buf++; }

        sscanf(buf, "%s", cmd);
        buf += strlen(cmd) + 1;

        //find label
        char *label_ex = strchr(cmd, ':');
        if (label_ex != NULL)
            label = true;

        //process hlt
        if (!stricmp(cmd, "hlt"))
        {
            code[field->pc] = HLT_CODE;
            field->pc++;

            PrintHeader(field);

            return SUCCESS;
        }

        //process label
        if (label)
        {
            int process_label_err = ProcessLabel(field, cmd);

            ASM_ERROR_CHECK(process_label_err, PROCESS_LABEL_ERROR);
        }

        #define DEF_CMD(name, num, arg, cpu_code, err_check)          \
                if (!stricmp(cmd, #name))                             \
                {                                                     \
                    if (arg)                                          \
                    {                                                 \
                        code[field->pc] |= name##_CODE;               \
                        int readarg_err = ReadArg(field, &buf);       \
                        ASM_ERROR_CHECK(readarg_err, SYNTAX_ERROR);   \
                    }                                                 \
                    else                                              \
                        code[field->pc] = name##_CODE;                \
                }                                                     \

        #include "../cmd.h"

        #undef DEF_CMD

        if (!label)
            field->pc++;
    }

    return NO_HLT_ERROR;
}

int PrintHeader(struct AsmField *field)
{
    ASM_ERROR_CHECK(field == NULL, PTR_NULL);

    *((char*)(field->code_buffer - HEADER_SIZE))     = SIGNATURE[0];
    *((char*)(field->code_buffer - HEADER_SIZE) + 1) = SIGNATURE[1];
    *(field->code_buffer - HEADER_SIZE + 1) = ASM_VERSION;
    *(field->code_buffer - HEADER_SIZE + 2) = field->pc;

    return SUCCESS;
}

int ProcessLabel(struct AsmField *field, char *cmd)
{
    int arg = 0;
    int cmd_len = strlen(cmd);

    if (isdigit(cmd[0]))
    {
        sscanf(cmd, "%d", &arg);
        ASM_ERROR_CHECK(arg < 0 || arg >= LABELS_AMOUNT, SYNTAX_ERROR);
        ASM_ERROR_CHECK(cmd[1] != ':', SYNTAX_ERROR);

        field->labels[arg].value = field->pc;
    }

    else
    {
        ASM_ERROR_CHECK(cmd[cmd_len - 1] != ':', SYNTAX_ERROR);
        cmd[cmd_len - 1] = '\0';

        for (int i = 0; i <= LABELS_AMOUNT; i++) //find free label
        {
            ASM_ERROR_CHECK(i == LABELS_AMOUNT, LABEL_OVERFLOW_ERROR);

            if (!stricmp(field->labels[i].name, cmd))
            {
                field->labels[i].value = field->pc;

                break;
            }

            if (field->labels[i].value == LABEL_VALUE_POISON
                && !strcmp(field->labels[i].name, "EMPTY_LABEL"))
            {
                strcpy(field->labels[i].name, cmd);

                field->labels[i].name[cmd_len] = '\0';

                field->labels[i].value = field->pc;

                break;
            }
        }
    }

    return SUCCESS;
}

int CountSize(const char *file_name, size_t *file_size)
{
    ASM_ERROR_CHECK(file_name == NULL, PTR_NULL);

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
    ASM_ERROR_CHECK(field == NULL, PTR_NULL);

    size_t lines_count = 0;
    char *ch = field->char_buffer;


    for (int i = 0; i < field->chars_count; i++)
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
    char reg[MAX_WORD_LEN] = {};

    int  arg  = 0;
    int  sign = 1;

    bool label   = false;
    bool plus_ex    = false;
    bool memory = false;

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
        plus_ex = true;
        field->code_buffer[field->pc] |= REGISTER_CODE;
        field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;
    }

    if (label_ptr)
        label = true;

    ASM_ERROR_CHECK(label && plus_ex, SYNTAX_ERROR);

    if (plus_ex)
    {
        sscanf(*buf, "%d", &arg);

        field->code_buffer[++(field->pc)] = arg;

        SKIP_DIGIT();

        ASM_ERROR_CHECK(**buf != '+', SYNTAX_ERROR);

        (*buf)++;

        sscanf(*buf, "%s", reg);

        ASM_ERROR_CHECK(reg[0] != 'r' || reg[2] != 'x', SYNTAX_ERROR);
        ASM_ERROR_CHECK(reg[1]  < 'a' || reg[1]  > 'd', SYNTAX_ERROR);

        arg = reg[1] - 'a' + 1;

        field->code_buffer[++(field->pc)] = arg;

        (*buf) += REGISTER_LENGTH;
    }

    //read label
    else if (label)
    {
        int read_label_err = ReadLabel(field, buf);
        ASM_ERROR_CHECK(read_label_err, READ_LABEL_ERROR);
    }

    //read register
    else if (**buf == 'r')
    {
        field->code_buffer[field->pc] |= REGISTER_CODE;

        ASM_ERROR_CHECK(*(*buf + 1) < 'a' || *(*buf + 1) > 'd', SYNTAX_ERROR);
        ASM_ERROR_CHECK(*(*buf + 2) != 'x', SYNTAX_ERROR);
        arg = *(*buf+1) - 'a' + 1;

        field->code_buffer[++field->pc] = arg;

        (*buf) += 3; //register_length
    }

    //read digit
    else if (isdigit(**buf) || **buf == '-')
    {
        field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;

        if (**buf == '-')
        {
            sign = -1;
            (*buf)++;
        }

        sscanf(*buf, "%d", &arg);

        arg *= sign;

        field->code_buffer[++field->pc] = arg;

        SKIP_DIGIT();
    }

    else
        return SYNTAX_ERROR;

    if (memory)
    {
        ASM_ERROR_CHECK(**buf != ']', SYNTAX_ERROR);
        (*buf)++;
    }

    return SUCCESS;
}

int ReadLabel(struct AsmField *field, char **buf)
{
    int arg = 0;
    char label_name[MAX_WORD_LEN] = {};

    ASM_ERROR_CHECK(**buf != ':', SYNTAX_ERROR);

    field->code_buffer[field->pc] |= IMMEDIATE_CONST_CODE;

    (*buf)++;

    if (isdigit(**buf))
    {
        sscanf(*buf, "%d", &arg);

        ASM_ERROR_CHECK(arg < 0 || arg >= LABELS_AMOUNT, SYNTAX_ERROR);

        //printf("arg = %d, file: %s, line: %d \n", field->labels[arg].value, __FILE__, __LINE__);        //debug
        field->code_buffer[++(field->pc)] = field->labels[arg].value;
        (*buf)++;

        ASM_ERROR_CHECK(isdigit(**buf), SYNTAX_ERROR);
    }

    else
    {
        sscanf(*buf, "%s", label_name);

        int check_err = FindLabelValue(field, label_name, &arg);

        ASM_ERROR_CHECK(check_err, FIND_LABEL_VALUE_ERROR);

        //printf("arg = %d, file: %s, line: %d\n", arg, __FILE__, __LINE__);
        field->code_buffer[++(field->pc)] = arg;
        (*buf) += strlen(label_name);
    }

    return SUCCESS;
}

int FindLabelValue(struct AsmField *field, char *label_name, int *arg)
{
    for (unsigned i = 0; i <= LABELS_AMOUNT; i++)
    {
        if (i == LABELS_AMOUNT)
            *arg = LABEL_VALUE_POISON;

        if(!strcmp(field->labels[i].name, label_name))
        {
            *arg = field->labels[i].value;

            break;
        }
    }

    return SUCCESS;
}

int InitializeLabels(struct AsmField *field)
{
    const char *label_poison_name = "EMPTY_LABEL";
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
    ASM_ERROR_CHECK(           field == NULL, PTR_NULL);
    ASM_ERROR_CHECK(output_file_name == NULL, PTR_NULL);

    FILE *output_file = fopen(output_file_name, "w");

    fwrite(field->code_buffer - HEADER_SIZE, field->pc + HEADER_SIZE, sizeof(int), output_file);

    fclose(output_file);

    printf("\nCODE WRITED SUCCESSFULLY\n");

    return SUCCESS;
}

int DumpCode(struct AsmField *field)
{
    ASM_ERROR_CHECK(field == NULL, PTR_NULL);
    char *head = (char*)(field->code_buffer - HEADER_SIZE);

    //print Header
    printf("\nHeader - %c%c, %d, %d\n\n", *head, *(head + 1), *((int*)head + 1), *((int*)head + 2));

    int *code = field->code_buffer;

    //print Code
    printf("Code: \n");
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
