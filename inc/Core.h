/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef CORE_H_
#define CORE_H_

#include "Common.h"
#include "SPck.h"
#include "RPck.h"
#include "LoadConfig.h"
#include "LiveDataBase.h"
#include "InComing.h"
#include "StartUpWSN.h"
#include "WSNCheck1.h"
#include "WSNCheck2.h"
#include "WSNCheck3.h"

class Core {

public:
	Core(const char *_xmlconf);
	virtual ~Core();

public:
	void m_Deinit();

private:
	LoadConfig *m_conf;
	SPck *m_send;
	RPck *m_recv;
	LiveDataBase *m_livedb;

	StartUpWSN *m_startup;
	InComing *m_incoming;

	WSNCheck1 *m_check1;
	WSNCheck2 *m_check2;
	WSNCheck3 *m_check3;

	int m_recv_serverSock;
	int m_send_client_sock;
};

#endif /* CORE_H_ */
