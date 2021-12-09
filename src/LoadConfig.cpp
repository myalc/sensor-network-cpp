/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidxml.hpp"
#include "LoadConfig.h"

using namespace rapidxml;

void* start_load_config(void* arg)
{
	LoadConfig* load = static_cast<LoadConfig *>(arg);
	load->threaded_loop();

	pthread_detach(pthread_self());
	pthread_exit(0);
}

LoadConfig::LoadConfig(const char *xmlfile)
{
	int n;

	m_xmlpath = xmlfile;
	pthread_mutex_init(&m_mtx1, NULL);

	/* create thread */
	if ((n = pthread_create(&m_thr, 0, start_load_config, this)) != 0)
	{
		LOG(LG_ERR, "Pthread create error (LoadConfig): %s\n", strerror(n));
		return;
	} else
	{
		LOG(LG_INFO, "Created LoadConfig thread\n");
		load_config(m_xmlpath.c_str());
	}
	m_config_check_period = CONF_CHECK_PERIOD_DEFAULT;
}

void LoadConfig::threaded_loop()
{
	while(!exit_request)
	{
		sleep(m_config_check_period);
		load_config(m_xmlpath.c_str());
	}
}

void LoadConfig::load_config(const char* xml_file)
{
	int length, i;
	char *buffer;
	xml_document<> doc;
	xml_node<> *node;
    ifstream is;
    char conf[2][100];

	is.open(xml_file, ios::binary);

	is.seekg (0, ios::end);
	length = is.tellg();
	is.seekg (0, ios::beg);

	buffer = new char[length];

	is.read(buffer, length);
	is.close();

    doc.parse<0>(buffer);
    node = doc.first_node(ROOTNODE);
    node = node->first_node(CONFNODE);

    while (!exit_request)
    {
    	i = 0;
		for (xml_attribute<> *attr = node->first_attribute(); attr;
				attr = attr->next_attribute()) {
			strcpy(conf[i++], attr->value());
		}
		//cout << conf[0] << " " << conf[1] << "\n";
		add2conf(conf[0], conf[1]);
		node = node->next_sibling(CONFNODE);
		if (node == NULL)
			break;
	}
    LOG(LG_INFO, "Loaded all configuration parameters.\n");
	delete[] buffer;
}

int LoadConfig::add2conf(char* _key, char *_value)
{
	string key, value;

	key = _key;
	value = _value;

	pthread_mutex_lock(&m_mtx1);
	if (key.compare("logpath") == 0) {
		m_logpath = value;
	} else if (key.compare("loglevel") == 0) {
		m_loglevel = atoi(value.c_str());
	} else if (key.compare("database_host") == 0) {
		m_database_host = value;
	} else if (key.compare("database") == 0) {
		m_database = value;
	} else if (key.compare("database_conn_str") == 0) {
		m_database_conn_str = value;
	} else if (key.compare("database_user") == 0) {
		m_database_user = value;
	} else if (key.compare("database_psw") == 0) {
		m_database_psw = value;
	} else if (key.compare("seril_port") == 0) {
		m_seril_port = atoi(value.c_str());
	} else if (key.compare("check1_first_timeout") == 0) {
		m_check1_first_timeout = atoi(value.c_str());
	} else if (key.compare("check1_timeout") == 0) {
		m_check1_timeout = atoi(value.c_str());
	} else if (key.compare("check2_timeout") == 0) {
		m_check2_timeout = atoi(value.c_str());
	} else if (key.compare("check3_timeout") == 0) {
		m_check3_timeout = atoi(value.c_str());
	} else if (key.compare("livedb_update_period") == 0) {
		m_livedb_update_period = atoi(value.c_str());
	} else if (key.compare("config_check_period") == 0) {
		m_config_check_period = atoi(value.c_str());
	} else {
		printf("Invalid configuration parameter: %s\n", key.c_str());
	}
	pthread_mutex_unlock(&m_mtx1);

	return SUCCESS;
}

LoadConfig::~LoadConfig()
{
	/* delete some */
	pthread_mutex_destroy(&m_mtx1);
}

string LoadConfig::get_logpath()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_logpath;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_loglevel()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_loglevel;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

string LoadConfig::get_database_host()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_database_host;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

string LoadConfig::get_database()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_database;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

string LoadConfig::get_database_conn_str()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_database_conn_str;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

string LoadConfig::get_database_user()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_database_user;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

string LoadConfig::get_database_psw()
{
	string tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_database_psw;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_seril_port()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_seril_port;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_check1_first_timeout()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_check1_first_timeout;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_check1_timeout()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_check1_timeout;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_check2_timeout()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_check2_timeout;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_check3_timeout()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_check3_timeout;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}

int LoadConfig::get_livedb_update_period()
{
	int tmp;

	pthread_mutex_lock(&m_mtx1);
	tmp = m_livedb_update_period;
	pthread_mutex_unlock(&m_mtx1);

	return tmp;
}
