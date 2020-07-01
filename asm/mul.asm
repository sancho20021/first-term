        section         .text

        global          _start
_start:

        sub             rsp, 4*num_len  ;let us operate with 4 bloks, each is num_len long
        mov             rcx, qwords_amount  ;input length = qwords_amount

        lea             r8,  [rsp]      ;r8 will be the address of the first number (a), 1st block
        lea             rdi, [r8]       ;read a
        call            read_long
        lea             r9,  [rsp+num_len]  ;r9 will be the address of the second number(b), 2nd block
        lea             rdi, [r9]           ;read b
        call            read_long

        lea             rdi, [rsp+2*num_len];from 3-4th blocks there will be answer

        mov             rcx, qwords_amount
        call            mul_long_long
        lea             rcx, [2*qwords_amount]
        call            write_long
        mov             al, 0x0a
        call            write_char
        jmp             exit


;multiplies 2 long numbers
;   rcx -- length of long numbers in qwords
;    r8 -- address of multiplier 1
;    r9 -- address of multiplier 2

;result:
;    product is written ro rdi
;   length of the product is 2*rcx (in qwords)
mul_long_long:
        push        rbx
        push        r8
        push        r9
        push        r10
        push        r11 ;register for i
        push        r12 ;another register that I use
        push        r13 ;another helpful register)
        push        r14
        push        r15 ;counter

        shl         rcx, 4
        sub         rsp, rcx
        shr         rcx, 4
        mov         r10, rsp
        xor         r11, r11 ;i should be zero in the beginning
        mov         r15, rcx
        shl         rcx, 1    ;init the answer with zero
        call        set_zero
.loop:
        mov         r13, rdi            ;keep rdi safe

        mov         rdi, r10
        call        set_zero
        lea         r14, [r10+r11]    ;shift b
        shr         rcx, 1
        call        copy_from_r9_to_r14

        mov         rbx, [r8+r11]       ;rbx = a[i]
        inc         rcx
        lea         rdi, [r10+r11]
        call        mul_long_short
        dec         rcx
        mov         rsi, r10            ;now from rsi starts array-number (b<<i)*a[i]
        mov         rdi, r13            ;prepare the rdi for the answer
        
        shl         rcx, 1
        call        add_long_long


        add         r11, 8      ; next a[i]
        dec         r15         ;dec counter
        test        r15, r15
        jnz         .loop

        shl     rcx, 3
        add     rsp, rcx
        shr     rcx, 4

        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rbx
        ret

;rcx -- length of array
copy_from_r9_to_r14:
        push         r9
        push        rcx
        push        r14
        clc
.loop:
        mov        rax, [r9]
        mov        [r14], rax
        add        r14, 8
        add        r9, 8
        dec        rcx
        jnz        .loop
        pop        r14
        pop        rcx
        pop        r9
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
                push            rsi
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
                pop             rsi
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
qwords_amount:    equ        128
num_len:    equ        qwords_amount*8
