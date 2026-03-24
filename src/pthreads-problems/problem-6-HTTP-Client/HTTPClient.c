#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h> 

#define BUF_SIZE 512

#define CNT_RETRIES 5
#define MAX_APP_ARGS 3

#define MAX_DIGITS 20
#define NEW_LINE_CHARS_CNT 8
#define NEW_LINE "\r\n\r\n"

#define PROTOCOL_STR "HTTP/1.1\r\n"
#define HOST_STR "Host: "
#define GET_METHOD_STR "GET /"
#define CONN_TYPE_STR "\r\nConnection: close\r\n\r\n"

#define POST_METHOD_STR "POST /"
#define CONTENT_TYPE_TEXT_STR "\r\nContent-Type: text/html; charset=utf-8\r\n"
#define CONTETN_LEN_STR "Content-Length: "

// ERRORS CODES
#define NO_ERR 0
#define EMPTY_STR_ERR 1
#define ALLOC_ERR 2
#define SOCK_ERR_WRITE 3
#define SOCK_ERR_READ 4

// ARGS ERRORS CODES
#define NO_ARGS_ERR 5
#define POST_ARGS_ERR 6
#define true  1
#define false 0


enum request_method_type 
{ 
    GET, POST
};

struct Node 
{
    char* str_val;
    struct Node* next;
    size_t index;
};

static int cpy_str(char **new_str, char *old_str) 
{

    if (old_str == NULL) 
    {
        *new_str = NULL;
        return 0;
    }
   
    size_t str_len = strlen(old_str) + 1;
    *new_str = (char*)malloc(str_len);
   
    if (*new_str == NULL) 
    {
        perror("Memory for the string can not be allocated.");
        return 1;
    }

    memcpy(*new_str, old_str, str_len);
   
    return 0;
    
} 

static int make_node(struct Node **node, char *val) 
{

    *node = (struct Node*) malloc(sizeof(struct Node*));
     
    if (*node == NULL) 
    {
        
        return 1;
    }
    char *new_val = NULL;
        
    if (cpy_str(&new_val, val) != 0) 
    {
        return 1;
    }

    (*node)->str_val = new_val;
    (*node)->next = NULL;
    
}

static int add(struct Node **head, char *val)
{

    struct Node *new_node;
    int res = make_node(&new_node, val);
    if (!res) {
        
        return 1;
    }
    if (*head == NULL) 
    {
        *head = new_node;
        return 0;
    }

    struct Node* cur = *head;

    while (cur->next != NULL)
        cur = cur->next;

    cur->next = new_node;
  
    new_node->index = cur->index + 1;
    
}

