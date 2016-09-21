extern _main
	call _main
	mov edx,4
	int 0x40
global _api_end
_api_end:
	mov edx,4
	int 0x40
global _api_putchar:;void api_putchar(int c);
_api_putchar:
	mov edx, 0x1
	mov al, [esp + 4];
	int 0x40
	ret
global _api_putstr
_api_putstr:
	push ebx
	mov edx, 0x2
	mov ebx,[esp + 8]
	int 0x40
	pop ebx
	ret

global _api_openwin
;edx 5
;ebx = ���ڻ�����
;esi = ������x�����ϵĴ�С
;edi = ������y�����ϵĴ�С
;eax ͸��ɫ
;ecx ��������
;eax���ھ����
_api_openwin:
	push edi
	push esi
	push ebx
	mov edx, 5
	mov ebx, [esp + 16]
	mov esi, [esp + 20]
	mov edi, [esp + 24]
	mov eax, [esp + 28]
	mov ecx, [esp + 32]
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret
	
global _api_putstrwin
_api_putstrwin:
	push edi
	push esi
	push ebp
	push ebx
	mov edx,0x6
	mov ebx, [esp + 20]
	mov esi, [esp + 24]
	mov edi, [esp + 28]
	mov eax, [esp + 32]
	mov ecx, [esp + 36]
	mov ebp, [esp + 40]
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret
global _api_boxfilwin
_api_boxfilwin:
	push edi
	push esi
	push ebp
	push ebx
	mov  edx, 0x7
	mov ebx, [esp + 20]
	mov eax, [esp + 24]
	mov ecx, [esp + 28]
	mov esi, [esp + 32]
	mov edi, [esp + 36]
	mov ebp, [esp + 40]
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret
global _api_point
_api_point:
	push edi
	push esi
	push ebx
	mov edx,0xb
	mov ebx,[esp + 16];win
	mov esi,[esp + 20];x
	mov edi,[esp + 24];y
	mov eax,[esp + 28];col
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret
global _api_refreshwin
_api_refreshwin:
	push edi
	push esi
	push ebx
	mov edx, 0xc
	mov ebx, [esp + 16];win
	mov eax, [esp + 20];x0
	mov ecx, [esp + 24];y0
	mov esi, [esp + 28];y1
	mov edi, [esp + 32];y1
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret
global _api_linewin
_api_linewin:
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,13
		MOV		EBX,[ESP+20]	; win
		MOV		EAX,[ESP+24]	; x0
		MOV		ECX,[ESP+28]	; y0
		MOV		ESI,[ESP+32]	; x1
		MOV		EDI,[ESP+36]	; y1
		MOV		EBP,[ESP+40]	; col
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
global _api_closewin
_api_closewin:
		push ebx
		mov edx, 14
		mov ebx,[esp + 8]
		int 0x40
		pop ebx
		ret
global _api_getkey
_api_getkey:
		mov edx, 15
		mov eax,[esp + 4]
		int 0x40
		ret
global _api_alloctimer
_api_alloctimer:
        mov edx , 16
        int 0x40
        ret
global _api_inittimer
_api_inittimer:
        push ebx
        mov edx,17
        mov ebx,[esp + 8]
        mov eax,[esp + 12]
        int 0x40
        pop ebx
        ret
global _api_settimer
_api_settimer:
        push ebx
        mov edx, 18
        mov ebx,[esp + 8]
        mov eax,[esp + 12]
        int 0x40
        pop ebx
        ret
global _api_freetimer
_api_freetimer:
        push ebx
        mov edx, 19
        mov ebx,[esp + 8]
        int 0x40
        pop ebx
        ret
global _api_beep
_api_beep:
		mov edx,20
		mov eax,[esp + 4]   ;tone
		int 0x40
		ret
global _api_fopen
_api_fopen:
		push ebx
		mov edx,21
		mov ebx,[esp+8]
		int 0x40
		pop ebx
		ret
global _api_fclose
_api_fclose:;void api_fclose(int fhandle);
		mov edx,22
		mov eax,[esp + 4]
		int 0x40
		ret
global _api_fseek
_api_fseek:;void api_fseek(int fhandle, int offset, int mode);
		push ebx
		mov edx, 23
		mov eax,[esp + 8];fhandle
		mov ecx,[esp + 16];mode
		mov ebx,[esp + 12];offset
		int 0x40
		ret
global _api_fsize
_api_fsize:;int api_fsize(int fhandle, int mode);
		mov edx, 24
		mov eax,[esp + 4];fhandle
		mov ecx,[esp + 8];mode
		int 0x40
		ret
global _api_fread
_api_fread:;int api_fread(char * buf, int maxsize, int fhandle);
		push ebx
		mov edx,25
		mov eax,[esp + 16];fhandle
		mov ecx,[esp + 12];maxsize
		mov ebx,[esp + 8];buf
		int 0x40
		pop ebx
		ret
global _api_cmdline
_api_cmdline:
		push ebx
		mov edx,26
		mov ecx, [esp + 12]
		mov ebx,[esp + 8]
		int 0x40
		pop ebx
		ret
global _api_getlang
_api_getlang:;int api_getlang(void);
		mov edx,27
		int 0x40
		ret
global _api_initmalloc
_api_initmalloc: ;void api_initmalloc(void)
		ret
global _api_malloc
_api_malloc:
	ret;
global _api_free
_api_free:
	ret;