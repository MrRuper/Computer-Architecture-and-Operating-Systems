; Constant describing the step in the array int64_t* x.
; 8 here means 8 bytes.
QWORD_STEP equ 8

; Constant which describes the left shifts in that program.
ONE_POSITION_LEFT equ 1

; Constant representing the first bit of the number.
FIRST_BIT equ 1

; Constant representing the second bit of the number.
SECOND_BIT equ 2

; Constant representing the size of binary representation of each number
; in int64_t* x.  
BLOCK_SIZE equ 64

; Constant representing three in binary representation.
FIRST_AND_SECOND_BIT equ 3

; Macro for checking if y is zero.
%macro CHECK_Y_ZERO 0
	cmp rdx, 0
	jne .y_is_not_zero
	div rdx ; Results SIGFPE signal being raised 
%endmacro

global mdiv

section .text

; This fragmen converts the signed integer x to an ansigned integer.
; (x -> -x).
negate_x:
	mov rcx, rsi ; Loop counter.
	xor r11, r11 ; Index of the element in array.
	stc ; Set carry flag for 2'th complement of x.

.check_loop:
	mov r8, QWORD [rdi + QWORD_STEP * r11] 		; Load all block to the r8 (r8 = x[r11]).
	not r8 						; Negate every bit in r8.
	adc r8, 0 					; In 2'th complement we add 1 at the end.
	mov QWORD [rdi + QWORD_STEP * r11], r8 		; Move modified block back to the array x.
	inc r11 					; Increases r11.
	loop .check_loop 				; rcx-- and Jump to .check_loop if rcx != 0. 

	; Before the first jump to negate_x r10b will have 
	; first bit 1 so this line will always send us to mdiv.negate_x_before_division. 
	test r10b, 1				
	jnz mdiv.negate_x_before_division
	
	; After the second jump to negate_x r10b will have
	; first bit zero. We simple return. 
	ret
	
; rdi = x -> array of int64_t representing long number x,
; rsi = n -> length of the array x,
; rdx = y -> divisor
; r9b  -> hold the sign of x / y and sign of x, i.e.
; the first bit is zero if x / y > 0 and is one
; if x / y < 0. In the second bit of r9d we hold 
; the sign of the long number x. 
; Using this we know the sign of the remainder:
; If x > 0 and y > 0 => sign(remainder) > 0,
; if x > 0 and y < 0 => sign(remainder) > 0,
; if x < 0 and y > 0 => sign(remainder) < 0,
; if x < 0 and y < 0 => sign(remainder) < 0. 
mdiv: 
	CHECK_Y_ZERO

.y_is_not_zero:
	xor r9b, r9b 					; Make r9b hold zero.				
	mov cl, byte [rdi + QWORD_STEP * rsi - 1] 	; Take the last byte.
	test cl, cl  					; Raises the SIGN FLAG (SF) if x < 0.	
	jns .check_y 					; If x > 0 take care of the y.
	or r9b, FIRST_AND_SECOND_BIT 			; Sign of x / y in the first bit, sign of x in the second bit.
	or r10, FIRST_BIT				; Prepare r10 for the negate_x function.
	jmp negate_x					; x -> -x.

.negate_x_before_division:

; This block of the code converts -y to the y.
.check_y:
	test rdx, rdx 					; Checks the sign of y.
	jns unsigned_div				; If y > 0 go to the unsigned_div.
	neg rdx						; If y < 0 convert -y -> y.
	xor r9b, FIRST_BIT				; Change the sign of x / y if needed.

; r8 - represents the segment of array x which we divide. 
; r10 - holds the result
; rax - holds the remainder.
; rcx - holds the first loop counter.
; r11 - holds the second loop counter.
; The below code is the implementation of the classic division "dzielenie pod kreską".
unsigned_div:
	mov rcx, rsi 					; .array_loop counter.
	xor eax, eax					; Remainder is zero at the beginning.

.array_loop:
	mov r8, QWORD [rdi + QWORD_STEP * rcx - QWORD_STEP] ; Load the block of x starting from the last one.
	xor r10, r10
	mov r11b, BLOCK_SIZE 				; Nested loop counter.

.bit_loop:
	shld rax, r8, ONE_POSITION_LEFT			; Borrows 1 bit from r8 and puts it to the rax. 
	shl r8, ONE_POSITION_LEFT			; shld does not change r8, thus we also shift r8.
	shl r10, ONE_POSITION_LEFT			; Shift r10 in the left.
	cmp rax, rdx					; As in classical division we compare rax and y.
	jb .nothing_to_subtract				; If rax < y we need one more bit from r8.
	sub rax, rdx					; If rax > y => rax = rax - rdx.
	inc r10 					; If rax > y it means we update the result.

.nothing_to_subtract:
	dec r11b					; Update the .bit_loop counter.
	jnz .bit_loop					; Jump back to the .bit_loop if r11b > 0.
	mov [rdi + QWORD_STEP * rcx - QWORD_STEP], r10  ; After the .bit_loop we move back the result to x.
	loop .array_loop				; Go back to the next block of x.

; Check sign of the remainder and after that check
; for the -INT_MAX / -1 case which occures overlow.
.final_part:
	test r9b, SECOND_BIT				; Check the sign of x.
	jz .positive_remainder				; If x > 0 => remainder > 0.
	neg rax						; If x < 0 => remainder < 0  => rax = -rax.

.positive_remainder:
	test r9b, FIRST_BIT				; Check the sign of x / y.
	jz .check_overflow				; If x / y > 0 we check for the overflow: -INT_MAX / -1.
	
	; If x / y < 0 we have to change sign of x again. Thus r10 has to be zero
	; to correctly return from negate_x.
	xor r10, r10					
	jmp negate_x

; Checks if the x = -INT_MAX and y = -1.
.check_overflow:
	mov cl, byte[rdi + QWORD_STEP * rsi - 1] 	; Take the biggest byte of x.
	test cl, cl					; Look at the sign of x.
	jns .no_overflow				; If the most significant bit is zero then ok, if not => SIGFPE.
	div edx 					; Always returns SIGFPE.
	
.no_overflow:
	ret
