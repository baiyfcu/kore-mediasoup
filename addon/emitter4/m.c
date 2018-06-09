#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>

#include <stdint.h>

#include <sys/un.h>
#include <sys/types.h>
#include <node_api.h>


#include <uv.h>
#define socket_name "/home/globik/fuck"
static uv_poll_t sockin_watcher,sockout_watcher;
static uv_os_sock_t data_socket=0;
int uv_poll_init_success=0;
int fucker=0;

//static char log_buf[4096];
//static size_t log_buf_pos=0;

static void on_sock_read(uv_poll_t*,int,int);
static void on_sock_write(uv_poll_t*,int,int);
static int write_start(char*,size_t);

static void on_clo(uv_handle_t*);
static void at_pexit(void);
static int sockfd_init(void);
static int uvpoll_init(napi_env);
static void uvpoll_cleanup(void);

static void uvpoll_cleanup(){
printf("uvpoll_cleanup() occured.\n");
if(uv_poll_init_success==0){ printf("sucess init: %d\n",uv_poll_init_success);return;}else{
printf("sucess init: %d\n",uv_poll_init_success);
}
	printf("before uv_poll\n");
//uv_poll_stop(&sockout_watcher);
//uv_poll_stop(&sockin_watcher);
uv_close((uv_handle_t*)&sockout_watcher,on_clo);
	printf("af4\n");
	//usleep(1000000);
uv_close((uv_handle_t*)&sockin_watcher,on_clo);
	printf("af5\n");
	//usleep(100000);
	printf("data_socket end: %d\n",data_socket);
close(data_socket);
	data_socket=0;
uv_poll_init_success=0;
} 



static void on_clo(uv_handle_t*h){
printf("****on_clo occured.****\n");
printf("handle: %p, h->data: %p\n",h,h->data);
if(h->data){
printf("h->data %s\n",(char*)h->data);
if(h->data !=NULL) free(h->data);
}
}

static int sockfd_init(){
int ret;
struct sockaddr_un addr;
data_socket=socket(AF_UNIX,SOCK_SEQPACKET | O_NONBLOCK,0);
if(data_socket==-1){
fprintf(stderr,"data_socket err: %d\n", data_socket);
perror("socket");
return -1;
}
memset(&addr,0,sizeof(struct sockaddr_un));
addr.sun_family=AF_UNIX;
strncpy(addr.sun_path,socket_name,sizeof(addr.sun_path)-1);
ret=connect(data_socket,(const struct sockaddr*)&addr,sizeof(struct sockaddr_un));
if(ret==-1){
fprintf(stderr,"the server is down.\n");
if(data_socket !=0)close(data_socket);
data_socket=0;
return -1;
}
return 0;
}
static int uvpoll_init(napi_env env){
if(data_socket <=0) {printf("data_socket is undefined\n");return -1;}
uv_loop_t *loop=NULL;
napi_status status=napi_get_uv_event_loop(env,&loop);
if(status==napi_ok){
fprintf(stderr, "loop is ok\n");
}else{
fprintf(stderr,"loop is NOT ok\n");
if(data_socket !=0){close(data_socket);data_socket=0;}
return -1;
}
	
int s=uv_poll_init(loop, &sockin_watcher, data_socket);
if(0 !=s){
perror("poll init failed: ");
fprintf(stderr,"poll init failed\n");
if(data_socket !=0)close(data_socket);
return -1;
}
s=uv_poll_init(loop, &sockout_watcher, data_socket);
if(0 !=s){
perror("sockout_watcher init failed.");
uv_poll_stop(&sockin_watcher);
if(data_socket !=0)close(data_socket);
return -1;
}
uv_poll_init_success = 1;
uv_poll_start(&sockout_watcher, UV_READABLE | UV_DISCONNECT, on_sock_read);
return 0;
}

static int write_start(char*s, size_t leni){
uv_poll_stop(&sockout_watcher);
printf("read_start leni:: %d\n", leni);
//char*s="FUCKER HERE.";
sockin_watcher.data=NULL;
sockin_watcher.data=strdup(s);
uv_poll_start(&sockin_watcher, UV_WRITABLE, on_sock_write);
return 0;
}

