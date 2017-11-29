CLIOBJ = tokenizer.o CLI.o
CALCOBJ = tokenizer.o calc.o

CLI : $(CLIOBJ)
	gcc -o CLI $(CLIOBJ)

calc : $(CALCOBJ)
	gcc -o calc $(CALCOBJ)

listf : listf.o
	gcc -o listf listf.o

all:
	$(MAKE) CLI calc listf

calc.o : calc.c
	gcc -g -c -Wall calc.c

CLI.o : CLI.c
	gcc -g -c -Wall CLI.c

listf.o : listf.c
	gcc -g -c -Wall listf.c

tokenizer.o : tokenizer.c
	gcc -g -c -Wall tokenizer.c
