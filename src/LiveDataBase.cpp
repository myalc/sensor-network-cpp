/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include <pthread.h>
#include "LiveDataBase.h"

#define QUERY_LEN 100
#define UINT8LEN sizeof(uint8_t)
#define UINT32LEN sizeof(uint32_t)

static char *server_options[] = { (char*) "label_client",
		(char*) "--defaults-file=/etc/my.cnf" };
int num_elements = sizeof(server_options) / sizeof(char *);
static char *server_groups[] = { (char*)"mysqld", (char*)"client" };


LiveDataBase::LiveDataBase(LoadConfig *conf)
{
	init_mysql_library();
	m_conn_read = connect2db(conf->get_database_host().c_str(),
			conf->get_database_user().c_str(),
			conf->get_database_psw().c_str(), conf->get_database().c_str());
	m_conn_write = connect2db(conf->get_database_host().c_str(),
			conf->get_database_user().c_str(),
			conf->get_database_psw().c_str(), conf->get_database().c_str());

	if (m_conn_read == NULL || m_conn_read == NULL) {
		raise(SIGTERM);
	}

	/* insert data from disk based tables to memory based tables */
	init_db_events();
	init_mem_tables();

#if 0 // test 
	// mem_modules
	list<active_modules_t*> *l;
	l = get_active_modules();
	free_active_modules(l);
	get_tosafe(1, 2);
	get_towarning(1, 2);
	get_tocritical(1, 2);
	get_todanger(1, 2);

	// mem_thresholds
	get_safeth(1, 2);
	get_warningth(1, 2);
	get_criticalth(1, 2);
	get_dangerth(1, 2);

	// mem_module_data
	list<uint8_t> *ll;
	ll = get_check1_list(1234657699);
	free_check_list(ll);
	ll = get_check2_list(2121);
	free_check_list(ll);
	ll = get_check3_list(4343);
	free_check_list(ll);
	get_status(9);
	get_counter(9);

	set_status(7, 1);
	set_counter(7, 1);
	set_counter_incremet(9);
	update_module_data(99, 1212, 2, 3);
	update_module_data(99, 1212, 7, 3);
	insert_module_data_all(1,2,55,121212);

	return;
#endif

	m_conf = conf;
}

LiveDataBase::~LiveDataBase()
{
	sync_db_tables();
	LOG(LG_DBG, "Database tables synchronized.\n");
	deinit_db_events();
	closedb(m_conn_read);
	closedb(m_conn_write);
	deinit_mysql_library();
}

void LiveDataBase::init_mysql_library()
{
	/* Initialize the MySQL C API library */
	if (mysql_library_init(num_elements, server_options, server_groups))
	{
		utils_log(LG_ERR, "Could not initialize MySQL library\n");
		exit(FAILURE);
	}
	pthread_mutex_init(&m_write_mtx, NULL);
	utils_log(LG_INFO, "Initialized MySQL library\n");
}

void LiveDataBase::deinit_mysql_library()
{
	mysql_library_end();
	pthread_mutex_destroy(&m_write_mtx);
	utils_log(LG_INFO, "Finalized the MySQL C API library.\n");
}

MYSQL* LiveDataBase::connect2db(const char *db_host, const char *db_user,
		const char *db_passw, const char *db_name)
{
	MYSQL* ress = NULL;

	ress = mysql_init(NULL);
	if (ress == NULL) {
		LOG(LG_ERR, "mysql Error: connection is NULL!\n");
		return NULL;
	}
	mysql_options(ress, MYSQL_READ_DEFAULT_GROUP, "client");
	mysql_options(ress, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);

	if ((mysql_real_connect(ress, db_host, db_user, db_passw, db_name, 3306,
			"/tmp/mysql.sock", 0)) == NULL) {
		LOG(LG_ERR, "Cannot establish db connection! Error: %s\n", mysql_error(ress));
		return NULL;
	} else {
		LOG(LG_INFO, "Established db connection!\n");
		return ress;
	}
}

