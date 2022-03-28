;==============================================================================
;	Loop $ �� ����� 2128/4257ns �����邩�𒲂ׂ�
;		output	cx	loop��
;			ax	�P���loop $�̑���(ns)
;			bx	cx*ax
;==============================================================================
waittest	proc

	mov	ax,4257		;YM2203 (4257ns)
	cmp	es:[ongen],1
	jnz	wt_notym2608
	mov	ax,2128		;YM2608 (2128ns)
wt_notym2608:
	mov	[regdata_clock],ax

	push	dx
	push	ds
	push	es

;------------------------------------------------------------------------------
;	Timer�ݒ�
;------------------------------------------------------------------------------
	test	byte ptr es:[501h],80h
	jz	tcg_5mhz
	mov	bx,1997;200	;1ms Timer���荞�� (10ms=19968 8MHz�n)
	jmp	tcg_set
tcg_5mhz:
	mov	bx,2458;246	;1ms Timer���荞�� (10ms=24576 5MHz�n)
tcg_set:
	mov	al,36h
	out	77h,al
	mov	cx,10
	loop	$
	mov	al,bl
	out	71h,al
	mov	cx,10
	loop	$
	mov	al,bh
	out	71h,al

	mov	ax,1600h
	int	2fh
	test	al,7fh
	jz	not_windows
	mov	cx,20000		;Timer��ݒ肵�Ă���|����܂�
winwait:out	5fh,al			;�u�����N������悤��
	loop	winwait
not_windows:

;------------------------------------------------------------------------------
;	���݂� Timer�̃x�N�g��/�}�X�N��ۑ�
;------------------------------------------------------------------------------
	cli
	xor	ax,ax
	mov	es,ax
	mov	ax,cs
	mov	ds,ax

	mov	ax,es:[timervector*4]
	mov	[w_timerofs_push],ax
	mov	ax,es:[timervector*4+2]
	mov	[w_timerseg_push],ax
	mov	es:[timervector*4],offset timer_entry
	mov	es:[timervector*4+2],cs

;------------------------------------------------------------------------------
;	Timer�ȊO�̑S�Ă̊��荞�݂��֎~
;------------------------------------------------------------------------------
	in	al,2
	mov	[w_mmsk_push],al
	mov	al,0feh
	out	2,al

;------------------------------------------------------------------------------
;	���胁�C��
;------------------------------------------------------------------------------
	xor	bx,bx		;Timer Counter
	mov	al,20h		;EOI Send�p
	mov	dx,50000
	mov	cx,dx
	even			;
	nop			;
	sti			;�����Ԓn�ɍ��킹��
	loop	$		;
	REPT 19
	mov	cx,dx
	loop	$		;100000�� loop
	ENDM
	cli

;------------------------------------------------------------------------------
;	���荞�݂����ɖ߂�
;------------------------------------------------------------------------------
	mov	al,36h
	out	77h,al
	mov	cx,10
	loop	$
	mov	al,0ffh		;�Œᑬ�ݒ�(1)
	out	71h,al
	mov	cx,10
	loop	$
	out	71h,al		;�Œᑬ�ݒ�(2)

	mov	al,[w_mmsk_push]
	out	2,al

	mov	ax,[w_timerofs_push]
	mov	es:[timervector*4],ax
	mov	ax,[w_timerseg_push]
	mov	es:[timervector*4+2],ax
	sti

;------------------------------------------------------------------------------
;	�����2128ns���z���邩��check
;------------------------------------------------------------------------------
	mov	cx,1
	mov	ax,bx		;ax=1�񕪂�loop $
	cmp	ax,[regdata_clock]
	jnc	waittest_exit	;1��ŏ[��

loopnum_check:
	inc	cx
	add	bx,ax
	cmp	bx,[regdata_clock]
	jc	loopnum_check

;------------------------------------------------------------------------------
;	�I��
;------------------------------------------------------------------------------
waittest_exit:
	pop	es
	pop	ds
	pop	dx
	ret

;==============================================================================
;	timer Entry
;==============================================================================
timer_entry:
	inc	bx
	out	0,al
	iret

;==============================================================================
;	Datas
;==============================================================================
regdata_clock	dw	?
w_timerofs_push	dw	?
w_timerseg_push	dw	?
w_mmsk_push	db	?

timervector	equ	08h		;timer.���荞��

waittest	endp
