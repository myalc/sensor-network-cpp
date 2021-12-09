/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef WSNCHECK2_H_
#define WSNCHECK2_H_

#include "Common.h"
#include "LiveDataBase.h"
#include "SPck.h"
#include "LoadConfig.h"

class WSNCheck2 {
public:
	WSNCheck2(LiveDataBase *livedb, SPck *send, LoadConfig *conf);
	virtual ~WSNCheck2();

	void threaded_loop();
public:
	pthread_t m_thr;
private:
	LiveDataBase *m_livedb;
	SPck *m_send;
	LoadConfig *m_conf;
};

#endif /* WSNCHECK2_H_ */
