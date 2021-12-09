/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */


#ifndef INCOMING_H_
#define INCOMING_H_

#include "Common.h"
#include "LiveDataBase.h"
#include "RPck.h"
#include "LoadConfig.h"

class InComing {
public:
	InComing(LiveDataBase *livedb, RPck *recv, LoadConfig *conf);
	virtual ~InComing();

	void threaded_loop();
public:
	pthread_t m_thr;
private:
	LiveDataBase *m_livedb;
	RPck *m_recv;
	LoadConfig *m_conf;
};

#endif /* INCOMING_H_ */