static struct Node* get_node(struct Node *list, size_t index) 
{
    struct Node* cur = list;
    while (cur) 
    {
        if (cur->index == index)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void print_list(struct Node *list) 
{
     
    struct Node* cur = list;
    while (cur) 
    {
        printf("val %s index: %ld\n", cur->str_val, cur->index);
        cur = cur->next;
    }
}

int free_list(struct Node *list) 
{
    struct Node* cur = list;
    while (cur) 
    {
        struct Node *temp = cur;
        cur = cur->next;
        free(temp->str_val);
        free(temp);
    }
}

uint8_t empty(char const * str)
{
    if (str == NULL || str == "")
        return true;
    else
        return false;
}

static int parseURL(struct Node **list, char *URLstr) 
{
    char str_buff[2048];
    size_t buff_ptr = 0;
    size_t url_len = strlen(URLstr);
    puts("parseURL");
    memset(str_buff, 0, 2048 * sizeof(char));
    for (size_t i = 0; i < url_len && buff_ptr < 2047; i++) 
    {
     
        if (URLstr[i] == '/' || i + 1 == url_len) 
        {
            if (i + 1 == url_len)
                str_buff[buff_ptr++] = URLstr[i];

            str_buff[++buff_ptr] = '\0';
            puts(str_buff);
            int res_of_add = add(list, str_buff);
            memset(str_buff, 0, buff_ptr * sizeof(char));
            buff_ptr = 0;
        } 
        else 
        {
            str_buff[buff_ptr++] = URLstr[i];
        }
    }
    return 0;
}

static uint8_t check_protocol(struct List *list)
{
    puts(get_node(list, 1)->str_val);
    return true;
}

static int make_domain_name(struct Node* list, char ** domain_name)
{
    struct Node* domain_name_node = get_node(list, 2);
    if (domain_name_node == NULL)
        return 1;
    puts(domain_name_node->str_val);
    *domain_name = domain_name_node->str_val;
    return 0;
}

static int make_resourсe_url(struct Node *list, char **resourсe_url_res, size_t start_pos, size_t end_pos)
{
    size_t total_len = 0;
    size_t offset    = 0;

    struct Node *start = get_node(list, start_pos);
    if (!start)
    {
        return 1;
    }

    while (start)
    {
        total_len += strlen(start->str_val);
        start = start->next;
    }
    
    char *buff = (char*) malloc(total_len + 1);
    
    start = get_node(list, 3);
    while (start)
    {
        strcpy(buff + offset, start->str_val);
        offset += strlen(start->str_val);
        strcpy(buff + offset, "/");
        offset++;
        start = start->next;
    }
    buff[total_len] = '\0';

    *resourсe_url_res = buff;

    return NO_ERR;
}

uint8_t make_get_request(char const *domain_name, char const *resourсe_url, char **get_request)
{
    uint8_t err_code = 0;
    if (empty(domain_name) || empty(resourсe_url))
    {
        return EMPTY_STR_ERR;
    }
    
    size_t total_len =  strlen(domain_name) + strlen(resourсe_url) + strlen(PROTOCOL_STR) + strlen(CONN_TYPE_STR) + strlen(HOST_STR) 
    + strlen(GET_METHOD_STR);

    char *buff = (char*) malloc(total_len + 1);
    if (!buff)
    {
        return ALLOC_ERR;
    }

    strcpy(buff, GET_METHOD_STR);
 
    strcat(buff, resourсe_url);
    strcat(buff, PROTOCOL_STR);
    strcat(buff, HOST_STR);

    strcat(buff, domain_name); 
    strcat(buff, CONN_TYPE_STR);
    
    *get_request = buff;

    return NO_ERR;
}

uint8_t make_content_len_str(size_t body_len, char **content_len)
{
    char body_len_str[MAX_DIGITS + 1];
    snprintf(body_len_str, MAX_DIGITS, "%ld", body_len);
    
    body_len_str[MAX_DIGITS] = '\0';
    
    size_t content_len_str_len    = strlen(CONTETN_LEN_STR) + NEW_LINE_CHARS_CNT + MAX_DIGITS + 1;
    
    char *buff = (char *)malloc(content_len_str_len + 1);
    if (!buff)
    {
        return ALLOC_ERR;
    }
    
    strcpy(buff, CONTETN_LEN_STR);
    strcat(buff, body_len_str);
    strcat(buff, NEW_LINE);
    
    *content_len = buff;
    
    return NO_ERR;
}

uint8_t make_post_request(char const *domain_name, char const *resourсe_url, char const *body, char **post_request)
{
    uint8_t err_code = 0;
    if (empty(domain_name) || empty(resourсe_url))
    {
        return EMPTY_STR_ERR;
    } 
    
    size_t body_len              = strlen(body);
    char *content_len_str        = NULL;
    if ((err_code = make_content_len_str(body_len, &content_len_str)) != 0)
    {
        return err_code;
    }
    
    size_t total_len             =  strlen(domain_name) + strlen(resourсe_url) + strlen(PROTOCOL_STR) + strlen(HOST_STR) 
    + strlen(POST_METHOD_STR) +  strlen(CONTENT_TYPE_TEXT_STR) + strlen(content_len_str) + body_len;
    
    char *buff = (char*) malloc(total_len + 1);
    if (!buff)
    {
        return ALLOC_ERR;
    }
    
    strcpy(buff, POST_METHOD_STR);
    strcat(buff, resourсe_url);
    strcat(buff, PROTOCOL_STR);
    strcat(buff, HOST_STR);
    strcat(buff, domain_name);
    strcat(buff, CONTENT_TYPE_TEXT_STR);
    strcat(buff, content_len_str);
    strcat(buff, body);
    
    if (content_len_str)
        free(content_len_str);
    
    *post_request = buff;

    return NO_ERR;
}

static int get_ip_from_domain(char *domain_name, const char *port, struct addrinfo **result) 
{
    struct addrinfo hints;
    int err;
    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM ; /* TCP socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    err = getaddrinfo(domain_name, port, &hints, result);
    if (err != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }
    
    //freeaddrinfo(&hints);
    return NO_ERR;
}

static int try_connect(struct addrinfo *addr_info, int *sockfd) 
{
    struct addrinfo *rp;
    for (rp = addr_info; rp != NULL; rp = rp->ai_next)
    {
        *sockfd = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);
        if (*sockfd == -1)
            continue;

        if (connect(*sockfd, rp->ai_addr, rp->ai_addrlen) != -1) 
        {
            puts("Success");
            break; 
        }
        close(*sockfd);
    }
    
    if (rp == NULL) 
    {              
        fprintf(stderr, "Could not connect\n");
        return 1;
    }

    return NO_ERR;
}

uint8_t send_request(int sockfd, char *const msg)
{
    size_t len     = strlen(msg);
    size_t retries = 0;
    int bytes_sent = 0;

    while ((bytes_sent = send(sockfd, msg, len, 0)) != len) 
    {
        if (retries >= CNT_RETRIES)
        {
            return SOCK_ERR_WRITE;
        }
        retries++;
    }
    
    printf ("bytes sent \n", bytes_sent);

    return NO_ERR;
}

uint8_t get_response(int sockfd)
{
    size_t len = BUF_SIZE * 8;
    char buff[len];
    puts("response");
    size_t count;
    while ((count = recv(sockfd, (void*) buff, len, 0)) != 0) 
    {
        puts(buff);
        if (count == -1) {
            switch (errno) 
            {
                case EAGAIN:
                    usleep(2000);
                    break;
                default:
                    perror("Failed to read from socket");
                    return SOCK_ERR_READ;
            }
        }
    }

    return NO_ERR;
}


int main(int argc, char **argv) {

    char *URL               = NULL;
    char *body              = NULL;
    enum request_method_type method_type;
     puts("test000000");
    if (argc < 3) 
    {
        printf("NO_ARGS_ERR");
        return NO_ARGS_ERR;
    }
    else
    {
        char *URL = argv[2];
        puts(URL);
        if (strcmp(argv[1], "GET") == 0)
            method_type = GET;
        else 
        {
            method_type = POST;
            if (argc < 4) 
                puts("POST_ARGS_ERR");
            else
                body = argv[3];
        }     
    }
    puts("test000");
    struct Node *list       = NULL;
    int sockfd              = 0;
    struct addrinfo *result = NULL;
    char *msg               = NULL;
    puts("test0");
    int err = parseURL(&list, argv[2]);
    if (err != 0) {
        fprintf(stderr, "Problem with parsing URL.\n");
        return 1;
    }
    puts("test1");
    if (!check_protocol(list))
    {
        return 1;
    }
     puts("test2");
    char *domain_name = NULL;
    err = make_domain_name(list, &domain_name);
    if (err != 0) {
        fprintf(stderr, "Can not make resoutce url string.\n");
        return 1;
    }   
    puts("test3");
    char* resourсe_url = " ";
    err = make_resourсe_url(list, &resourсe_url, 3, 10);
    if (err != 0) {
        fprintf(stderr, "Can not make resoutce url string.\n");
        //return 1;
    }
    puts("test4");
    err = get_ip_from_domain(domain_name, "80", &result);
    if (err != 0) {
        fprintf(stderr, "Can not resolve the domain name.\n");
        return 1;
    }
    puts("test5");
    err = try_connect(result, &sockfd);
    if (err != 0) {
        fprintf(stderr, "Can not connect to the host.\n");
        return 1;
    }
    freeaddrinfo(result);
    
    puts(domain_name);
    puts(resourсe_url);
    //puts(msg);

    switch(method_type)
    {
        case GET:
            err = make_get_request(domain_name, resourсe_url, &msg);
            break;
        case POST:
            err = make_post_request(domain_name, resourсe_url, argv[3], &msg);
            break;  
        default:
            err = -1;
    }
    puts(msg);
    if (err != 0) {
        fprintf(stderr, "Can not generate msg.\n");
        return 1;
    }

    get_response(sockfd);

    if(resourсe_url)
        free(resourсe_url);
    if(msg)
        free(msg);

    close(sockfd);
    free_list(list);
}
