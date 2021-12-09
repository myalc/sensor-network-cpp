/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "SPck.h"

/**
 * TODO::
 * 1. serialport için ikinci bir yapıcı eklenecek
 * 2. packet2raw() fonsiyonu tamamlanacak
 * 3. loglar düzenlenecek.
 */

void* start_send_thread(void* arg)
{
	SPck* s = static_cast<SPck *>(arg);
	s->send1();

	pthread_detach(pthread_self());
	pthread_exit(0);
}


SPck::SPck(int port, char* server, int *client_sock, LoadConfig *conf)
{
	m_conf = conf;
	LOG(LG_DBG, "SPck constructor\n");
	init(port, server, client_sock);
}

SPck::SPck(LoadConfig *conf)
{
	m_conf = conf;
}

SPck::~SPck()
{
	pthread_mutex_destroy(&m_mtx_send1);
	/*delete some;*/
}

int SPck::init(int port, char *server, int *client_sock)
{
	int n;

	LOG(LG_DBG, "Initialize sender\n");
	utils_init_send(port, server, client_sock);

	m_port = port;
	sprintf(m_server, "%s", server);
	m_client_socket = *client_sock;
	pthread_mutex_init(&m_mtx_send1, NULL);

	/* create send thread */
	if ((n = pthread_create(&m_send_thr, 0, start_send_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (SPck): %s\n", strerror(n));
		return FAILURE;
	} else
	{
		LOG(LG_INFO, "Created SPck thread\n");
	}
	return SUCCESS;
}

void SPck::send1()
{
	int sz, client_sock, n;
	list<packet_t*>::iterator it;
	packet_t *p;
	packet_raw_t *raw = NULL;
	char *server_ptr;

	client_sock = m_client_socket;

	/*LOG(LG_DBG, "Initialize sender\n");
	init_send(m_port, m_server, &client_sock);*/

	while (!exit_request)
	{
		pthread_mutex_lock(&m_mtx_send1);
		sz = m_sPackets.size();
		pthread_mutex_unlock(&m_mtx_send1);

		if (sz == 0) {
			sleep(1);
			continue;
		}

		pthread_mutex_lock(&m_mtx_send1);
		it = m_sPackets.begin();
		p = *it;
		m_sPackets.erase(m_sPackets.begin());
		pthread_mutex_unlock(&m_mtx_send1);

		//LOG(LG_DBG, "sending: %d\n", p->mid);
		raw = packet2raw(p);

		utils_send_data((data_t*)raw, &client_sock);
		free(raw->buf);
		free(raw);
	}

	LOG(LG_DBG, "send thread terminated.\n");
}

int SPck::add2_sendList(packet_t *pck)
{
	//LOG(LG_DBG, "add2_sendlist %d\n", pck->mid);
	if (stat_sendconn == 0)
		return FAILURE;

	pthread_mutex_lock(&m_mtx_send1);
	m_sPackets.push_back(pck);
	pthread_mutex_unlock(&m_mtx_send1);

	return SUCCESS;
}

packet_raw_t* SPck::packet2raw(packet_t *pck)
{
	uint8_t tmp = 33;
	packet_raw_t *raw = (packet_raw_t*) utils_malloc(sizeof(packet_raw_t));
	raw->len = 4;
	raw->buf = (char*) utils_malloc(raw->len);
	memcpy(raw->buf,   &(pck->mid), 1);
	memcpy(raw->buf+1, &(pck->ptype), 1);
	memcpy(raw->buf+2, &(pck->value), 1);
	memcpy(raw->buf+3, &tmp, 1);
	//LOG(LG_DBG, "pck2raw: %d, %d, %d, %d, l:%d\n", (int)(*(raw->buf+0)), (int)(*(raw->buf+1)), (int)(*(raw->buf+2)), (int)(*(raw->buf+3)) ,(int)raw->len);

	free(pck);
	return raw;
}
