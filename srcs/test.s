section .text

decrypt:
	pushf
	push rdi
	push rsi
	push rax
	push rbx
	push rcx
	push rdx
	; Print "....WOODY....."
	mov rax, 1						; syscall write
	mov rdi, rax					; stdout
	lea rsi, [rel str_woody]		; ptr to the string to write
	mov rdx, 16						; length of the string
	syscall
	; lea r8, [rel to_jmp]
	; sub r8, [r8]
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rsi
	pop rdi
	popf
	mov	r8, [rel to_jmp]
	jmp r8

str_woody:	db "....WOODY.....", 10, 0							; Woody string (16 bytes long)
to_jmp:		dq 0x00000000004003e0								; address to jmp (8 bytes long)
key:		db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	; decrypt key (16 bytes long)
