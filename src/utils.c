/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */
 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "utils.h"

volatile int exit_request = 0;
int stat_sendconn = 0;
int stat_recvconn = 0;

/********** logger */

static FILE	*logfile = NULL;
static char logfile_name[FILE_NAME_LEN];
static unsigned int	logfile_len = 0;
static unsigned int logfile_n;
static unsigned int loglevel;
static char VERSION_INFO[FILE_NAME_LEN];

pthread_mutex_t mtx_log = PTHREAD_MUTEX_INITIALIZER;

#define RESET		0
#define BRIGHT 		1
#define DIM			2
#define UNDERLINE 	4
#define BLINK		5
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7
#define	NONE		8

/********** serial_port */


/************************** functions ******************************/
/********** logger */

int utils_log_init(const int level, const char *filename)
{
#ifdef ENABLE_MTX_LOG
	pthread_mutex_init(&mtx_log, NULL);
#endif

	size_t len;
	char file[FILE_NAME_LEN];

	if (filename == NULL)
		len = 0;
	else
		len = strlen(filename);

	/*	set log level	*/
	utils_change_log_level(level);
	VERSION_INFO[0] = '\0';

	/*	control file name	*/
	if (len > FILE_NAME_LEN) {
		fprintf(stderr, "Cannot init logfile_name\r\n");
		logfile = stdout;
		return FAILURE;
	}

	if (len == 0) {
		fprintf(stdout, "Using stdout instead of file\r\n");
		logfile = stdout;
		return SUCCESS;
	}

	/*	copy log file name and create	*/
	strcpy(logfile_name, filename);
	snprintf(file, FILE_NAME_LEN, "%s0.log", logfile_name);
	logfile_n = 0;

	/*	open log file for writing	*/
	logfile = fopen(file, "w");
	if (logfile == NULL) {
		fprintf(stderr, "Cannot open %s\r\n", file);
		logfile = stdout;
		return FAILURE;
	}

	logfile_len = 0;
	LOG(LG_INFO, "utils_log_init SUCCESS\r\n");

	return SUCCESS;
}

int utils_change_log_level(const int level)
{
	loglevel = level;
	return SUCCESS;
}

int utils_log_free()
{
	if (logfile == NULL)
		return FAILURE;

	if (logfile == stdout)
		return SUCCESS;

	fclose(logfile);
	logfile = stdout;

	return SUCCESS;
}

int utils_log_backup()
{
	time_t ts = time(NULL);
	char cmd[FILE_NAME_LEN];

	snprintf(cmd, FILE_NAME_LEN, "mv %s0.log %s0.log.%u &", logfile_name,
			logfile_name, (uint32_t)ts);
	system(cmd);

	snprintf(cmd, FILE_NAME_LEN, "mv %s1.log %s1.log.%u &", logfile_name,
				logfile_name, (uint32_t)ts);
	system(cmd);

	return SUCCESS;
}

static int __change_file()
{
	char file[FILE_NAME_LEN];

	if (logfile == stdout)
		return SUCCESS;

	if (logfile_n == 0)
		logfile_n = 1;
	else
		logfile_n = 0;

	snprintf(file, FILE_NAME_LEN, "%s%d.log", logfile_name, logfile_n);
	fclose(logfile);
	logfile = fopen(file, "w");
	if (logfile == NULL) {
		fprintf(stderr, "Cannot open %s\r\n", file);
		logfile = stdout;
		return FAILURE;
	}

	logfile_len = 0;

	return SUCCESS;
}

static char *__get_date()
{
	char *ret;
	struct tm date;
	time_t lt;

	ret = (char *)utils_malloc(DATE_CHAR_LEN * sizeof(char));
	lt = time(&lt);
	localtime_r(&lt, &date);
	asctime_r(&date, ret);
	/*	delete the last '\n' character	*/
	ret[strlen(ret)-1] = '\0';

	return ret;
}

