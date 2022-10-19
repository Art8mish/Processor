CC = g++

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	   \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				   \

all : standart_compilation.exe

standart_compilation.exe : main.o cpu_functions.o io.o
	$(CC) $(CFLAGS) main.o -o standart_compilation.exe

main.o : C:\CBProjects\Processor\cpu\main.cpp
	$(CC) $(CFLAGS) -c C:\CBProjects\Processor\cpu\main.cpp

cpu_functions.o : C:\CBProjects\Processor\cpu\cpu_functions.cpp
	$(CC) $(CFLAGS) -c C:\CBProjects\Processor\cpu\cpu_functions.cpp

io.o : C:\CBProjects\Processor\cpu\io.cpp
	$(CC) $(CFLAGS) -c C:\CBProjects\Processor\cpu\io.cpp


clean:
	rm *.o standart_compilation.exe
