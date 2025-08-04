#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h> 

#define BUF_SIZE 512

struct Node {
	char* str_val;
	struct Node* next;
	size_t index;
};

static int cpy_str(char** new_str, char* old_str) {

    if (old_str == NULL) {
        *new_str = NULL;
        return 0;
    }
   
    size_t str_len = strlen(old_str) + 1;
    *new_str = (char*)malloc(str_len);
   
    if (*new_str == NULL) {
        perror("Memory for the string can not be allocated.");
        return 1;
    }

    memcpy(*new_str, old_str, str_len);
   
    return 0;
	
} 

static int make_node(struct Node** node, char* val) {

	*node = (struct Node*) malloc(sizeof(struct Node*));
	 
	if (*node == NULL) {
		
		return 1;
	}
	char* new_val;
		
	if (cpy_str(&new_val, val) != 0) {
		return 1;
	}

	(*node)->str_val = new_val;
	(*node)->next = NULL;
	
}

static int add(struct Node** head, char* val) {

	struct Node* new_node;
	int res = make_node(&new_node, val);
	if (!res) {
		
		return 1;
	}
	if (*head == NULL) {
		*head = new_node;
		return 0;
	}

	struct Node* cur = *head;

	while (cur->next != NULL)
		cur = cur->next;

	cur->next = new_node;
  
	new_node->index = cur->index + 1;
	
}

static int parseURL(struct Node** list, char* URLstr) {

	char str_buff[2048];
	size_t buff_ptr = 0;
	size_t url_len = strlen(URLstr);
	memset(str_buff, 0, 2048 * sizeof(char));
	printf("%d\n", url_len);
	for (size_t i = 0; i < url_len && buff_ptr < 2047; i++) {
	 
		if (URLstr[i] == '/' || i + 1 == url_len) {
                        if (i + 1 == url_len)
                            str_buff[buff_ptr++] = URLstr[i];
			str_buff[++buff_ptr] = '\0';
			int res_of_add = add(list, str_buff);
			memset(str_buff, 0, buff_ptr * sizeof(char));
			buff_ptr = 0;
		} else {
		    str_buff[buff_ptr++] = URLstr[i];
		}
	
	}

}

static struct Node* get_node(struct Node* list, size_t index) {
	struct Node* cur = list;
    while (cur) {
        if (cur->index == index)
			return cur;
        cur = cur->next;
    }
	return NULL;
}

static void print_list(struct Node* list) {
     
    struct Node* cur = list;
    while (cur) {
        printf("val %s index: %d\n", cur->str_val, cur->index);
        cur = cur->next;
    }
}

int free_list(struct Node* list) {
    struct Node* cur = list;
    while (cur) {
        struct Node* temp = cur;
        cur = cur->next;
        free(temp->str_val);
        free(temp);
    }
}

static int get_ip_from_domain(char* domain_name, const char* port, struct addrinfo** result) {
	struct addrinfo hints;
	int err;
	memset(&hints, 0, sizeof(struct addrinfo));
	
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM ; /* TCP socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    err = getaddrinfo(domain_name, port, &hints, result);
	if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }
	
	//freeaddrinfo(&hints);
	return 0;
}

static int try_connect(struct addrinfo* addr_info, int* sockfd) {
	struct addrinfo* rp;
	for (rp = addr_info; rp != NULL; rp = rp->ai_next) {
        *sockfd = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);
        if (*sockfd == -1)
            continue;

        if (connect(*sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
		    puts("Success");
			break; 
	    }
		close(*sockfd);
    }
	
    if (rp == NULL) {              
        fprintf(stderr, "Could not connect\n");
        return 1;
    }

	return 0;
}


int main(int argc, char** argv) {

    if (argc < 2) {
        printf("");
        return 0;
    }
	
    char* URL = argv[1];
    struct Node* list = NULL;
    int sockfd = 0;
	struct addrinfo* result;
	
	
    int err = parseURL(&list, URL);
    if (err != 0) {
		fprintf(stderr, "Problem with parsing URL.\n");
		return 1;
	}
	
	struct Node* domain_name_node = get_node(list, 2);

    if (domain_name_node == NULL)
        return 1;
	
    char* domain_name = domain_name_node->str_val;
    puts(domain_name);
    
	err = get_ip_from_domain(domain_name, "80", &result);
	if (err != 0) {
		fprintf(stderr, "Can not resolve the domain name.\n");
		return 1;
	}

	err = try_connect(result, &sockfd);
	if (err != 0) {
		fprintf(stderr, "Can not connect to the host.\n");
		return 1;
	}
	
	freeaddrinfo(result);
	
    const char* msg = 
        "GET / HTTP/1.1\r\n"
        "Host: www.ccfit.nsu.ru\r\n"
        "Connection: close\r\n"
        "\r\n";
    int len, bytes_sent;

    len = strlen(msg);
    bytes_sent = send(sockfd, msg, len, 0);
    printf ("bytes sent \n", bytes_sent);
    char buff[4096];
    len = 4096;
    size_t count;
    
    while ((count = recv(sockfd, (void*) buff, len, 0)) != 0) {
        puts(buff);
        if (count == -1) {
            switch (errno) {
                case EAGAIN:
                usleep(20000);
                break;
            default:
                perror("Failed to read from socket");
                close(sockfd);
                break;
            }
        }
    }
    close(sockfd);
	free_list(list);
}
