
#include "include/asm.h"

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


