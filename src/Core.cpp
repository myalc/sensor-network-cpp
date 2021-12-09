/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "Core.h"


Core::Core(const char *_xmlconf)
{
	utils_log_init(LOG_DBG, "");
	m_conf = new LoadConfig(_xmlconf);
	cout << "CONF:: " << m_conf->get_logpath() << " " << m_conf->get_loglevel() << " " << m_conf->get_database() << " " << m_conf->get_database_conn_str() << " " << m_conf->get_database_user() << " " << m_conf->get_database_psw() << " " << m_conf->get_seril_port() << " " << m_conf->get_check1_first_timeout() << " " << m_conf->get_check1_timeout() << " "<< m_conf->get_check2_timeout() << " " << m_conf->get_check3_timeout() << " " << m_conf->get_livedb_update_period() << "\n";

	utils_log_init(m_conf->get_loglevel(), m_conf->get_logpath().c_str());
	utils_log_init(LOG_DBG, "");

	m_send_client_sock = -1;
	m_send = new SPck(8001, (char*)"127.0.0.1", &m_send_client_sock, m_conf);		//threaded
	//m_send = new SPck(m_conf);

	m_recv_serverSock = -1;
	m_recv = new RPck(8002, &m_recv_serverSock, m_conf);							//threaded
	//m_recv = new RPck(m_conf);

	m_livedb 	= new LiveDataBase(m_conf);
	m_startup 	= new StartUpWSN(m_livedb, m_send, m_conf);
	m_incoming 	= new InComing(m_livedb, m_recv, m_conf);		//threaded

	m_check1 	= new WSNCheck1(m_livedb, m_send, m_conf);		//threaded
	m_check2 	= new WSNCheck2(m_livedb, m_send, m_conf);		//threaded
	m_check3 	= new WSNCheck3(m_livedb, m_send, m_conf);		//threaded

#if 0
	/* testing */
	int co = 0;
	int t;
	packet_t *rpck = NULL;
	packet_t *spck = NULL;
	while(!exit_request)
	{
		/* recv */
		rpck = m_recv->get_fromlist();
		if (rpck != NULL) {
			LOG(LG_DBG, "RECV: %d, %d, %d\n", (int)rpck->mid, (int)rpck->ptype, (int)rpck->value);
		}
		else {
			LOG(LG_DBG, "recv connection error\n");
			sleep(1);
			continue;
		}

		/* send */
		spck = (packet_t*) hsec_malloc(sizeof(packet_t));
		spck->mid 	= rpck->mid;
		spck->ptype = 101;
		spck->value = 10;
		t = m_send->add2_sendList(spck);
		if (t == 0)
			LOG(LG_DBG, "\tSENT: %d, %d, %d\n", (int)spck->mid, (int)spck->ptype, (int)spck->value);
		else
			LOG(LG_DBG, "send connection error\n");

		free(rpck);
	}
#endif
}

Core::~Core()
{
	delete m_conf;
	delete m_send;
	delete m_recv;
	delete m_livedb;
	delete m_startup;
	delete m_incoming;
	delete m_check1;
	delete m_check2;
	delete m_check3;
}

void Core::m_Deinit()
{
	m_startup->m_DeInitWSN();
}
