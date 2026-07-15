#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

static size_t first_sig  = 0;
static size_t second_sig = 0;


void first_signal_handler(int signal_num)
{
    first_sig++;
}

void second_signal_handler(int signal_num)
{
   second_sig++;
}

void term_signal_handler(int signal_num)
{
   printf("%ld %ld\n", first_sig, second_sig); 
   exit(0);
}

int main() 
{
    signal(SIGUSR1, first_signal_handler);
    signal(SIGUSR2, second_signal_handler);
    signal(SIGTERM, term_signal_handler);
    while(1) {}

    return 0;
}
