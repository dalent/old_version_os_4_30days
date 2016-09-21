nasm -f aout -o e.o entry.asm
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o c.o color.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o u.o unity.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o c2.o color2.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o n.o noodle.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o m.o main.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o t.o type.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o i.o iroha.c 
ld -T link.ld -Map help.txt -o main e.o m.o 
ld -T link.ld -Map help.txt -o color2 e.o c2.o
ld -T link.ld -Map help.txt -o color e.o c.o
ld -T link.ld -Map help.txt -o noodle e.o n.o u.o
ld -T link.ld -o type e.o t.o
ld -T link.ld -o iroha e.o i.o
del *.o
pause