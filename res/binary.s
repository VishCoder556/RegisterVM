section .text
global _main
default rel
extern _putchar
extern _printf
_main:
.prologue
	push rbp
	mov rbp, rsp
	sub rsp, 64
instr_0:
	call instr_2
instr_1:
	jmp main_epilogue
instr_2:
	lea rax, [rel constant_pool]
	add rax, 0
	mov qword [rbp - 8], rax
instr_3:
	mov qword [rbp - 16], 5
instr_4:
	mov rax, rsp
	add rax, -8
	mov rbx, qword [rbp - 16]
	mov qword [rax], rbx
instr_5:
	mov rdi, qword [rbp - 8]
	mov rsi, qword [rbp - 16]
	mov rdx, qword [rbp - 24]
	call _printf
instr_6:
	mov qword [rbp - 8], 0
instr_7:
	jmp main_epilogue
main_epilogue:
	mov rax, 0x2000001
	mov rdi, qword [rbp - 8]
	syscall
section .data
	constant_pool: db 'Hi Guys, %d!', 10, '', 0, ''
