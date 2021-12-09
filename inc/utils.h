/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef _LIB_UTILS_H_
#define _LIB_UTILS_H_

#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>


#define		FAILURE		1
#define		SUCCESS		0

#define		FILE_NAME_LEN	2048
#define		MAX_FILE_SIZE	1024*1024*1024 /* 2M */
#define		DATE_CHAR_LEN	32
#define		PACKET_LEN		255

/********** logger */

#define IPV4_CHAR_LEN sizeof("255.255.255.255")
#define MAX_PCK_LEN 255
#define CONN_RETRY_PERIOD 3

extern volatile int exit_request;
extern int stat_sendconn;
extern int stat_recvconn;
extern pthread_mutex_t mtx_log;

#define ENABLE_MTX_LOG

#ifdef ENABLE_MTX_LOG
#define LOG(...) do { pthread_mutex_lock(&mtx_log); \
		utils_log(__VA_ARGS__); pthread_mutex_unlock(&mtx_log); \
	} while (0)
#else
#define LOG(...) do { utils_log(__VA_ARGS__); } while (0)
#endif

/**	log levels	*/
enum LOG_LEVEL {
	LOG_ERR = 0,
	LOG_WARN,
	LOG_INFO,
	LOG_DBG,
	LOG_TRACE
};

/** error conditions */
#define	LG_ERR		"ERR   " , __FILE__, __LINE__, LOG_ERR
#define	LG_WARN		"WARN  " , __FILE__, __LINE__, LOG_WARN
#define	LG_INFO		"INFO  " , __FILE__, __LINE__, LOG_INFO
#define	LG_DBG		"DBG   " , __FILE__, __LINE__, LOG_DBG
#define	LG_TRACE	"TRACE " , __FILE__, __LINE__, LOG_TRACE

/********** serial_port */
#define SP_SELF_TEST

/********************** function prototypes *************************/

/** initialize logger
 * @param level log level
 * @param filename log file name 
 * @return SUCCESS or FAILURE
 */ 
int utils_log_init(const int level, const char *filename);

/** change log level
 * @param level new log level
 * @return SUCCESS or FAILURE
 */
int utils_change_log_level(const int level);

/** deinitialize logger 
 * @return SUCCESS or FAILURE
 */
int utils_log_free();

/** backup log files
 * @return SUCCESS or FAILURE
 */
int utils_log_backup();

/** write log to log file
 * @param str message to log
 * @param file logfile 
 * @param line line number
 * @param level log level
 * @param fmt format
 */
int utils_log(const char *str, const char *file, const int line,
		const int level, const char *fmt, ...);


/********** common */

/** allocate space from memory
 * @param size allocation size
 * @return allocated space or NULL on FAILURE
 */
void *utils_malloc(size_t size);

/** run system command 
 * @param cmd command 
 * @return result of command
 */
char *utils_read_pipe(char *cmd);

uint32_t get_epochtime();


/********** serial_port  */

typedef struct data_s {
	uint8_t len;
	char *buf;
} data_t;

typedef struct packet_raw_s {
	uint8_t len;
	char *buf;
} packet_raw_t;

typedef struct packet_s {
	uint32_t ts;
	uint8_t mid;
	uint8_t ptype;
	uint8_t value;
} packet_t;

/* sender - client */
int utils_init_send(int sport, char *server, int *serv_fd);
int utils_connect_server(int *sck);
int utils_check_conn_state(int sock);
int utils__send_data(data_t *data, int *clnt_fd);
int utils_send_data(data_t *data, int *clnt_fd);

/* receiver - server */
int utils_init_recv(int rport, int *cli_fd);
int utils_create_server(int *sck);
int utils_accept_conn(int *serv_sck, int *cli_sck);
int utils__receive_data(data_t *data, int *sock);
int utils_receive_data(data_t *data, int *serv_sck, int *sock);


#endif /* _LIB_UTILS_H_ */
