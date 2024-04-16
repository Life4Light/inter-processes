#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFF_LENGTH 256
volatile sig_atomic_t status = 0;

void handle_sigpipe(int sig)
{
    status = sig;
}

void handle_sigchld(int sig)
{
    status = sig;
}

int main(int argc, char *argv[])
{
    char result[BUFF_LENGTH];

    signal(SIGPIPE, handle_sigpipe);
    signal(SIGCHLD, handle_sigchld);

    if (argc != 4)
    {
        printf("Usage: %s <file1> <file2> <output>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        close(pipefd1[0]);
        dup2(pipefd1[1], STDOUT_FILENO);
        execl("./reader", "reader", argv[1], NULL);
        _exit(EXIT_SUCCESS);
    }

    waitpid(pid1, NULL, 0);
    pid_t pid2 = fork();
    if (pid2 == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0)
    {
        close(pipefd2[0]);
        dup2(pipefd2[1], STDOUT_FILENO);
        execl("./reader", "reader", argv[2], NULL);
        _exit(EXIT_SUCCESS);
    }

    waitpid(pid2, NULL, 0);

    close(pipefd1[1]);
    close(pipefd2[1]);

    char str1[BUFF_LENGTH], str2[BUFF_LENGTH];
    memset(&str1[0], 0, sizeof(str1));
    memset(&str2[0], 0, sizeof(str2));

    read(pipefd1[0], str1, BUFF_LENGTH);
    read(pipefd2[0], str2, BUFF_LENGTH);

    for (size_t i = 0; i < BUFF_LENGTH; i++)
    {
        result[i] = (str1[i] ^ str2[i]);
    }

    FILE *output_file = fopen(argv[3], "wb");
    if (output_file == NULL)
    {
        printf("Error: Failed to open output file\n");
        exit(1);
    }

    fwrite(result, sizeof(char), BUFF_LENGTH, output_file);
    fclose(output_file);
    if (status == SIGPIPE || status == SIGCHLD)
        puts("Interactive attention signal caught.");

    wait(NULL);
    wait(NULL);
    exit(EXIT_SUCCESS);
}