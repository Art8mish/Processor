
#include "cpu.h"

static const char * const LOG_FILE_NAME     = "../io/cpu_out.txt";


int ReadCode(const char *code_file_name, struct CpuField *field)
{
    ERROR_CHECK(code_file_name == NULL, PTR_NULL);
    ERROR_CHECK(         field == NULL, PTR_NULL);

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
    ERROR_CHECK(input_file == NULL, PTR_NULL);
    ERROR_CHECK(     field == NULL, PTR_NULL);

    int  header[HEADER_SIZE] = {};

    fread(header, sizeof(int), HEADER_SIZE, input_file);
    int fread_err_check = ferror(input_file);

    ERROR_CHECK(fread_err_check, FREAD_ERROR);

    ERROR_CHECK(strncmp((char*)header, SIGNATURE, 2), WRONG_SIGNATURE_ERROR);

    int version  = header[1];

    ERROR_CHECK(version != CPU_VERSION, WRONG_VERSION_ERROR);

    field->op_count = header[2];

    //printf("Version: %d; field.op_count = %d\n", version, field->op_count);

    field->code_buffer = (int*) calloc(field->op_count, sizeof(int));

    ERROR_CHECK(field->code_buffer == NULL, CALLOC_ERROR);

    return SUCCESS;
}
