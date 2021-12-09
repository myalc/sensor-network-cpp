/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */


#ifndef SPCK_H_
#define SPCK_H_

#include "Common.h"
#include "LoadConfig.h"

class SPck {

public:
	SPck(int port, char* server, int *client_sock, LoadConfig *conf);
	SPck(LoadConfig *conf);
	virtual ~SPck();
	int init(int port, char *server, int *client_sock);
	void send1();
	int add2_sendList(packet_t *pck);

	list<packet_t*> m_sPackets;

private:
	packet_raw_t* packet2raw(packet_t *pck);

	LoadConfig *m_conf;
	pthread_t m_send_thr;
	int m_client_socket, m_port;
	pthread_mutex_t m_mtx_send1;
	pthread_cond_t m_cond;
	char m_server[32];

};

#endif /* SPCK_H_ */
