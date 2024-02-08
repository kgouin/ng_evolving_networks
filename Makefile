all: template main

template: create_template.c
	gcc -o create_template create_template.c

main: main.c
	gcc -o main main.c

clean:
	rm -f *.o
	rm -f create_template
	rm -f main
