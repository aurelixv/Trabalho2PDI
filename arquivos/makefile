all:
	gcc -o programa main.c base.c cores.c desenho.c geometria.c imagem.c segmenta.c -lm

run:
	gcc -o programa main.c base.c cores.c desenho.c geometria.c imagem.c segmenta.c -lm
	clear
	./programa

debug:
	gcc -g main.c base.c cores.c desenho.c geometria.c imagem.c segmenta.c -lm -Wall
	gdb a.out
