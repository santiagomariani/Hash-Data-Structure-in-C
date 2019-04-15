EXEC = pruebas
OBJFILES = 
CFILES_SUELTOS = main.c testing.c hash.c lista.c pruebas_catedra.c

CC = gcc
CFLAGS = -g -std=c99 -Wall
CFLAGS += -Wconversion -Wno-sign-conversion -Werror -Wbad-function-cast -Wshadow
CFLAGS += -Wpointer-arith -Wunreachable-code -Wformat=2
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes
GDB_FLAGS = -quiet  -ex 'set pagination off' -ex 'set confirm off'

#Compila y linkea
all: $(EXEC)

#Corre	
run: all
	./$(EXEC)

#Valgrindea
valgrind: all
	valgrind $(VFLAGS) ./$(EXEC)

#Corre gdb y printea primeras 30 lineas
gdb: all
	gdb $(GDB_FLAGS) -ex '1,30' -'start' ./$(EXEC) 

#Corre gdb con interfaz
gdb_tui: all
	gdb $(GDB_FLAGS)  -tui ./$(EXEC) 

#Limpia los objetos y el ejecutable
clean:
	rm -f *.o $(EXEC)

#Tiempo de ejecuccion
runtime: all
	time ./$(EXEC)


#Compilador	
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
	
#Linkeador
$(EXEC): $(OBJFILES) $(CFILES_SUELTOS)
	$(CC) $(CFLAGS) $(OBJFILES) $(CFILES_SUELTOS) -o $(EXEC)