void LiveDataBase::closedb(MYSQL *conn)
{
	mysql_close(conn);
	LOG(LG_INFO, "Closed db connection!\n");
}

MYSQL_RES* LiveDataBase::select_query(MYSQL *conn, char *query)
{
	MYSQL_RES *results;

	/*printf("query: %s\n", query);*/
	mysql_query(conn, query);
	results = mysql_store_result(conn);

	return results;
}

uint8_t LiveDataBase::get_uint8value(char *query)
{
	MYSQL_RES *result;
	MYSQL_ROW record;
	uint8_t val = 0;

	result = select_query(m_conn_read, query);
	while((record = mysql_fetch_row(result)))
	{
		val = atoi(record[0]);
		/*cout << (int) val << endl;*/
	}
	mysql_free_result(result);

	return val;
}

list<uint8_t> *LiveDataBase::get_check_list(char *query)
{
	list<uint8_t> *check_list;
	MYSQL_RES *result;
	MYSQL_ROW record;
	uint8_t v;

	check_list  = new list<uint8_t>;
	result = select_query(m_conn_read, query);
	while((record = mysql_fetch_row(result)))
	{
		v = atoi(record[0]);
		check_list->push_back(v);
		/*cout << (int) v << endl;*/
	}
	mysql_free_result(result);

	return check_list;
}

void LiveDataBase::update_query(char *stmt)
{
	/*cout << stmt << endl;*/
	pthread_mutex_lock(&m_write_mtx);
	if (mysql_query(m_conn_write, stmt) != 0)
	{
		utils_log(LG_ERR, "Error while executing query! Error: %s\n",
				mysql_error(m_conn_write));
	}
	pthread_mutex_unlock(&m_write_mtx);
	/*printf("%ld row(s) updated\n", (long) mysql_affected_rows(m_conn_write));*/
}

void LiveDataBase::init_mem_tables()
{
	char q[QUERY_LEN];

	sprintf(q, "call p_init_module_data();");
	update_query(q);

	sprintf(q, "call p_init_modules();");
	update_query(q);

	sprintf(q, "call p_init_thresholds();");
	update_query(q);
}

void LiveDataBase::init_db_events()
{
	char q[QUERY_LEN];

	sprintf(q, "SET GLOBAL event_scheduler = ON");
	update_query(q);
}

void LiveDataBase::deinit_db_events()
{
	char q[QUERY_LEN];

	sprintf(q, "SET GLOBAL event_scheduler = OFF");
	update_query(q);
}

void LiveDataBase::sync_db_tables() {
	char q[QUERY_LEN];

	sprintf(q, "call p_sync();");
	update_query(q);
}


/* packet_lookup */
/* (uygulamadan herhangi bir sorgu almıyor)*/

/* mem_modules */
list<active_modules_t*>* LiveDataBase::get_active_modules()
{
	list<active_modules_t*> *actives;
	MYSQL_RES *result;
	MYSQL_ROW record;
	active_modules_t *data = NULL;

	actives = new list<active_modules_t*>;
	result = select_query(m_conn_read, (char*) "select id, type, tosafe from "
			"mem_modules where active = 1 and type != 1");
	while((record = mysql_fetch_row(result)))
	{
		data = (active_modules_t*) utils_malloc(sizeof(active_modules_t));

		data->id = atoi(record[0]);
		data->type = atoi(record[1]);
		data->tosafe = atoi(record[2]);
		actives->push_back(data);
		/*cout << (int) data->id << "\t" << (int) data->type << "\t" << (int) data->tosafe << endl;*/
	}
	mysql_free_result(result);

	return actives;
}

void LiveDataBase::free_active_modules(list<active_modules_t*> *actives)
{
	list<active_modules_t*>::iterator it;
	active_modules_t *tmp;

	for (it = actives->begin(); it != actives->end(); ++it) {
		tmp = *it;
		free(tmp);
	}
	actives->clear();
	delete actives;
}

