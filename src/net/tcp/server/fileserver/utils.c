static size_t get_tokens_cnt(char const *str, size_t len) 
{
    size_t cnt = 1;
    for (size_t i = 0 ;i < len; i++)
        if (str[i] == ':')
            cnt++;
    return cnt;
}

static void add_elemnt(char **tokens, char *str, size_t tokens_indx, size_t start_pos, size_t end_pos)
{
    size_t token_size = end_pos - start_pos;
    tokens[tokens_indx] = malloc(token_size);
    memcpy(tokens[tokens_indx], str + start_pos, token_size);
    tokens[tokens_indx][token_size] = 0;
    if (tokens[tokens_indx][token_size - 1]  == '\n') 
    {
        tokens[tokens_indx][token_size - 1] = 0;
    }
}

enum Operation get_op_type(char *const request) 
{
    if (strncmp(request, "GET", 3) == 0) 
        return GET;
    if (strncmp(request, "GET", 3) == 0) 
        return POST;
    return UNDEFINED;
}

void start_daemon()
{
    pid_t pid = fork();

    if (pid < 0)
    {
       exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0)
    {
       exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    pid = getpid();
    printf("[%d]\n", pid);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

char **parse_request(char *str, size_t *out_token_cnt)
{
    size_t token_size;
    int    str_len = strlen(str); 
    size_t tokens_cnt = get_tokens_cnt(str, (size_t)str_len);
    
    char **tokens = malloc(sizeof(char*) * tokens_cnt);
    for (size_t i = 0; i < tokens_cnt;  i++)
    {
        tokens[i] = 0;
    }
    size_t start_pos = 0;
    size_t tokens_indx = 0;
    size_t end_pos = 0;
    for (; end_pos < str_len; end_pos++) 
    {
        if (str[end_pos] == ':' && tokens_indx < tokens_cnt)
        {
            add_elemnt(tokens, str, tokens_indx, start_pos, end_pos);
            start_pos = end_pos + 1;
            tokens_indx++;
        }
    }
    add_elemnt(tokens, str, tokens_indx, start_pos, end_pos);
    
    *out_token_cnt = tokens_cnt;
    return tokens;
}

void free_args_list(char **args_list, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        free(args_list[i]);
    }
    free(args_list);
}