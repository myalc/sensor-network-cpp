/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef WSNCHECK1_H_
#define WSNCHECK1_H_

#include "Common.h"
#include "LiveDataBase.h"
#include "SPck.h"
#include "LoadConfig.h"

class WSNCheck1 {
public:
	WSNCheck1(LiveDataBase *livedb, SPck *send, LoadConfig *conf);
	virtual ~WSNCheck1();

	void threaded_loop();
public:
	pthread_t m_thr;
private:
	LiveDataBase *m_livedb;
	SPck *m_send;
	LoadConfig *m_conf;
};

#endif /* WSNCHECK1_H_ */