static void on_sock_read(uv_poll_t*watcher,int status,int revents){
char buffer[512];
fucker++;
printf("fucker: %d",fucker);
printf("revents: %d, status: %d\n",revents,status);
if(status !=0)return;
if(fucker==10){
fprintf(stderr,"fucker=%d\n",fucker);
uvpoll_cleanup();
return;
}
if(revents == UV_READABLE){
		
int	ret=read(data_socket,buffer,512);
if(ret==-1){
perror("read");
fprintf(stderr,"read error occured in sock_read.\n");
return;
}
buffer[ret-1]=0;
fprintf(stderr,"result => %s\n",buffer);
}else if((revents == UV_DISCONNECT) || revents==5){
	//cleanup
	printf("in uv disconnect event and 5\n");
	uvpoll_cleanup();
}else{
fprintf(stderr,"unknown event in sock_read.\n");
}
}

int buka=0;

static void on_sock_write(uv_poll_t*watcher,int status,int mevents){
buka++;
	int ret;
if(status !=0){
if(watcher->data !=NULL)free(watcher->data);
watcher->data=NULL;
uv_poll_stop(&sockin_watcher);
uv_poll_start(&sockout_watcher, UV_READABLE | UV_DISCONNECT, on_sock_read);
return;
}
	
if(buka==5){
printf("on_sock_write occured times: %d\n",buka);
//mexit=1;
uv_poll_stop(&sockin_watcher);
//printf("AFTER buka: %d, mexit: %d\n",buka,mexit);
uv_poll_stop(&sockout_watcher);
//printf("NOCH AFRER mexit: %d\n",mexit);
uv_close((uv_handle_t*)&sockout_watcher, on_clo);
uv_close((uv_handle_t*)&sockin_watcher,on_clo);
printf("data_socket: %d\n",data_socket);
if(data_socket !=0)close(data_socket);
return;
}
printf("BUKA write_sock: %d\n", buka);
printf("write_sock: MEVENTS: %d STATUS: %d\n",mevents,status);
if(mevents == UV_WRITABLE){
fprintf(stderr, "Uv writable\n");
char buf[512];
//strcpy(buf,"Hallo world!");
if(watcher->data !=NULL){
char*s=watcher->data;
printf("watcher->data: %s\n",s);
strcpy(buf,s);
free(s);
watcher->data=NULL;
int leni=strlen(buf)+1;
ret=send(data_socket,buf, leni, 0);
		
if(ret==-1){
printf("ret:: %d\n",ret);
perror("write2");
}
printf("leni: %d, ret: %d\n",leni,ret);
}
memset(buf,0,512);
}
uv_poll_stop(&sockin_watcher);
uv_poll_start(&sockout_watcher, UV_READABLE | UV_DISCONNECT, on_sock_read);
}

napi_value p_init(napi_env env, napi_callback_info info)
{
//napi_status k;
size_t argc = 2;
napi_value args[2];
napi_get_cb_info(env, info, &argc, args, NULL, NULL);
if(argc == 1){
printf("OK in a number of argc. A single argument as expected: %d\n", argc);
}else{
printf("argc is %d\n", argc);
}
napi_valuetype val0;
napi_typeof(env, args[0], &val0);
if(val0 == napi_function){
printf("OK, args[0] is a function.\n");
}else{
printf("args[0] is not a function.\n");
}
napi_valuetype val1;
napi_typeof(env, args[1], &val1);
if(val1 == napi_undefined){
printf("args[1] is undefined.\n");
}else{
printf("Additional args[1] should be undefined.\n");
}

napi_value argv[1];
int a=sockfd_init();
int b=uvpoll_init(env);

	printf("sockfd_init(): %d\n",a);
	printf("uvpoll_init(); %d\n",b);
	
const char * str = "start_result";
size_t str_len = strlen(str);
napi_create_string_utf8(env, str, str_len, argv);
napi_value global;
napi_get_global(env, &global);
napi_value cb = args[0];
napi_status status = napi_call_function(env, global, cb, 2, argv, NULL);
if(status == napi_ok){
printf("napi_status is OK! Event fired!\n");
}else{
printf("napi_status is NOT OK!\n");
return NULL;
}
return NULL;
}

