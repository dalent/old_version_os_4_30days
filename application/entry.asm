extern _main
	call _main
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
;ebx = 窗口缓冲区
;esi = 窗口在x坐标上的大小
;edi = 窗口在y坐标上的大小
;eax 透明色
;ecx 窗口名称
;eax窗口句柄。
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
	