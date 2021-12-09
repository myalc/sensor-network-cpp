/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef WSNCHECK3_H_
#define WSNCHECK3_H_

#include "Common.h"
#include "LiveDataBase.h"
#include "SPck.h"
#include "LoadConfig.h"

class WSNCheck3 {
public:
	WSNCheck3(LiveDataBase *livedb, SPck *send, LoadConfig *conf);
	virtual ~WSNCheck3();

	void threaded_loop();
public:
	pthread_t m_thr;
private:
	LiveDataBase *m_livedb;
	SPck *m_send;
	LoadConfig *m_conf;
};

#endif /* WSNCHECK3_H_ */
