nasm -f aout -o e.o entry.asm
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o m.o noodle.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o u.o unity.c 
ld -T link.ld -Map help.txt -o test e.o m.o u.o
del *.o
pause