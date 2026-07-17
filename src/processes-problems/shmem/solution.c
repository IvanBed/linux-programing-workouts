// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1000 
#define SHM_EL_CNT 100

key_t get_key(char const * key_str)
{
    key_t key;
    char* end_ptr; 
   
    key = strtol(key_str, &end_ptr, 10);
    if (end_ptr && *end_ptr != '\0' )
        return 0;     
    else 
        return key;
}

int main(int argc, char *argv[]) 
{
    if (argc != 3)
    {
        return 1;
    }
    
    int *f_data;
    int *s_data;
    int *res_data;
	
    int f_shmid;
    int s_shmid;
    int res_shmid;
	
	key_t f_shmem_key;
	key_t s_shmem_key;
	key_t res_shmem_key;
	
	res_shmem_key = ftok(argv[0], 1);
    f_shmem_key   = get_key(argv[1]);
    s_shmem_key   = get_key(argv[2]);
    
    if ((f_shmid = shmget(f_shmem_key, SHM_SIZE, 0666 )) == -1) 
	{
        perror("shmget 1");
        exit(1);
    }    
	
    if ((s_shmid = shmget(s_shmem_key, SHM_SIZE, 0666 )) == -1) 
	{
        perror("shmget 2");
        exit(1);
    }   

    if ((res_shmid = shmget(res_shmem_key, SHM_SIZE, 0666  | IPC_CREAT)) == -1) 
	{
        perror("shmget 3");
        exit(1);
    } 	
	
    if ((f_data = shmat(f_shmid, NULL, 0)) == (void *)-1) 
    {
        exit(1);
    }
    
    if ((s_data = shmat(s_shmid, NULL, 0)) == (void *)-1) 
    {
        exit(1); 
    }
	
    if ((res_data = shmat(res_shmid, NULL, 0)) == (void *)-1) 
    {
        exit(1); 
    }	
    size_t i = 0;
	for (; i < SHM_EL_CNT; i++)
	{
	    res_data[i] = f_data[i] + s_data[i];
	}
	
	printf("%d\n", res_shmem_key);
    return 0;
}