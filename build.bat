del /Q .\bin\*.*
nasm -o .\bin\begin.bin begin.asm
nasm -o .\bin\haribote.o haribote.asm
nasm -f aout -o .\bin\kernelFunc kernelfunc.asm
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\bootpack bootpack.c 
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\gdt gdt.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\graphic graphic.c
gcc -Wall    -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\idt idt.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\pic pic.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\mouse mouse.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\fifo fifo.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\kb kb.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\memory memory.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\unity unity.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\s sheet.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\t timer.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\m mtask.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\f file.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\c console.c
gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\w window.c
cd .\bin 
ld -T .\..\link.ld -o a kernelfunc bootpack .\..\hankaku.obj gdt idt graphic pic mouse fifo kb memory unity s t m f c w
copy /B haribote.o+a kernel  



rem del kernelfunc
pause