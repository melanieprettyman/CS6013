extern puts

section .rodata ; this is the read only data (hello is a constant) 
helloString: db "hello", 0 ; hellostring is the name of our symbol

section .text 

global _sayHello
global _myPuts
global _myGTOD

_sayHello:          
push rbp             ; push the frame pointer onto the stack
    mov  rdi, helloString  ; load the address of helloString into rdi
    call puts             ; call the puts function
    pop  rbp              ; pop the frame pointer off the stack
    ret

_myPuts:
 mov rax, 1 ;put the system call number for write in the rax register
 mov rdx, rsi ; grabbing the size param (from rsi), storing it in rdx 
 mov rsi, rdi  ; grabbing parameter char *s (from  rdi), move it into rsi
 mov rdi, 1 ; set rdi with 1, file decriptor for stdout
 syscall ; vals above are passed into syscall instructions to write system call
 ret 
 
_myGTOD:
    sub rsp, 16 ; Make space for the timeval struct on the stack
    mov rsi, 0 ; setting timezone to nullptr
  

    syscall

    mov rax, rsp              ; Load seconds into rax
    mov rdx, [rsp +8]          ; Load microseconds into rdx	
    add rsp, 16
    ret

