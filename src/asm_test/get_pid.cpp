#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    pid_t res_gp = getpid();
    int res = 0;
    asm(
        "mov $22, %%rdi \n"
        "mov $39, %%rax \n"
        "syscall"
        : "=a" (res)
        
    );
    printf("res: %d\n", res);
    printf("res_gp: %d\n", res_gp);
}