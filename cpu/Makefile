CC = g++

CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations \
         -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual	   \
         -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                 \
         -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192         \
         -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE				   \

all : standart_compilation.exe

standart_compilation.exe : main.o
	$(CC) $(CFLAGS) main.o -o standart_compilation.exe

main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

#pop_push_functions.o : pop_push_functions.cpp
#	$(CC) $(CFLAGS) -c pop_push_functions.cpp

#verification_functions.o : verification_functions.cpp
#	$(CC) $(CFLAGS) -c verification_functions.cpp

#construct_functions.o : construct_functions.cpp
#	$(CC) $(CFLAGS) -c construct_functions.cpp

clean:
	rm *.o standart_compilation.exe
