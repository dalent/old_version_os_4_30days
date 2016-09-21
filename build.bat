del /Q .\bin\*.*
nasm -o .\bin\begin.bin begin.asm
nasm -o .\bin\haribote.o haribote.asm
nasm -f aout -o .\bin\kernelFunc kernelfunc.asm
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\bootpack bootpack.c 
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\gdt gdt.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\graphic graphic.c
i586-pc-msdosdjgpp-gcc -Wall    -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\idt idt.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\pic pic.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\mouse mouse.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\fifo fifo.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\kb kb.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\memory memory.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\unity unity.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\s sheet.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\t timer.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\m mtask.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\f file.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\c console.c
i586-pc-msdosdjgpp-gcc -Wall   -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin  -c -o .\bin\w window.c
cd .\bin 
i586-pc-msdosdjgpp-ld -T .\..\link.ld  -o a kernelfunc bootpack .\..\hankaku.obj gdt idt graphic pic mouse fifo kb memory unity s t m f c w
copy /B haribote.o+a kernel  



rem del kernelfunc
pause