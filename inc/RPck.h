/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */


#ifndef RPCK_H_
#define RPCK_H_

#include "Common.h"
#include "LoadConfig.h"

class RPck {

public:
	RPck(int port, int *server_sock, LoadConfig *conf);
	RPck(LoadConfig *conf);
	virtual ~RPck();
	int init(int port, int *server_sock);
	void recv1();
	packet_t *get_fromlist();

	list<packet_t*> m_rPackets;

private:
	int raw2packet(packet_raw_t *raw);
	int add2_receivedList(packet_t *pck);

	LoadConfig *m_conf;
	pthread_t m_recv_thr;
	int m_server_socket, m_port;
	pthread_mutex_t m_mtx_recv1;
};

#endif /* RPCK_H_ */
