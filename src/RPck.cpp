/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "RPck.h"

/**
 * TODO::
 * 1. serialport için ikinci bir yapıcı eklenecek
 * 2. raw2packet() fonsiyonu tamamlanacak
 * 3. loglar düzenlenecek.
 */

void* start_receive_thread(void* arg)
{
	RPck* r = static_cast<RPck *>(arg);
	r->recv1();

	pthread_detach(pthread_self());
	pthread_exit(0);
}


RPck::RPck(int port, int *server_sock, LoadConfig *conf)
{
	m_conf = conf;
	LOG(LG_DBG, "RPck constructor\n");
	init(port, server_sock);
}

RPck::RPck(LoadConfig *conf)
{
	m_conf = conf;
}

RPck::~RPck()
{
	pthread_mutex_destroy(&m_mtx_recv1);
	/*delete some;*/
}

int RPck::init(int port, int *server_sock)
{
	int n;

	LOG(LG_DBG, "Initialize receiver\n");
	utils_init_recv(port, server_sock);

	m_server_socket = *server_sock;
	pthread_mutex_init(&m_mtx_recv1, NULL);

	/* create receive thread */
	if ((n = pthread_create(&m_recv_thr, 0, start_receive_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (RPck): %s\n", strerror(n));
		return FAILURE;
	} else
	{
		LOG(LG_INFO, "Created RPck thread\n");
	}
	return SUCCESS;
}

void RPck::recv1()
{
	int server_sock;
	packet_raw_t raw;
	int cli_fd = -1;

	server_sock = m_server_socket;

	/*LOG(LG_DBG, "Initialize receiver\n");
	init_recv(m_port, &server_sock);*/

	raw.buf = (char*) utils_malloc(MAX_PCK_LEN);
	utils_accept_conn(&server_sock, &cli_fd);

	while (!exit_request)
	{
		utils_receive_data((data_t*)&raw, &server_sock, &cli_fd);
		//LOG(LG_DBG, "received: %d, %d, %d, %d, l:%d\n", (int)(*(raw.buf+0)), (int)(*(raw.buf+1)), (int)(*(raw.buf+2)), (int)(*(raw.buf+3)) ,(int)raw.len);
		raw2packet(&raw);
	}

	LOG(LG_DBG, "Receive thread terminated.\n");
	free(raw.buf);
}

int RPck::raw2packet(packet_raw_t *raw)
{
	/* combo paketler de bu fonksiyonda ayrıştırılacak */
	/* bir adet packet_raw_t'den birden fazla packet_t çıkartılabilir */

	packet_t *pck = NULL;
	int ret;

	pck = (packet_t*) utils_malloc(sizeof(packet_t));
	memset(pck, 0, sizeof(packet_t));

	pck->mid 	= (*(raw->buf+0));
	pck->ptype 	= (*(raw->buf+1));
	pck->value	= (*(raw->buf+2));
	pck->ts 	= get_epochtime();

	if (raw->len == 5)
	{
		pck->ptype -= 1;
		ret = add2_receivedList(pck);
		//LOG(LG_DBG, "add2rcvlist: %d, %d, %d\n", (int)pck->mid, (int)pck->ptype, (int)pck->value);

		// battery
		pck = (packet_t*) utils_malloc(sizeof(packet_t));
		memset(pck, 0, sizeof(packet_t));

		pck->mid 	= (*(raw->buf+0));
		pck->ptype 	= 1;
		pck->value	= (*(raw->buf+3));
		pck->ts 	= get_epochtime();

		ret = add2_receivedList(pck);
		//LOG(LG_DBG, "add2rcvlist: %d, %d, %d\n", (int)pck->mid, (int)pck->ptype, (int)pck->value);
	}
	else
	{
		ret = add2_receivedList(pck);
		//LOG(LG_DBG, "add2rcvlist: %d, %d, %d\n", (int)pck->mid, (int)pck->ptype, (int)pck->value);
	}

	return SUCCESS;
}

int RPck::add2_receivedList(packet_t *pck)
{
	//LOG(LG_DBG, "add2_receivedList: %d\n", pck->mid);
	pthread_mutex_lock(&m_mtx_recv1);
	m_rPackets.push_back(pck);
	pthread_mutex_unlock(&m_mtx_recv1);

	return SUCCESS;
}

packet_t *RPck::get_fromlist()
{
	int sz = 0;
	list<packet_t*>::iterator it;
	packet_t *p = NULL;

	if (stat_recvconn == 0)
		return NULL;

	do {
		pthread_mutex_lock(&m_mtx_recv1);
		sz = m_rPackets.size();
		pthread_mutex_unlock(&m_mtx_recv1);
		if (sz == 0)
			sleep(1);
	} while (sz == 0);

	pthread_mutex_lock(&m_mtx_recv1);
	it = m_rPackets.begin();
	p = *it;
	m_rPackets.erase(m_rPackets.begin());
	//LOG(LG_DBG, "get_pck1: %d\n", (int)p->mid);
	pthread_mutex_unlock(&m_mtx_recv1);

	/*pthread_mutex_lock(&m_mtx_recv1);
	if (m_rPackets.size() == 0) {
		p = NULL;
	} else {
		it = m_rPackets.begin();
		p = *it;
		m_rPackets.erase(m_rPackets.begin());
		//LOG(LG_DBG, "get_pck1: %d\t%d\t%d\t%d\n", p->mid, p->type, p->value, p->crc);
		//free(p);
	}
	pthread_mutex_unlock(&m_mtx_recv1);*/

	return p;
}