napi_value p_close(napi_env env, napi_callback_info info)
{
//napi_status k;
size_t argc = 2;
napi_value args[2];
napi_get_cb_info(env, info, &argc, args, NULL, NULL);
if(argc == 1){
printf("OK in a number of argc. A single argument as expected: %d\n", argc);
}else{
printf("argc is %d\n", argc);
}
napi_valuetype val0;
napi_typeof(env, args[0], &val0);
if(val0 == napi_function){
printf("OK, args[0] is a function.\n");
}else{
printf("args[0] is not a function.\n");
}
napi_valuetype val1;
napi_typeof(env, args[1], &val1);
if(val1 == napi_undefined){
printf("args[1] is undefined.\n");
}else{
printf("Additional args[1] should be undefined.\n");
}

napi_value argv[1];

const char * str = "close_result";
size_t str_len = strlen(str);
napi_create_string_utf8(env, str, str_len, argv);
napi_value global;
napi_get_global(env, &global);
napi_value cb = args[0];
napi_status status = napi_call_function(env, global, cb, 2, argv, NULL);
if(status == napi_ok){
printf("napi_status is OK! Event fired!\n");
}else{
printf("napi_status is NOT OK!\n");
return NULL;
}
return NULL;
}


napi_value p_send_ev_fd(napi_env env, napi_callback_info info)
{
napi_status k;
size_t argc = 2; // input buffer and callback
napi_value args[2]; // callback
napi_get_cb_info(env, info, &argc, args, NULL, NULL);
if(argc == 1){
printf("OK in a number of argc. A single argument as expected: %d\n", argc);
}else{
printf("argc is %d\n", argc);
}
	
//const unsigned char*input;
	char*input;
	size_t len;
	bool is_buf;
	k=napi_is_buffer(env,args[0],&is_buf);
	if(k !=napi_ok)printf("kstatus buf failed.\n");
	if(is_buf){
printf("IS BUFFER!\n");
	k=napi_get_buffer_info(env,args[0],(void**)(&input),&len);
	if(k !=napi_ok){
printf("failed to get buffer info input.\n");
return NULL;
	}
		printf("length: %d\n",len);
		printf("input : %s\n",input);
	}else{printf("IS NOT A BUFFER\n");}
	
	
napi_valuetype val0;
napi_typeof(env, args[1], &val0);
if(val0 == napi_function){
printf("OK, args[1] is a function.\n");
}else{
printf("args[1] is not a function.\n");

}
	

	//return NULL;
//write_start("mama\0", 5);
	write_start(input,len);

napi_value argv[2];// for cb args: err, data

const char * str = "write_result";
size_t str_len = strlen(str);
napi_create_string_utf8(env, str, str_len, &argv[1]);
napi_get_null(env,&argv[0]);
napi_value global;
napi_get_global(env, &global);
napi_value cb = args[1];//args[0] is an input buffer, args[1] is output -> callback(err,data)=>err is &argv[0], data is &argv[1]
napi_status status = napi_call_function(env, global, cb, 2, argv, NULL);
if(status == napi_ok){
printf("napi_status is OK! Event fired!\n");
}else{
printf("napi_status is NOT OK! for call_func\n");
return NULL;
}
return NULL;
}

static void at_pexit(){
printf("at_exit cb occured.\n");
uvpoll_cleanup();
	//usleep(100000);
}

napi_value Init(napi_env env, napi_value exports)
{
atexit(at_pexit);
napi_property_descriptor desc[3] = {
	{"p_init",0, p_init, 0, 0, 0, napi_default, 0},
	{"p_close",0,p_close, 0, 0, 0, napi_default, 0},
	{"p_send_ev_fd",0,p_send_ev_fd,0,0,0,napi_default,0}
};
napi_define_properties(env, exports, sizeof(desc)/sizeof(*desc), desc);
return exports;
}
NAPI_MODULE(addon,Init)