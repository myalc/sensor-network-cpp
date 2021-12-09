/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef STARTUPWSN_H_
#define STARTUPWSN_H_

#include "Common.h"
#include "LiveDataBase.h"
#include "SPck.h"
#include "LoadConfig.h"

class StartUpWSN {
public:
	StartUpWSN(LiveDataBase *livedb, SPck *send, LoadConfig *conf);
	virtual ~StartUpWSN();

	/* send tosafe values to active modules, 101 */
	void m_InitWSN();
	/* send 0 to active modules, 102 */
	void m_DeInitWSN();

private:
	LiveDataBase *m_livedb;
	SPck *m_send;
	LoadConfig *m_conf;
};

#endif /* STARTUPWSN_H_ */
