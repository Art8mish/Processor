
#include <windows.h>
#include <stdio.h>

int main()
{
    system("assembler.exe ../io/krujochek.txt");
    system("..\\cpu\\cpu.exe");
    system("assembler.exe ../io/quadratic.txt");
    system("..\\cpu\\cpu.exe");
    system("assembler.exe ../io/factorial.txt");
    system("..\\cpu\\cpu.exe");
}