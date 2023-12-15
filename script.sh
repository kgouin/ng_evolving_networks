rm template
rm meta
rm extra
rm stats

gcc -o main main.c
gcc -o create_template create_template.c

./create_template
for ((i = 0; i < 100; i++));
    do
        ./main;
        sleep 1;
    done

cat template meta >> stats