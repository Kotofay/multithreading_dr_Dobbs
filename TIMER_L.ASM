;//--------------------------------------------------------------------------
;//
;//   TIMER.ASM
;//   
;//   Borland C++ v2.00
;//
;//   "Dr.Dobb's journal"#148 February 1989., by Tom Green,
;//   26 May 1993 
;//
;//--------------------------------------------------------------------------
DGROUP	group	_DATA,_BSS
	assume	cs:TIMER_TEXT,ds:DGROUP
_DATA	segment word public 'DATA'
_DATA	ends
_BSS	segment word public 'BSS'
_BSS	ends
;//--------------------------------------------------------------------------
TIMER_TEXT	segment byte public 'CODE'
;//--------------------------------------------------------------------------
	assume	cs:TIMER_TEXT
@timer_handler$qve	proc	far
	cli
	push	ax
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	bp,	DGROUP
	mov	ds,	bp
	pushf
	call	dword ptr DGROUP:_old_timer_handler
	sti
	xor	dx,	dx
	mov	bx,	sp
	push	dx	;-+- pointer to signal      (3 parameter)
	push	dx	;-|
	push	dx	;-| flag = TASK_TIMER_INTR (2 parameter)
	push	ss	;-|
	push	bx	;--- pointer to task_image  (1 parameter)
	push	word ptr DGROUP:_gl_tptr+2
	push	word ptr DGROUP:_gl_tptr
	call	far  ptr @task_control@task_switch$qn10task_imageuin6signal
	mov	ss,	dx
	mov	sp,	ax
	pop	bp
	pop	di
	pop	si
	pop	ds
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	sti
	iret
@timer_handler$qve	endp
;//--------------------------------------------------------------------------
	assume	cs:TIMER_TEXT
@save_image$quin6signal	proc	far
; ���p����� ��p�� ����p��� � Ax,Bx,
; ��p�� "signal" � Di,Si
; � � Dx "flag".
	pop	ax	;-+- ������� ��p�� ����p��� CS:IP
	pop	bx	;-|
			;   ������� ��p������� ��p����p�
	pop	dx	;-- flag (1-� ��p����p)
	pop	si	;-+- ������� ��������� �� "signal" (2-� ��p����p)
	pop	di	;-|

	push	di	;   ��������� �� ��p����.
	push	si	;
	push	dx	;
	sti
; �����p��� ����� �p�p������ INT 08h
	pushf		;
	push	bx	;   IP ?? ����p��
	push	ax	;   CS ?
	push	ax	;
	push	bx
	push	cx
	push	dx
	push	es
	push	ds
	push	si
	push	di
	push	bp
	mov	ax,	sp	;   ��������� �� ������ "image"
	push	di	;-+- ������� ��������� �� "signal"
	push	si	;-|
	push	dx	;-- flag
	push	ss	;-+- ��p�� �� "task_image" � SS:SP
	push	ax	;-|
	push	word ptr DGROUP:_gl_tptr+2; ��������� �� ��������p ������
 	push	word ptr DGROUP:_gl_tptr  ; 
	cli
; ��p������� ������ (���� ����������).
	call	far ptr @task_control@task_switch$qn10task_imageuin6signal
	sti
	mov	ss,	dx	;-+- ����� ��p�� �� "task_image" � SS:SP
	mov	sp,	ax	;-|
	pop	bp	; ��p����� � ����� (!) ������
	pop	di
	pop	si
	pop	ds
	pop	es
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	iret
@save_image$quin6signal	endp
TIMER_TEXT	ends
;//--------------------------------------------------------------------------
	public	@timer_handler$qve
	extrn	_old_timer_handler:dword
	extrn	@task_control@task_switch$qn10task_imageuin6signal:far
	extrn	_gl_tptr:dword
	public	@save_image$quin6signal
;//--------------------------------------------------------------------------
	end
