		section         .text

                global          _start
_start:

                sub             rsp, 6*num_len
		lea		r8, [rsp+3*num_len];r8 for rdi temp
		lea		r9, [rsp+4*num_len];r9 for answer
		lea		r12, [rsp+5*num_len];r12 for rsi temp
                mov             rcx, qword_kol
                lea		rdi, [rsp+2*num_len] 
                call            read_long
		lea		rdi, [rsp+num_len]
                call            read_long
		lea		rsi, [rsp+2*num_len]
		mov		rdi, r9
		call		set_zero	;to initialize answer with 0
		mov		rdi, rsp
		call		set_zero	;to initialize space for shifting with 0
		lea		rdi, [rsp+num_len]	
		call 		mul_long_long
		mov		rdi, r9
                call            write_long
		mov		al, 0x0a
		call 		write_char
                jmp             exit


;multiplies 2 long numbers
;	rdi -- address of multiplier 1
;	rsi -- address of multiplier 2
;	rcx -- length of long numbers in qwords
;result:
;	product is written ro rdi
mul_long_long:
		push		rdi
		push		rsi
		mov		r10, rcx ;let r10 save rcx
		call 		copy_to_r8	
		call		copy_to_r12
		xor		r11, r11 ; r11 is number of current block		
		clc
.loop:	
		call		copy_to_rdi
		mov		rbx, [r12+r11]
		call		mul_long_short	
		sub		rdi, r11
		call		copy_to_rsi
		call		add_long_long
		call		copy_to_r9
		call		set_zero
		add		rdi, r11		


		dec		r10
		add		r11, 8
		test		r10, r10
		jnz		.loop

		pop		rsi
		pop 		rdi


		ret


;copies array from rdi to r8
;	rcx -- length of array
copy_to_r8:
		push 		rdi
		push		rcx
		push		r8
		clc
.loop:
		mov		rax, [rdi]
		mov		[r8], rax
		add		rdi, 8
		add		r8, 8
		dec		rcx
		jnz		.loop
		pop r8
		pop rcx
		pop rdi
		ret

;copies array from r8 to rdi
;	rcx -- length of array
copy_to_rdi:
		push		rdi
		push		rcx
		push 		r8
		clc
.loop:
		mov		rax, [r8]
		mov		[rdi], rax
		add		rdi, 8
		add		r8, 8
		dec		rcx
		jnz		.loop
		pop r8
		pop rcx
		pop rdi
		ret



;copies array from rdi to r9
;	rcx -- length of array
copy_to_r9:
		push 		rdi
		push		rcx
		push		r9
		clc
.loop:
		mov		rax, [rdi]
		mov		[r9], rax
		add		rdi, 8
		add		r9, 8
		dec		rcx
		jnz		.loop
		pop r9
		pop rcx
		pop rdi
		ret

;copies array from r9 to rsi
;	rcx -- length of array
copy_to_rsi:
		push		rsi
		push		rcx
		push 		r9
		clc
.loop:
		mov		rax, [r9]
		mov		[rsi], rax
		add		rsi, 8
		add		r9, 8
		dec		rcx
		jnz		.loop
		pop r9
		pop rcx
		pop rsi
		ret

;copies array from rsi to r12
;	rcx -- length of array
copy_to_r12:
		push 		rsi
		push		rcx
		push		r12
		clc
.loop:
		mov		rax, [rsi]
		mov		[r12], rax
		add		rsi, 8
		add		r12, 8
		dec		rcx
		jnz		.loop
		pop r12
		pop rcx
		pop rsi
		ret

	

; adds two long number
;    rdi -- address of summand #1 (long number)
;    rsi -- address of summand #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to rdi
add_long_long:
                push            rdi
                push            rsi
                push            rcx

                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rsi
                pop             rdi
                ret

; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
		push 		rsi
                push            rax
                push            rdi
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rdi
                pop             rax
		pop		rsi
                ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
qword_kol:	equ		256
num_len:	equ		qword_kol*8
