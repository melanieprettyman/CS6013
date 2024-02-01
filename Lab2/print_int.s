;;;Melanie Prettyman
;;;01/29/23

section .rodata ;read only data section
section .text

global print_int
global main

;;void print_int(long x)
;; 1234 => '1' '2' '3' '4'
;; 1234/10 => 123 remainder 4
;; 123/10 => 12 remander 3
;; 12/10 => 1 remainder 2
;; 1/10 => 0 remainder 1
;; 0 <- stop

print_int:
	;;PROLOGUE
	push rbp		 ;save the prev (main) bp so we can go back to it
	mov rbp, rsp		 ; update the bp to your location (in case we call someone)
	sub rsp, 20		 ; subtract from the stack pointer to save 20 bytes (characters) on stack for each digit 
	mov rax, rdi		 ; copy the passed in 1st param (x) into rax reg to do work there
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-code to rev
	mov rbx,20 		 ; create an index to access remainders. BX is the 'iTH' location in digit arra
	mov byte[rsp+rbx], 10	 ; (ascii 10 is '/n') put CR into the 1st location of my digits array 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	;;START ALGORITHM
dividing: 
	cmp rax, 0 		 ;stop condition (end loop)
	je print 		 ; if the above is true, jump equals print

	mov rdx, 0 		 ; initialize rdx to 0 (just have to or it crashes)

	mov rcx, 10		 ; rcx is given the value of 10
	div rcx			 ; rax <= rax / rcx ...remainder is put into rdx
	add rdx, 48		 ; turn the number 4, into the digit '4' (based on asii values)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-code to rev
	dec rbx		         ; index --
	mov [rsp+rbx], dl	 ; access array (rsp at bx) and move value at rdx into it (remainder)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	jmp dividing		 ; start over 
print: 
	mov rax, 1 		; tells OS to use the write fx
	mov rdi, 1 		; l RDI is 1st param which is file descriptor (1 == std out)

	mov rcx, rsp 		;copy the value of rsp to rcx 
	add rcx, rbx  		;array is located at rsp + 1
	mov rsi, rcx 		; 2nd param: addrss of the 1st char in our array of chars
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;-code to 
	mov r9, 21              ;move 21 into register 9
	sub r9, rbx		;to get the size of the array subtract the count from r9
	mov rdx, r9 		;3rd param:  rbx hold the # of digits -put in rdx so write will see it
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
	syscall

	;;EPILOGUE (fix up sp and bp)
	mov rsp, rbp
	pop rbp
	ret



main:
	mov rdi, 1234		 ; declare variables for print_int 
	call print_int		 ;call print_int 	
	mov rax, 60		 ; sys_exit call
	mov rdi, 0		 ; 1st param error code 
	syscall
