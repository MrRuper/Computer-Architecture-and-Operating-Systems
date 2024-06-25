; System call constants.
SYS_READ equ 0
SYS_WRITE equ 1
STDOUT equ 1
SEEK_CUR equ 1
SYS_OPEN equ 2
SYS_CLOSE equ 3
SYS_LSEEK equ 8
SYS_EXIT equ 60
 
; Constant describing exit code 1.
END_WITH_ERROR equ 1 
 
; Constant describing the position of file pointer
; in the stack with respect to the rsp.
FILE_POINTER_POSITION equ 16
 
; Constant describing the position of the crc 
; string address in the stack with respect to the rsp.
POL_CRC_POSITION equ 24
 
; Constant describing the correct number of inputs:
; program name, file, crc_poly.
CORRECT_NUMBER_OF_INPUTS equ 3
 
; Constant representing the maximum bufor size,
; which is used everytime to keep data fragment 
; from the file. Here 65540 = 2^{16}.
MAXIMAL_FRAGMENT_SIZE equ 65536
 
; Constant representing the number of bytes
; at the beginning of each fragment in file.
FRAGMENT_BYTE_SIZE equ 2
 
; This constant represents the number of bytes
; given at the end of each fragment, describing 
; the shift from one fragment to the next fragment.
FRAGMENT_SHIFT_SIZE equ 4
 
; Constant representing the uncorrect size
; of crc_poly. NOTE: in real applications
; crc_poly could have maximally 65 bit size 
; (unwritten 1 at the beginning), but we handle it 
; in a proper way in .crc_main_loop.  
INCORRECT_CRC_POLY_SIZE equ 65
 
; Constant representing the end of the string.
END_OF_STRING equ 0
 
; Constant representing the maximum size of the crc_poly.
MAXIMAL_SIZE_OF_CRC_POLY equ 64
 
; Constant representing the shift of 8 bits to the left.  
SHIFT_MAXIMALLY_TO_THE_RIGHT equ 56
 
; Constant representing the number of bits in one byte.
BYTE_SIZE equ 8
 
section .data
    ret_code: db 1
 
section .bss
 
    ; This memory keeps the length of data fragment.
    fragment_length: resw 1 
 
    ; This memory keeps the crc_poly length.
    crc_polynomial_length: resb 1
 
    ; This memory keeps the whole fragment 
    bufor: resb MAXIMAL_FRAGMENT_SIZE
 
    ; Answer represented as an ASCII string
    answer: resb 66
 
section .text
    global _start

; This function reads from the file the precise
; number of bytes and loads them to the given buffer
; (or in general to some memory). It also handles 
; sys_read error. Here arguments are:
; rdi - file descriptor,
; rsi - beginning of the buffer,
; rcx - lenght of the fragment to be read.
; NOTE sys_read needs:
; rdi - file descriptor,
; rsi - *bufor,
; rdx - size read.  
read_data:
.read_loop:
    xor rax, rax ; For sys_read.
    syscall
  
    cmp rax, 0 ; rax holds the number of read bytes.
    jg .valid_read
    jmp close_file
 
.valid_read:
    sub rdx, rax ; Update the number of bytes to be upload.
    jz .read_data_end
    add rsi, rax ; Update index in the buffor.
    jmp .read_loop
 
.read_data_end:
    ret
 
