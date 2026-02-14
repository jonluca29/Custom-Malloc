

debugProgram: myMalloc.c
	gcc -g myMalloc.c -o debugProgram

myMalloc: myMalloc.c
	gcc myMalloc.c -o myMalloc

clean:
	rm -f debugProgram myMalloc

