gcc -o main main.c
for ((i = 0; i < 100; i++));
    do
        ./main;
        mv meta meta_$i;
    done