nasm -f aout -o e.o entry.asm
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o m.o main.c 
ld -T link.ld -Map help.txt -o test e.o m.o
del *.o
pause