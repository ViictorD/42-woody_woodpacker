section .text
	global encrypt_me

; void (unsigned char* src_dest, char *key)
;                         |             |
;                         v             v
;                        rdi           rsi

encrypt_me:
	movdqu		xmm5, [rsi] ; mettre la cle
	movdqu		xmm0, [rsi] ; mettre la cle
	call		init_key
	
	movdqu		xmm0, [rdi] ; recp la partie a convertir
	pxor		xmm0, xmm5
	aesenc		xmm0, xmm6
	aesenc		xmm0, xmm7
	aesenc		xmm0, xmm8
	aesenc		xmm0, xmm9
	aesenc		xmm0, xmm10
	aesenc		xmm0, xmm11
	aesenc		xmm0, xmm12
	aesenc		xmm0, xmm13
	aesenc		xmm0, xmm14
	aesenclast	xmm0, xmm15

	movdqu		[rdi], xmm0
	ret

init_key:
	pxor			xmm2, xmm2

	aeskeygenassist	xmm1, xmm0, 0x1
	call			key_combine
	movaps			xmm6, xmm0

	aeskeygenassist	xmm1, xmm0, 0x2
	call			key_combine
	movaps			xmm7, xmm0

	aeskeygenassist	xmm1, xmm0, 0x4
	call			key_combine
	movaps			xmm8, xmm0

	aeskeygenassist	xmm1, xmm0, 0x8
	call			key_combine
	movaps			xmm9, xmm0

	aeskeygenassist	xmm1, xmm0, 0x10
	call			key_combine
	movaps			xmm10, xmm0

	aeskeygenassist	xmm1, xmm0, 0x20
	call			key_combine
	movaps			xmm11, xmm0

	aeskeygenassist	xmm1, xmm0, 0x40
	call			key_combine
	movaps			xmm12, xmm0

	aeskeygenassist	xmm1, xmm0, 0x80
	call			key_combine
	movaps			xmm13, xmm0

	aeskeygenassist	xmm1, xmm0, 0x1B
	call			key_combine
	movaps			xmm14, xmm0

	aeskeygenassist	xmm1, xmm0, 0x36
	call			key_combine
	movaps			xmm15, xmm0

key_combine:
	pshufd xmm1, xmm1, 0xff
	shufps xmm2, xmm0, 0x10
	pxor   xmm0, xmm2
	shufps xmm2, xmm0, 0x8c
	pxor   xmm0, xmm2
	pxor   xmm0, xmm1
	ret
