; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; ����������load��ַ
DSKCAC	EQU		0x00100000		; ������������ݻ����һЩ�ļ���Ϣ
DSKCAC0	EQU		0x00008000		; 


CYLS	EQU		0x0ff0			; 
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 
SCRNX	EQU		0x0ff4			; 
SCRNY	EQU		0x0ff6			; 
VRAM	EQU		0x0ff8			; 
VBEMODE EQU		0x105

		ORG		0xc200			; http://community.osdev.info/?VESA
		
		mov 	ax,0x9000
		mov 	es,ax
		mov		di,0
		mov 	ax,0x4f00
		int 	0x10
		cmp 	ax,0x004f
		jne		scrn320
		
		mov		ax,[es:di+4]
		cmp 	ax,0x0200
		jb		scrn320
		
		mov 	cx,VBEMODE;�����ܲ���ʹ��0x105ģʽ��1024*768��
		mov 	ax,0x4f01
		int 	0x10
		cmp 	ax,0x004f
		jne 	scrn320
		
		cmp		byte [es:di + 0x19],8;��ɫ���������ǲ���8λ��ɫ��
		jne 	scrn320
		cmp 	byte [es:di + 0x1b],4;�����ǲ��ǵ�ɫ��ģʽ
		jne 	scrn320
		mov 	ax,[es:di+0x00];ģʽ���ԣ�bit7�������1�Ͳ��ð죨�����Ļ��Ͳ��ܼ���0x400��
		and		ax,0x0080
		jz		scrn320
		
		mov 	bx,VBEMODE+0x4000;1024*768ģʽ
		mov 	ax,0x4f02
		int 	0x10
		mov 	byte[VMODE],8
		mov 	ax,[es:di+0x12];����x�ֱ���
		mov 	[SCRNX],ax
		mov 	ax,[es:di+0x14]
		mov 	[SCRNY],ax
		mov 	eax,[es:di+0x28]
		mov 	[VRAM],eax
		jmp 	keystatus
scrn320:
		MOV		AL,0x13			; 
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL



		;����pic�ж�
		MOV		AL,0xff
		OUT		0x21,AL
		NOP						; 
		OUT		0xa1,AL

		CLI						; ���ÿ������ж� 


		;����A20�ܽ�
		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

		LGDT	[GDTR0]			; 
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	;��ֹ��ҳ
		OR		EAX,0x00000001	; ��������ģʽ
		MOV		CR0,EAX
		MOV		AX,1*8			;  ��ƫ���� 
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		mov     ax,AX
		MOV		SS,AX

		
		MOV		ESI,bootpack	; �����ǵ���������ص�0x280000
		MOV		EDI,BOTPAK		; 
		MOV		ECX,512*1024/4
		CALL	memcpy
		
		MOV		ESI,0x7c00		;��boot������ص�DSKCAC
		MOV		EDI,DSKCAC		; 
		MOV		ECX,512/4
		CALL	memcpy
		
		MOV		ESI,DSKCAC0+512	; �������Ҳ���ؽ���
		MOV		EDI,DSKCAC+512	; 
		MOV		ECX,0
		MOV		CL,15;BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 
		SUB		ECX,512/4		; 
		CALL	memcpy
		
		MOV		esp,0x000280000 
		
		JMP		dword 16:0x00000000;dword����ģ���ʾ�μ�ת��


waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 
		RET

memcpy:
		cld
		a32 rep movsd
		RET


		ALIGNB	16
GDT0:
		RESB	8				; 
		DW		0xffff,0x0000,0x9200,0x00cf	; ����
		DW		0xffff,0x0000,0x9a28,0x004f	; ��ִ��

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:

