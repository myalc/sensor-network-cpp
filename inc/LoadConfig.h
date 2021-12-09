/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef LOADCONFIG_H_
#define LOADCONFIG_H_

#include "Common.h"

#define ROOTNODE "sens"
#define CONFNODE "conf"
#define CONF_CHECK_PERIOD_DEFAULT 120

class LoadConfig {
public:
	LoadConfig(const char *xmlfile);
	virtual ~LoadConfig();

	void threaded_loop();

private:
	void load_config(const char* xml_file);
	int add2conf(char* key, char *value);

	pthread_t m_thr;
	pthread_mutex_t m_mtx1;
	string m_xmlpath;
	int m_config_check_period;

	string m_logpath;
	int m_loglevel;
	string m_database_host;
	string m_database;
	string m_database_conn_str;
	string m_database_user;
	string m_database_psw;
	int m_seril_port;
	int m_check1_first_timeout;
	int m_check1_timeout;
	int m_check2_timeout;
	int m_check3_timeout;
	int m_livedb_update_period;

public:
	string get_logpath();
	int get_loglevel();
	string get_database_host();
	string get_database();
	string get_database_conn_str();
	string get_database_user();
	string get_database_psw();
	int get_seril_port();
	int get_check1_first_timeout();
	int get_check1_timeout();
	int get_check2_timeout();
	int get_check3_timeout();
	int get_livedb_update_period();
};

#endif /* LOADCONFIG_H_ */
