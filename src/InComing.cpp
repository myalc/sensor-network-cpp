/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "InComing.h"

void* start_incoming_thread(void* arg)
{
	InComing* i = static_cast<InComing *>(arg);
	i->threaded_loop();

	pthread_detach(pthread_self());
	pthread_exit(0);
}

InComing::InComing(LiveDataBase *livedb, RPck *recv, LoadConfig *conf)
{
	int n;

	m_livedb = livedb;
	m_recv = recv;
	m_conf = conf;

	/* create thread */
	if ((n = pthread_create(&m_thr, 0, start_incoming_thread, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (InComing): %s\n", strerror(n));
		return;
	} else
	{
		LOG(LG_DBG, "Created InComing thread\n");
	}
}

InComing::~InComing()
{
}

void InComing::threaded_loop()
{
	while (!exit_request)
	//while (1)
	{
		//LOG(LG_DBG, "running InComing thread\n");
		sleep(3);
	}
	LOG(LG_DBG, "Terminated InComing thread\n");
}
