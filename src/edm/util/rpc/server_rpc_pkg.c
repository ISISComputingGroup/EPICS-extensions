#include stdio
#include stdlib
#include descrip

#include sys_types
#include server_rpc_priv

static int g_init = 1;

static void do_init ( void ) {

  if ( g_init ) {

    g_init = 0;

  }

}

int server_rpc_create_buffer (
  int max_funcs,
  RPC_PORT port,
  void *server_data,
  RPC_BUF *buf
) {

PRIV_RPC_BUF_PTR priv_buf;
int i, ret_stat;

  if ( g_init ) do_init();

  priv_buf = (PRIV_RPC_BUF_PTR) calloc( 1, sizeof(PRIV_RPC_BUF_TYPE) );
  if ( !priv_buf ) {
    ret_stat = RPC_NOMEM;
    goto err_return;
  }

  priv_buf->rpc_port = port;

  priv_buf->server_data = server_data;

  priv_buf->func = (FUNC_PTR) calloc( max_funcs, sizeof(FUNC_TYPE) );
  if ( priv_buf->func == NULL ) {
    ret_stat = RPC_NOMEM;
    goto err_return;
  }
  for ( i=0; i<max_funcs; i++ ) priv_buf->func[i] = NULL;

  priv_buf->max_funcs = max_funcs;

  *buf = (RPC_BUF *) priv_buf;

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}

int server_rpc_destroy_buffer (
  RPC_BUF buf
) {

PRIV_RPC_BUF_PTR priv_buf;
int ret_stat;

  priv_buf = (PRIV_RPC_BUF_PTR) buf;

  if ( !priv_buf ) {
    ret_stat = RPC_BADPARAM;
    goto err_return;
  }

  if ( priv_buf->func ) {
    free( priv_buf->func );
  }

  free( priv_buf );

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}

int server_rpc_register_function(
  RPC_BUF buf,
  void (*func)(),
  int func_index
) {

PRIV_RPC_BUF_PTR priv_buf;
int ret_stat;

  priv_buf = (PRIV_RPC_BUF_PTR) buf;

  if ( func_index >= priv_buf->max_funcs ) {
    ret_stat = RPC_BADFUNCINDEX;
    goto err_return;
  }

  priv_buf->func[func_index] = func;

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}

int server_rpc_send_reply (
  RPC_BUF buf
) {

PRIV_RPC_BUF_PTR priv_buf;
int len, stat, ret_stat;

  priv_buf = (PRIV_RPC_BUF_PTR) buf;

  priv_buf->msg_buf.rcv_seq_num = priv_buf->msg_buf.send_seq_num;

  stat = nsv_send_msg( priv_buf->rpc_port, priv_buf->msg_size,
   &priv_buf->msg_buf );
  if ( stat != NSV_SUCCESS ) {
    ret_stat = stat;
    goto err_return;
  }

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}

int server_rpc_process_request (
  RPC_BUF buf
) {

PRIV_RPC_BUF_PTR priv_buf;

int func_id, num_args, len, ofs, buf_size, i, ret_stat, stat;
int result;
char *arg[RPC_MAX_ARGS];
char *data;
ARG_DESC_TYPE *arg_desc;

  priv_buf = (PRIV_RPC_BUF_PTR) buf;

  stat = nsv_wait_on_port( priv_buf->rpc_port, (SYS_TIME_PTR) NULL, &result );
  if ( stat != NSV_SUCCESS ) {
    ret_stat = stat;
    goto err_return;
  }

  stat = nsv_receive_msg( priv_buf->rpc_port, sizeof(MSG_BUF_TYPE), &len,
   &priv_buf->msg_buf );
  if ( stat != NSV_SUCCESS ) {
    ret_stat = stat;
    goto err_return;
  }

  priv_buf->msg_size = len;

  func_id = priv_buf->msg_buf.func_id;
  if ( func_id >= priv_buf->max_funcs ) {
    ret_stat = RPC_BADFUNCINDEX;
    goto err_return;
  }

  if ( priv_buf->func[func_id] == NULL ) {
    ret_stat = RPC_NOSUCHFUNC;
    goto err_return;
  }

  num_args = priv_buf->msg_buf.num_args;
  if ( num_args >= RPC_MAX_ARGS ) {
    ret_stat = RPC_NUMARGSINV;
    goto err_return;
  }

  arg_desc = (ARG_DESC_TYPE *) priv_buf->msg_buf.data;
  data = (char *) priv_buf->msg_buf.data;

  for ( i=0; i<num_args; i++ ) {
    arg[i] = data + arg_desc[i].ofs;
  }

  switch ( num_args ) {

    case 1:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0] );
      break;

    case 2:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1] );
      break;

    case 3:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2] );
      break;

    case 4:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3] );
      break;

    case 5:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4] );
      break;

    case 6:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5] );
      break;

    case 7:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6] );
      break;

    case 8:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7] );
      break;

    case 9:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8] );
      break;

    case 10:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9] );
       
      break;

    case 11:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10] );
      break;

    case 12:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11] );
      break;

    case 13:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12] );
      break;

    case 14:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13] );
      break;

    case 15:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14] );
      break;

    case 16:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14], arg[15] );
      break;

    case 17:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14], arg[15], arg[16] );
      break;

    case 18:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14], arg[15], arg[16],
       arg[17] );
      break;

    case 19:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14], arg[15], arg[16],
       arg[17], arg[18] );
      break;

    case 20:
      (*priv_buf->func[func_id])( priv_buf->server_data, arg[0], arg[1],
       arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9],
       arg[10], arg[11], arg[12], arg[13], arg[14], arg[15], arg[16],
       arg[17], arg[18], arg[19] );
      break;

    default:
      ret_stat = RPC_NUMARGSINV;
      goto err_return;

  }

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}

int server_rpc_dispatch_requests(
  RPC_BUF buf
) {

int stat, ret_stat;

  while ( 1 ) {

    stat = server_rpc_process_request( buf );
    if ( stat != RPC_SUCCESS ) {
      ret_stat = stat;
      goto err_return;
    }

    stat = server_rpc_send_reply( buf );
    if ( stat != RPC_SUCCESS ) {
      ret_stat = stat;
      goto err_return;
    }

  }

norm_return:
  return RPC_SUCCESS;

err_return:
  return ret_stat;

}
