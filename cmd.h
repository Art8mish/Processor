
DEF_CMD(HLT, 0, 0,
{
    return 0;
}, FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(PUSH, 1 << 0, 1,
{
    StackPush(&field->stk, value);
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(POP,  1 << 1, 2,
{
    //txSleep(0.5);
    StackPop (&field->stk, &second_value);
    *pt_val = second_value;
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(JMP,  1 << 2, 1,
{
    field->pc = value - 1;
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(JB,   1 << 3, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value < second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(JBE,  1 << 4, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value <= second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(JA,   1 << 5, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value > second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(JAE,  1 << 6, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value >= second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(JE,   1 << 7, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value == second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(JNE,  1 << 8, 1,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    if (first_value != second_value)
        field->pc = value - 1;
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(CALL, 1 << 9, 1,
{
    StackPush(&field->ret_adr, field->pc + 1);
    field->pc = value - 1;
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(ADD, 3, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    StackPush(&field->stk,  first_value + second_value);
},  FILE_ERROR_CHECK(false, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(SUB, 5, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    StackPush(&field->stk,  first_value - second_value);
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(MUL, 6, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    StackPush(&field->stk,  first_value * second_value);
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(DIV, 7, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    FILE_ERROR_CHECK(second_value == 0, DIVISION_BY_ZERO_ERROR, log_file);
    StackPush(&field->stk,  first_value / second_value);
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(SQRT, 9, 0,
{
    StackPop (&field->stk, &second_value);
    StackPush(&field->stk,  (int)sqrt(second_value));
},  FILE_ERROR_CHECK(field->stk.data_size < 1, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(DUMP, 10, 0,
{
    DumpProcessor(field);
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(OUT, 11, 0,
{
    StackPop (&field->stk, &second_value);
    printf("out: %d\n", second_value);
},  FILE_ERROR_CHECK(field->stk.data_size < 1, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(IN, 12, 0,
{
    printf("Enter a number: ");
    scanf("%d", &value);
    StackPush(&field->stk, value);
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(RAM, 13, 0,
{
    //print RAM
    for (int i = 0; i < RAM_SIZE; i++)
    {
        if ((i > 0) && (i % 100 == 0))
            printf("\n");
        printf("%c ", field->Ram[i]);
    }

    printf("\n");
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(RET, 14, 0,
{
    StackPop (&field->ret_adr, &second_value);
    field->pc = second_value - 1;
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(RSIN, 15, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    StackPush(&field->stk, (int)(first_value * sin(second_value * PI / 180)));
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)

DEF_CMD(RCOS, 17, 0,
{
    StackPop (&field->stk, &second_value);
    StackPop (&field->stk, &first_value);
    StackPush(&field->stk, (int)(first_value * cos(second_value * PI / 180)));
},  FILE_ERROR_CHECK(field->stk.data_size < 2, INCORRECT_USER_CODE_ERROR, log_file);)


DEF_CMD(CIRC, 18, 0,
{
    StackPop (&field->stk, &second_value);
    int R = second_value;
    for (double i = 0; i < PI * 2; i += 0.01)
        field->Ram[100 * (50 + (int)(R * sin(i))) + (50 - (int)(R * cos(i)))] = 64;
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)

DEF_CMD(SLP, 19, 0,
{
    Sleep(1000);
},  FILE_ERROR_CHECK(false, EMPTY_ERROR, log_file);)
