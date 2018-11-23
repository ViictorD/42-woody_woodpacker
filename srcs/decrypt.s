section .text

; void (unsigned char* dest, unsigned char *src, char *key)
;                        |                   |          |
;                        v                   v          v
;                       rdi                 rsi        rdx

go_decrypt:
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

	mov		rcx, [rel len]
	lea		rbx, [rel crypted_address]
	sub		rbx, [rbx]
	movdqu	xmm5, [rel key]
	movdqu	xmm0, [rel key]
	call	init_key
	call	decrypt_loop
	
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rsi
	pop rdi
	popf
	lea	r8, [rel start_address]
	sub r8, [r8]
	jmp r8

decrypt_loop:
	cmp	rcx, 0
	jle	end

	call	decrypt_aes

	add	rbx, 16		; ptr += 16
	sub	rcx, 16		; i -= 16
	jmp	decrypt_loop

decrypt_aes:
	movdqu		xmm0, [rbx] 		; recp 16 bytes a convertir
	pxor		xmm0, xmm15
	aesdec		xmm0, xmm14
	aesdec		xmm0, xmm13
	aesdec		xmm0, xmm12
	aesdec		xmm0, xmm11
	aesdec		xmm0, xmm10
	aesdec		xmm0, xmm9
	aesdec		xmm0, xmm8
	aesdec		xmm0, xmm7
	aesdec		xmm0, xmm6
	aesdeclast	xmm0, xmm5

	movdqu		[rbx], xmm0
	ret

init_key:
	pxor			xmm2, xmm2

	aeskeygenassist	xmm1, xmm0, 0x1
	call			key_combine
	aesimc			xmm6, xmm0

	aeskeygenassist	xmm1, xmm0, 0x2
	call			key_combine
	aesimc			xmm7, xmm0

	aeskeygenassist	xmm1, xmm0, 0x4
	call			key_combine
	aesimc			xmm8, xmm0

	aeskeygenassist	xmm1, xmm0, 0x8
	call			key_combine
	aesimc			xmm9, xmm0

	aeskeygenassist	xmm1, xmm0, 0x10
	call			key_combine
	aesimc			xmm10, xmm0

	aeskeygenassist	xmm1, xmm0, 0x20
	call			key_combine
	aesimc			xmm11, xmm0

	aeskeygenassist	xmm1, xmm0, 0x40
	call			key_combine
	aesimc			xmm12, xmm0

	aeskeygenassist	xmm1, xmm0, 0x80
	call			key_combine
	aesimc			xmm13, xmm0

	aeskeygenassist	xmm1, xmm0, 0x1b
	call			key_combine
	aesimc			xmm14, xmm0

	aeskeygenassist	xmm1, xmm0, 0x36
	call			key_combine
	movaps			xmm15, xmm0

key_combine:
	pshufd	xmm1, xmm1, 0xff
	shufps	xmm2, xmm0, 0x10
	pxor	xmm0, xmm2
	shufps	xmm2, xmm0, 0x8c
	pxor	xmm0, xmm2
	pxor	xmm0, xmm1
	ret

end:
	ret

str_woody:			db "....WOODY.....", 10, 0							; Woody string (16 bytes long)
start_address:		dq 0x0000000000000000								; address to jmp (8 bytes long)
crypted_address:	dq 0x0000000000000000								; address to decrypt (8 bytes long)
key:				db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	; decrypt key (16 bytes long)
len:				dq 0x0000000000000000								; len to decrypt (8 bytes long)