
typedef struct server_msg 
{


} ServerMsg;
 
typedef struct client_msg 
{


} ClientMsg;

void secure_get_client_hello(ConnectionDesc *conn_desc)
{


}

uint8_t secure_conn_handshakes(ConnectionDesc *conn_desc)
{
    ssize_t        sent_cnt;
    ssize_t        recv_cnt;
    char           request[BUFSIZE];
    char           response[BUFSIZE];

    conn_desc->cipher_desc.asyn_session = open_asym_session();
    //send public key
    // get symmetrict key

    conn_desc->cipher_desc.syn_session = open_sym_session();
    // send ok

    return ESTABLISHED;
}

