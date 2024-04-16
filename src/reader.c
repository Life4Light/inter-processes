#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

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
    signal(SIGPIPE, handle_sigpipe);
    signal(SIGCHLD, handle_sigchld);
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    FILE *file = fopen(argv[1], "rb");
    char line[BUFF_LENGTH] = {0};

    size_t bytesRead = fread(line, 1, BUFF_LENGTH, file);
    if (bytesRead == 0)
    {
        printf("File is empty or an error occurred\n");
        exit(1);
    }

    write(STDOUT_FILENO, line, bytesRead);

    exit(0);
}