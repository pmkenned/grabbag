.global _start

.text

foo:
	pushq	%rbp
	movq	%rsp, %rbp

    movq $1, %rax         # write(
    movq $1, %rdi         #   STDOUT_FILENO,
    movq $msg, %rsi       #   "Hello, world!\n",
    movq $msglen, %rdx    #   sizeof("Hello, world!\n")
    syscall               # );

	popq	%rbp
    ret

_start:

    call foo

    movq $60, %rax        # exit(
    movq $0, %rdi         #   EXIT_SUCCESS
    syscall               # );

.section .rodata
msg:
    .ascii "Hello, world!\n"
    .set msglen, .-msg
