	mov edx,2
	mov ebx,msg
	int 0x40
	retf
msg:
	db "hello",0