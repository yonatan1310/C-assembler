assembler: assembler.o commas.o definitions.o files.o first_pass.o labels.o parse.o operations.o second_pass.o word.o
	gcc -g -ansi -Wall -pedantic assembler.o commas.o definitions.o files.o first_pass.o labels.o parse.o operations.o second_pass.o word.o -o assembler -lm

assembler.o: assembler.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic assembler.c -o assembler.o

commas.o: commas.c assembler.h
	gcc -c -g -ansi -Wall -pedantic commas.c -o commas.o

definitions.o: definitions.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic definitions.c -o definitions.o -lm

files.o: files.c assembler.h
	gcc -c -g -ansi -Wall -pedantic files.c -o files.o

first_pass.o: first_pass.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic first_pass.c -o first_pass.o

labels.o: labels.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic labels.c -o labels.o

operations.o: operations.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic operations.c -o operations.o

parse.o: parse.c assembler.h
	gcc -c -g -ansi -Wall -pedantic parse.c -o parse.o

second_pass.o: second_pass.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic second_pass.c -o second_pass.o

word.o: word.c assembler.h ops_and_defs.h
	gcc -c -g -ansi -Wall -pedantic word.c -o word.o
