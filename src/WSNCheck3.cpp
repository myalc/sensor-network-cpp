/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "WSNCheck3.h"

void* start_check3_thread(void* arg)
{
	WSNCheck3* check3 = static_cast<WSNCheck3 *>(arg);
	check3->threaded_loop();

	pthread_detach(pthread_self());
	pthread_exit(0);
}


WSNCheck3::WSNCheck3(LiveDataBase *livedb, SPck *send, LoadConfig *conf)
{
	int n;

	m_livedb = livedb;
	m_send = send;
	m_conf = conf;

	/* create thread */
	if ((n = pthread_create(&m_thr, 0, start_check3_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (WSNCheck3): %s\n", strerror(n));
		return;
	} else
	{
		LOG(LG_INFO, "Created WSNCheck3 thread\n");
	}
}

WSNCheck3::~WSNCheck3()
{
}

void WSNCheck3::threaded_loop()
{
	while (!exit_request)
	//while (1)
	{
		//LOG(LG_DBG, "running WSNCheck3 thread\n");
		sleep(3);
	}
	LOG(LG_DBG, "Terminated WSNCheck3 thread\n");
}
