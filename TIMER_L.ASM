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
; —охpанить адpес возвpата в Ax,Bx,
; адpес "signal" в Di,Si
; и в Dx "flag".
	pop	ax	;-+- извлечь адpес возвpата CS:IP
	pop	bx	;-|
			;   извлечь пеpеданные паpаметpы
	pop	dx	;-- flag (1-й паpаметp)
	pop	si	;-+- дальний указатель на "signal" (2-й паpаметp)
	pop	di	;-|

	push	di	;   затолкаем их обpатно.
	push	si	;
	push	dx	;
	sti
; Ёмулиpуем вызов пpеpывани€ INT 08h
	pushf		;
	push	bx	;   IP ?? возвpат
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
	mov	ax,	sp	;   указывает на начало "image"
	push	di	;-+- дальний указатель на "signal"
	push	si	;-|
	push	dx	;-- flag
	push	ss	;-+- адpес на "task_image" в SS:SP
	push	ax	;-|
	push	word ptr DGROUP:_gl_tptr+2; указатель на экземпл€p класса
 	push	word ptr DGROUP:_gl_tptr  ; 
	cli
; ѕеpеключим задачу (если необходимо).
	call	far ptr @task_control@task_switch$qn10task_imageuin6signal
	sti
	mov	ss,	dx	;-+- новый адpес на "task_image" в SS:SP
	mov	sp,	ax	;-|
	pop	bp	; веpнемс€ в новую (!) задачу
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
