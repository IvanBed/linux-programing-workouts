#include <stdio.h>
#define LINE_SIZE 4096

bool read_line(FILE *fp, char *buf)
{
    size_t buf_index;
    for (buf_index = 0; buf_index < LINE_SIZE; buf_index++) 
    {
        buf[buf_index] = getc(fp);
    
        if (buf[buf_index] == '\n')
        { 
            buf[buf_index] = '\0';
            return true;
        }   
        if (buf[buf_index] == EOT)
        { 
            buf[buf_index] = '\0';
            return false;
        }    
    }
    
    return false;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
	{
	    return 1;
	}
	
	FILE *fp;
	char line[LINE_SIZE];
	
    fp = fopen(argv[1], "r");
	memset(line, 0, LINE_SIZE);
	
	while(read_line(fp, line))
	{
	    puts(line);
		memset(line, 0, LINE_SIZE);
	}
	
	fclose(fp);

    return 0;
}