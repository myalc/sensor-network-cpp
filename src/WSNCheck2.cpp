/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "WSNCheck2.h"

void* start_check2_thread(void* arg)
{
	WSNCheck2* check2 = static_cast<WSNCheck2 *>(arg);
	check2->threaded_loop();

	pthread_detach(pthread_self());
	pthread_exit(0);
}

WSNCheck2::WSNCheck2(LiveDataBase *livedb, SPck *send, LoadConfig *conf)
{
	int n;

	m_livedb = livedb;
	m_send = send;
	m_conf = conf;

	/* create thread */
	if ((n = pthread_create(&m_thr, 0, start_check2_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (WSNCheck2): %s\n", strerror(n));
		return;
	} else
	{
		LOG(LG_INFO, "Created WSNCheck2 thread\n");
	}
}

WSNCheck2::~WSNCheck2()
{
}

void WSNCheck2::threaded_loop()
{
	while (!exit_request)
	//while (1)
	{
		//LOG(LG_DBG, "running WSNCheck2 thread\n");
		sleep(3);
	}
	LOG(LG_DBG, "Terminated WSNCheck2 thread\n");
}

