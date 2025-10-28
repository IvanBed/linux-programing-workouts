#include <unistd.h>
using namespace std;

int myget_pid()
{
    int res = 0;
    asm(
        "mov $22, %%rdi \n"
        "mov $39, %%rax \n"
        "syscall"
        : "=a" (res) 
    );
    return res;   
}
