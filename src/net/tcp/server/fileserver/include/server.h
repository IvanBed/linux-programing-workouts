#ifndef SERVER_H

#define SERVER_H
#include "net.h"
#include "utils.h"
#include "os_layer.h"
#include "conn_pool.h"

//#include "log.h"

#include <sys/select.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <stdatomic.h>
#include <fcntl.h>

#define LOOPBACK "127.0.0.1"
#define QEUEUSIZE 5
#define MAX_ATTEMPS 100
#define BUFSIZE 4096 * 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define THREAD_LIMIT 64

typedef struct Pipe
{
    int    pfd[2];
    fd_set readfds;
    int    nfds;
} Pipe;

typedef struct WorkerArgs
{
    ConnectionsPool *connections_pool;
    int64_t          offset;
    LogDesc         *log_desc;
} WorkerArgs;

#endif