static inline void textcolor(int attr, int fg, int bg)
{
	fprintf(stdout, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
}

int utils_log(const char *str, const char *file, const int line,
		const int level, const char *fmt, ...)
{
	va_list args;
	char *date;

	if (level > loglevel)
		return logfile_len;

	if (logfile == NULL)
		return logfile_len;

	flockfile(logfile);
	date = __get_date();
	logfile_len += fprintf(logfile, "%s ", date);
	free(date);

	if (logfile == stdout) {
		switch(level) {
		case LOG_ERR  : textcolor(BLINK, RED, NONE);		break;
		case LOG_WARN : textcolor(BRIGHT, MAGENTA, NONE);	break;
		case LOG_INFO : textcolor(BRIGHT, YELLOW, NONE);	break;
		case LOG_DBG  : textcolor(BRIGHT, GREEN, NONE);		break;
		case LOG_TRACE: textcolor(BRIGHT, BLUE, NONE);		break;
		default		  : textcolor(RESET, NONE, NONE);		break;
		}
	}

	logfile_len += fprintf(logfile, "%s", str);

	if (logfile == stdout) {
		textcolor(RESET, NONE, NONE);
	}

	logfile_len += fprintf(logfile, " %s:%d ", file, line);

	va_start(args, fmt);
	logfile_len += vfprintf(logfile, fmt, args);
	va_end(args);

	fflush(logfile);
	funlockfile(logfile);

	if (logfile_len >= MAX_FILE_SIZE)
		__change_file();

	return logfile_len;
}


/********** common */

void *utils_malloc(size_t size)
{
	void *ret;

	ret = malloc(size);
	if (ret == NULL) {
		LOG(LG_ERR, "%s\r\n", strerror(errno));
		raise(SIGTERM);
	}
	memset(ret, 0, size);

	return ret;
}

char *utils_read_pipe(char *cmd)
{
	FILE *in;
	char *ret = utils_malloc(FILE_NAME_LEN);

	if ((in = popen(cmd, "r")) == NULL) {
		LOG(LG_ERR, "Error while popen\n");
		return NULL;
	}

	while (fgets(ret, FILE_NAME_LEN, in) != NULL );
	pclose(in);
	ret[strlen(ret) - 1] = '\0';

	return ret;
}

uint32_t get_epochtime()
{
	time_t result;

	result = time(NULL);
	/*printf("%s%ju secs since the Epoch\n", asctime(localtime(&result)), (uintmax_t)result);*/

	return (uintmax_t)result;
}

/********** serial_port  */

char server_ip[IPV4_CHAR_LEN];
int sendport, recvport;

int utils_init_send(int port, char *server, int *cli_fd)
{
	sendport = port;
	sprintf(server_ip, "%s", server);
	utils_connect_server(cli_fd);

	return SUCCESS;
}

int utils_connect_server(int *sck)
{
	int n;
	struct sockaddr_in serv_addr;

	/* prepare for connect */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(sendport);
	serv_addr.sin_addr.s_addr = inet_addr(server_ip);
	stat_sendconn = 0;

	do {
		if (*sck != -1) close(*sck);
		// create socket
		if ((*sck = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			LOG(LG_ERR, "Create socket: %s\n", strerror(errno));
			sleep(CONN_RETRY_PERIOD);
			stat_sendconn = 0;
			continue;
		}

		// connect to server
		n = connect(*sck, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (n == -1) {
			LOG(LG_ERR, "Connect to server: %s\n", strerror(errno));
			sleep(CONN_RETRY_PERIOD);
			stat_sendconn = 0;
			continue;
		}
		break;
	} while (!exit_request);
	LOG(LG_WARN, "Connected to server\n");
	stat_sendconn = 1;

	return SUCCESS;
}

int utils_check_conn_state(int sock)
{
	char buf[MAX_PCK_LEN];
	int ret;

	ret = recv(sock, &buf[0], 100, MSG_DONTWAIT);
	if ((ret == 0) || ((ret == -1) && !(errno == EAGAIN || errno == EWOULDBLOCK))) {
		LOG(LG_WARN, "Connection closed by peer: ret: %d %s\n", ret, strerror(errno));
		return FAILURE;
	}
	return SUCCESS;
}

int utils__send_data(data_t *data, int *clnt_fd)
{
	if (utils_check_conn_state(*clnt_fd) == FAILURE)
			return FAILURE;

	if (write(*clnt_fd, &data->len, sizeof(uint8_t)) <= 0) {
		LOG(LG_WARN, "Send (len): %s\n", strerror(errno));
		return FAILURE;
	}
	if (write(*clnt_fd, data->buf, data->len) <= 0) {
		LOG(LG_WARN, "Send (buf): %s\n", strerror(errno));
		return FAILURE;
	}
	//LOG(LG_DBG, "sent: %s\t%d\n", data->buf, (int)data->len);

	return SUCCESS;
}

int utils_send_data(data_t *data, int *clnt_fd)
{
	int n;

	do {
		n = utils__send_data(data, clnt_fd);
		if (n == FAILURE)
			utils_connect_server(clnt_fd);
	} while ((n == FAILURE) && (!exit_request));

	return SUCCESS;
}


int utils_init_recv(int port, int *serv_fd)
{
	recvport = port;
	utils_create_server(serv_fd);

	return SUCCESS;
}

int utils_create_server(int *sck)
{
	struct sockaddr_in serv_addr;
	int n;

	/* prepare for bind */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(recvport);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	do {
		if (*sck != -1) close(*sck);
		stat_recvconn = 0;

		n = *sck = socket(AF_INET, SOCK_STREAM, 0);
		if (n == -1) {
			LOG(LG_WARN, "Create socket\n");
			sleep(CONN_RETRY_PERIOD);
			continue;
		}

		n = bind(*sck, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (n == -1) {
			LOG(LG_WARN, "Bind: %s\n", strerror(errno));
			sleep(CONN_RETRY_PERIOD);
			continue;
		}

		n = listen(*sck, 4294960000);
		if (n == -1) {
			LOG(LG_WARN, "Listen: %s\n", strerror(errno));
			sleep(CONN_RETRY_PERIOD);
			continue;
		}
		break;
	} while (!exit_request);

	LOG(LG_DBG, "Created server\n");
	return SUCCESS;
}

int utils_accept_conn(int *serv_sck, int *cli_sck)
{
	uint8_t *p;
	struct sockaddr_in cli;
	socklen_t csize = sizeof(cli);
	char cliaddr[IPV4_CHAR_LEN];
	stat_recvconn = 0;

	if (*cli_sck != -1) close(*cli_sck);
	*cli_sck = accept(*serv_sck, (struct sockaddr *)&cli, &csize);

	p = (uint8_t*) &(cli.sin_addr.s_addr);
	snprintf(cliaddr, IPV4_CHAR_LEN, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	LOG(LG_DBG, "New connection from %s:%d\n", cliaddr, *cli_sck);
	stat_recvconn = 1;

	return SUCCESS;
}

int utils__receive_data(data_t *data, int *sock)
{
	int n;

	/* read len */
	n = read(*sock, &data->len, sizeof(uint8_t));
	/* read buf */
	n = read(*sock, data->buf, data->len);
	if (n != data->len) {
		LOG(LG_ERR, "size do not match. n: %d rlen: %d\n", (int)n, (int)data->len);
		return FAILURE;
	}
	data->buf[data->len] = '\0';

	return SUCCESS;
}

int utils_receive_data(data_t *data, int *serv_sck, int *sock)
{
	int n;

	do {
		n = utils__receive_data(data, sock);
		if (n == FAILURE)
			utils_accept_conn(serv_sck, sock);
	} while ((n == FAILURE) && (!exit_request));

	return SUCCESS;
}






