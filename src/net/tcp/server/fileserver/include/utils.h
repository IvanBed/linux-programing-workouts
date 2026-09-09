#ifndef UTILS_H

#define UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum Operation 
{
    GET,
    POST,
    UNDEFINED
};

char **parse_request(char *str, size_t *out_token_cnt);
void free_args_list(char **args_list, size_t size);
enum Operation get_op_type(char *const request);

#endif

