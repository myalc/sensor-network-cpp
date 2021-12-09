/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include "StartUpWSN.h"

StartUpWSN::StartUpWSN(LiveDataBase *livedb, SPck *send, LoadConfig *conf)
{
	m_livedb 	= livedb;
	m_send 		= send;
	m_conf 		= conf;

	this->m_InitWSN();
}

StartUpWSN::~StartUpWSN()
{}

void StartUpWSN::m_InitWSN()
{
	packet_t* pck = NULL;
	int ret;
	list<active_modules_t*>::iterator it;
	list<active_modules_t*>* activeModules = m_livedb->get_active_modules();

	for (it = activeModules->begin(); it != activeModules->end(); it++)
	{
		pck = (packet_t*) utils_malloc(sizeof(packet_t));
		pck->mid = (*it)->id;
		pck->ptype = 101; 				// Set Module Config Response
		pck->value = (*it)->tosafe;
		do {
			ret = m_send->add2_sendList(pck);
		} while (ret == FAILURE);
	}
	m_livedb->free_active_modules(activeModules);
	LOG(LG_INFO, "Initialized WSN.\n");
}

void StartUpWSN::m_DeInitWSN()
{
	packet_t* pck = NULL;
	int ret;
	list<active_modules_t*>::iterator it;
	list<active_modules_t*>* activeModules = m_livedb->get_active_modules();

	for (it = activeModules->begin(); it != activeModules->end(); it++)
	{
		pck = (packet_t*) utils_malloc(sizeof(packet_t));
		pck->mid = (*it)->id;
		pck->ptype = 102; 					// Set Module Config
		pck->value = 0;
		do {
			ret = m_send->add2_sendList(pck);
		} while (ret == FAILURE);
	}
	m_livedb->free_active_modules(activeModules);
	LOG(LG_INFO, "DeInitialized WSN.\n");
}
