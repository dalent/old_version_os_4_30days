1.if win7 64bit  please install nasm and cygwin (select the devel/djgpp-binutils gcc-core)
1.if below win7 64bit please install djgpp nasm  first(and modify the build.bat use standard gcc ld)
2. please install winimage 
3.build.bat build it check if success 
4. use winimage add new 1.44M image set the image/boot sector properties to bin/bootload.bin
5.use winimage image/inject to inject the bin/kernel save to fg: a.ima
6. use virtualbox to add new linux 32 system , add the new floppy disk a.ima
7.start it 


explain:

0. begin.asm is the bootloader
1. haribote is the bios to 32bit system.(16 bit real mode to protect mode)
2.kernelFunc.asm is our assembly entry and a lot of useful function
3. bootpack.c is our main entry.(i used hello)
4. others is like the file name.
5. 4.ima is a example , start it with virtual box and type "dir" , then type  "start invader"
