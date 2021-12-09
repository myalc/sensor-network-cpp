/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "WSNCheck1.h"

void* start_check1_thread(void* arg)
{
	WSNCheck1* check1 = static_cast<WSNCheck1 *>(arg);
	check1->threaded_loop();

	pthread_detach(pthread_self());
	pthread_exit(0);
}

WSNCheck1::WSNCheck1(LiveDataBase *livedb, SPck *send, LoadConfig *conf)
{
	int n;

	m_livedb = livedb;
	m_send = send;
	m_conf = conf;

	/* create thread */
	if ((n = pthread_create(&m_thr, 0, start_check1_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (WSNCheck1): %s\n", strerror(n));
		return;
	} else
	{
		LOG(LG_INFO, "Created WSNCheck1 thread\n");
	}
}

WSNCheck1::~WSNCheck1()
{
}

void WSNCheck1::threaded_loop()
{
	while (!exit_request)
	//while (1)
	{
		//LOG(LG_DBG, "running WSNCheck1 thread\n");
		sleep(3);
	}
	LOG(LG_DBG, "Terminated WSNCheck1 thread\n");
}
