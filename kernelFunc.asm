[bits 32]
hah:
		extern _hello
	    MOV		eAX,1*8			;  段偏移量 
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		SS,AX
		mov esp,0x280000
loo:
		call _hello
	   jmp loo

;往指定的内存写一个字
_write_byte: ;void write_byte(int addr,int color )
		push eax
		push ebx
		mov ebx,[esp+16]
		mov eax,[esp+12]
testloop:
		mov byte[eax],bl
		pop ebx
		pop eax
	   ret
;
;
global _io_hlt
_io_hlt:;void io_hlt(void);
		hlt
		ret
		
global _io_cli		
_io_cli:;void io_cli(void);
		cli
		ret
		
global _io_sti		
_io_sti: ; void io_sti(void);
		sti
		ret

global _io_stihlt		
_io_stihlt:	; void io_stihlt(void);
		sti
		hlt
		ret
	
global _io_in8	
_io_in8:	; int io_in8(int port);
		mov		edx,[esp+4]		; port
		mov		eax,0
		in		al,dx
		ret

global _io_in16			
_io_in16:	; int io_in16(int port);
		mov		edx,[esp+4]		; port
		mov		eax,0
		in		ax,dx
		ret
		
_io_in32:	; int io_in32(int port);
		mov		edx,[esp+4]		; port
		IN		eax,dx
		RET
		
global _io_out8
_io_out8:	; void io_out8(int port, int data);
		mov		EDX,[ESP+4]		; port
		mov		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		mov		EDX,[ESP+4]		; port
		mov		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		mov		EDX,[ESP+4]		; port
		mov		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET
global _io_load_eflags
_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 
		POP		EAX
		RET
global _io_store_eflags
_io_store_eflags:	; void io_store_eflags(int eflags);
		mov		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; 
		ret
global _load_gdtr
extern _gdtp
_load_gdtr:		; void load_gdtr(int addr);
        mov eax,[esp +4]
		lgdt [eax]
		
		jmp dword 0x10:fin;we test if we can jump to  the right place

fin:
        ret

   
 
extern _idtp 
global _load_idtr
_load_idtr:		; void load_gidtr(int addr);
        lidt [_idtp]
        ret

        
global _asm_inthandler21
extern _inthandler21
_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

global _asm_inthandler27
extern _inthandler27
_asm_inthandler27:
			PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

global _asm_inthandler2c
extern _inthandler2c
_asm_inthandler2c:
			PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD
;timer
global _asm_inthandler20
extern _inthandler20
_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

global _asm_inthandler0d
extern _inthandler0d
_asm_inthandler0d:;异常捕获程序
		STI
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler0d
		CMP		EAX,0		; 
		JNE		_asm_end_app		; 
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		ADD		ESP,4			; 
		IRETD
global _asm_inthandler0c:;栈异常
extern _inthandler0c
_asm_inthandler0c:
		sti
		push es
		push ds
		pushad
		mov eax,esp
		push eax
		mov ax, ss
		mov ds, ax
		mov es, ax
		call _inthandler0c
		cmp eax, 0
		jne _asm_end_app
		pop eax
		popad
		pop ds
		pop es
		add esp,4
		iretd
		
global _load_cr0;int load_cr0(void)
_load_cr0:
    mov eax,cr0
    ret
global _store_cr0
_store_cr0:;void store_cr0(int cr0);
    mov eax,[esp+4]
    mov cr0,eax
    ret
global _taskswitch4
_taskswitch4:
		jmp dword 4*8:0
		ret
global _load_tr
_load_tr:
		ltr [esp+4]
		ret
global _taskswitch3
_taskswitch3:
		jmp dword 3*8:0
		ret
global _farjmp;void farjmp(int eip,int cs);
_farjmp:
		jmp far [esp + 4]
		ret
global _farcall
_farcall:
		call far [esp + 4]
		ret
global _asm_cons_putchar
extern _cons_putchar
_asm_cons_putchar:
		sti
		pushad
		push 1
		and eax, 0xff
		push eax
		push dword [0x0fec]
		call _cons_putchar
		add esp, 12
		popad
		iretd
global _asm_hrb_api
extern _hrb_api
_asm_hrb_api:
		STI
		PUSH	DS
		PUSH	ES
		PUSHAD		; 
		PUSHAD		; 
		MOV		AX,SS       ;因为操作系统可以负责ss和esp的回归，所以我们直接用ss来恢复操作系统的ds与es
		MOV		DS,AX		; 
		MOV		ES,AX
		CALL	_hrb_api
		CMP		EAX,0		; 
		JNE		_asm_end_app
		ADD		ESP,32
		POPAD
		POP		ES
		POP		DS
		IRETD
global _asm_end_app
_asm_end_app:
;	
		MOV		ESP,[EAX]
		mov 	dword [eax + 4],0
		POPAD
		RET					; 
global _start_app
_start_app:; void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
		PUSHAD		; 
		MOV		EAX,[ESP+36]	; 
		MOV		ECX,[ESP+40]	; 
		MOV		EDX,[ESP+44]	; 
		MOV		EBX,[ESP+48]	; 
		MOV		EBP,[ESP+52]	; 
		MOV		[EBP  ],ESP		; 
		MOV		[EBP+4],SS		; 
		MOV		ES,BX
		MOV		DS,BX
		MOV		FS,BX
		MOV		GS,BX
;	
		OR		ECX,3			; 
		OR		EBX,3			; 
		PUSH	EBX				; 
		PUSH	EDX				; 
		PUSH	ECX				; cs压入栈
		PUSH	EAX				; eip压入栈
		RETF                    ;操作系统不能call应用程序，只能通过这种方式啦
		