uint8_t LiveDataBase::get_tosafe(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select tosafe from mem_modules where type = %d "
			"and id = %d and active = 1", type, id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_towarning(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select towarning from mem_modules where type = %d "
			"and id = %d and active = 1", type, id);

	return get_uint8value(q);
	//select towarning from mem_modules where type = t and id=i and active = 1;
}

uint8_t LiveDataBase::get_tocritical(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select tocritical from mem_modules where type = %d "
			"and id = %d and active = 1", type, id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_todanger(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select todanger from mem_modules where type = %d "
			"and id = %d and active = 1", type, id);

	return get_uint8value(q);
}


/* mem_thresholds */
uint8_t LiveDataBase::get_safeth(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select safe from mem_thresholds where type = %d and id = %d",
			type, id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_warningth(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select warning from mem_thresholds where type = %d and id = %d",
			type, id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_criticalth(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select critical from mem_thresholds where type = %d and id = %d",
			type, id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_dangerth(uint8_t id, uint8_t type)
{
	char q[QUERY_LEN];

	sprintf(q, "select danger from mem_thresholds where type = %d and id = %d",
			type, id);

	return get_uint8value(q);
}


/* mem_module_data */
list<uint8_t> *LiveDataBase::get_check1_list(uint32_t epoch)
{
	char q[QUERY_LEN];

	sprintf(q, "select id from mem_module_data where epoch < %d "
			"and status = %d", epoch, 0);

	return get_check_list(q);
}

list<uint8_t> *LiveDataBase::get_check2_list(uint32_t epoch)
{
	char q[QUERY_LEN];

	sprintf(q, "select id from mem_module_data where epoch < %d "
			"and status = %d", epoch, 1);

	return get_check_list(q);
}

list<uint8_t> *LiveDataBase::get_check3_list(uint32_t epoch)
{
	char q[QUERY_LEN];

	sprintf(q, "select id from mem_module_data where epoch < %d "
			"and status = %d", epoch, 2);

	return get_check_list(q);
}

void LiveDataBase::free_check_list(list<uint8_t> *check)
{
	check->clear();
	delete check;
}

uint8_t LiveDataBase::get_status(uint8_t id)
{
	char q[QUERY_LEN];

	sprintf(q, "select status from mem_module_data where id = %d", id);

	return get_uint8value(q);
}

uint8_t LiveDataBase::get_counter(uint8_t id)
{
	char q[QUERY_LEN];

	sprintf(q, "select counter from mem_module_data where id = %d", id);

	return get_uint8value(q);
}

void LiveDataBase::set_status(uint8_t id, uint8_t val)
{
	char q[QUERY_LEN];

	sprintf(q,"update mem_module_data set status = %d, sync = 1 where id = %d", val, id);
	update_query(q);
}

void LiveDataBase::set_counter(uint8_t id, uint8_t val)
{
	char q[QUERY_LEN];

	sprintf(q,"update mem_module_data set counter = %d, sync = 1 where id = %d", val, id);
	update_query(q);
}

void LiveDataBase::set_counter_incremet(uint8_t id)
{
	char q[QUERY_LEN];

	sprintf(q,"update mem_module_data set counter=counter+1, sync = 1 where id = %d", id);
	update_query(q);
}

void LiveDataBase::update_module_data(uint8_t id, uint32_t epoch,
		uint8_t status, uint8_t counter)
{
	char q[QUERY_LEN];

	sprintf(q,"call p_update_module_data(%d,%d,%d,%d)", id, epoch, status, counter);
	update_query(q);
}


/* module_data_all */
void LiveDataBase::insert_module_data_all(uint8_t id, uint8_t type,
		uint8_t value, uint32_t epoch)
{
	char q[QUERY_LEN];

	sprintf(q,"call p_insert_module_data_all(%d,%d,%d,%d);", id, type, value, epoch);
	update_query(q);
}