; Here we use:
; ***********
; r9  - holds the crc_poly,
; r8  - holds the file descriptor,
; rdi - holds the file descriptor (temporarily at the
; beginning it holds char* file,
; ***********
_start:
    cmp QWORD [rsp], CORRECT_NUMBER_OF_INPUTS
    jne sys_exit
 
.correct_input:
    mov rdi, QWORD [rsp + FILE_POINTER_POSITION]  ; Keeps the char* file.
 
; Because the crc_polynomial will be automatically
; wrongly converted by assembler, we need to convert it 
; manually. The converted polynomial we put into r9.
; rax will keep for a while the crc_polynomial length.
    xor rax, rax                            
    mov rdx, QWORD [rsp + POL_CRC_POSITION] ; Pointer to crc_poly.
    xor r9, r9     
 
; We go through the crc_poly string and convert it 
; to the normal binary representation. After that loop
; we also have to shift left r9 by the 64 - len(crc_poly). 
convertation_polynomial:
    mov cl, BYTE [rdx + rax] ; Take char on the position rdx + rax.
    cmp cl, END_OF_STRING    ; Compare with the end of string. 
    je end_of_convertation  
    shl r9, 1    
    cmp cl, '0'              ; Compare with char representing 0.
    je after_zero_one_check
    cmp cl, '1'              ; Compare with char representing 1.
    je is_one
    jmp sys_exit             ; If cl is not 0, '0' or '1' return 1.
 
is_one: 
    or r9, 1                     ; last bit is 1.                   
 
after_zero_one_check:
    inc rax ; Update length.
    cmp rax, INCORRECT_CRC_POLY_SIZE ; If rax > 64 then return 1.
    je sys_exit
    jmp convertation_polynomial
 
; Before we finish with convertation, one need to 
; shift r9 by the 64 - length(crc_poly) to match it
; with crc_poly given in the input.
end_of_convertation:
    mov BYTE [crc_polynomial_length], al ; Save length to the memory.  
    mov rcx, MAXIMAL_SIZE_OF_CRC_POLY    
    sub rcx, rax ; Now rdx keeps the shift value.
    shl r9, cl  ; Match r9 to the input.
 
; Open file. After a syscall if everything is ok 
; rax will keep the file descriptor.
    mov rax, SYS_OPEN
    xor rsi, rsi ; rsi = 0 for sys_read.
    syscall
 
; Check the sys_open output.
    test rax, rax
    js sys_exit
 
    ; Here we implement the crc algorithm.
    mov r8, rax ; r8 will remember the file descriptor.
    xor r10, r10 ; r10 holds currently przeprocesowany block.
 
.crc_main_loop:
 
    ; Here we use the read_data function. We need to 
    ; prepare a suitable registers before the call.
    ; To do so we keep in  
    ; rdi - the file descriptor,
    ; rsi - the beginning of the buffor,
    ; rcx - the size of fragment to be read.
    ; Firstly we read the block size.
    mov rdi, r8
    mov rsi, fragment_length
    mov rdx, FRAGMENT_BYTE_SIZE
    call read_data
    ; print "successfully read frag lenght ", rax
    ; print "rax = ", rax
 
    ; Secondly we read the actual data fragment.
    mov rsi, bufor
 
    ; Operations on registers smaller than 32-bits don't 
    ; clear out the upper 32-bits.
    xor rdx, rdx 
    mov dx, WORD [fragment_length]
    add edx, FRAGMENT_SHIFT_SIZE
    call read_data
 
    xor rdx, rdx 
    mov dx, WORD [fragment_length]
 
    ; Now we go through the fragment and find the 1. 
    ; After that we do XOR on this fragment and crc_poly. 
    ; To resolve the problem with holes between fragments we 
    ; always load 1 byte of the processing fragment into "rax"
    ; (in fact to the al) and with shld command we find bit "1". 
    ; The rsi will be the inside_loop counter. 0 <= rsi < rdx := fragment_size.
    xor rsi, rsi 
 
.inside_loop: 
    mov al, BYTE [bufor + rsi] ; Load 1 Byte of fragment to the rax.
    shl rax, SHIFT_MAXIMALLY_TO_THE_RIGHT  ; Shift for the shld in the next step.
    xor rcx, rcx ; The inside_inside loop counter.
 
.inside_inside_loop:
    shld r10, rax, 1      ; Shift two blocks: r10 and rax for searching 1.
    jnc .no_carry_flag 
    xor r10, r9        ; If we found 1, XOR with crc_poly.
 
.no_carry_flag:
    inc rcx ; Update the inside_inside_loop counter.
    cmp rcx, BYTE_SIZE 
    jl .inside_inside_loop
 
    ; Update rsi counter.
    inc rsi
    cmp rsi, rdx
    jl .inside_loop
 
 
    ; Check if this is the last fragment.
    mov eax, DWORD [bufor + rsi]
    add rsi, FRAGMENT_SHIFT_SIZE + FRAGMENT_BYTE_SIZE - 1
    cmp rax, rsi
    je end_of_crc_loop
 
    ; Go to the next fragment. Use lseek.
    mov rsi, rax
    mov rax, SYS_LSEEK
    mov rdx, SEEK_CUR
    syscall 
  
    test rax, rax
    js close_file
    jmp .crc_main_loop
 
end_of_crc_loop:
    xor ecx, ecx
    mov cl, BYTE [crc_polynomial_length]
 
.last_loop:
    shl r10, 1
    jnc .update_length
    xor r10, r9 
 
.update_length:
    loop .last_loop 
 
; Print the answer (n upper bits of r10)
; rcx = loop counter
; rax = answer (buffer) pointer
; dl  = current binary digit in ASCII
    xor ecx, ecx ; clear out the counter
    mov cl, BYTE [crc_polynomial_length]
    mov rax, answer
.next_digit:
 
; dl = ASCII representation of the highest bit in r10
    mov dl, '0'
    shl r10, 1 ; cut off the highest bit in r10
    jnc .is_zero
    inc dl
.is_zero:
    mov BYTE [rax], dl
    inc rax ; move answer buffer pointer
    loop .next_digit
 
    mov BYTE [rax], `\n` ; end with newline
 
; sys_write syscall
    xor edx, edx
    mov eax, SYS_WRITE
    mov edi, STDOUT 
    mov rsi, answer ; buf pointer
    mov dl, BYTE [crc_polynomial_length]
    inc dl 
    syscall
    test eax, eax
    js close_file

    mov BYTE [ret_code], 0 ; Set zero if everything is correct.
 
close_file:
    mov rax, SYS_CLOSE
    mov rdi, r8
    syscall
    test rax, rax
    jz sys_exit
    mov BYTE [ret_code], 1
 
sys_exit:
    mov rax, SYS_EXIT
    xor rdi, rdi
    mov dil, BYTE [ret_code]
    syscall