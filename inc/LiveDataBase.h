/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef LIVEDATABASE_H_
#define LIVEDATABASE_H_

#include "Common.h"
#include "LoadConfig.h"

#ifdef __cplusplus
extern "C" {
#include "mysql.h"
}
#endif

/**
 * Active module structure used in list
 */
typedef struct active_modules_s {
	uint8_t id;
	uint8_t type;
	uint8_t tosafe;
} active_modules_t;


class LiveDataBase {
public:
	LiveDataBase(LoadConfig *conf);
	virtual ~LiveDataBase();

private:
	LoadConfig *m_conf;
	MYSQL *m_conn_read;
	MYSQL *m_conn_write;
	pthread_mutex_t m_write_mtx;

	/**
	 * Initialize mysql library
	 */
	void init_mysql_library();

	/**
	 * De-Initialize mysql library
	 */
	void deinit_mysql_library();

	/**
	 * Connect to mysql database
	 * @param db_host database host
	 * @param db_user database user
	 * @param db_passw database password
	 * @param db_name database name
	 * @return mysql connection
	 */
	MYSQL* connect2db(const char *db_host, const char *db_user,
			const char *db_passw, const char *db_name);

	/**
	 * Close mysql connection
	 * @param conn mysql connection
	 */
	void closedb(MYSQL *conn);

	/**
	 * Performs select query
	 * @param conn mysql connection
	 * @param query query
	 */
	MYSQL_RES* select_query(MYSQL *conn, char *query);

	/**
	 * Gets uint8_t value from database
	 * @param query query
	 */
	uint8_t get_uint8value(char *query);

	/**
	 * Get check lists
	 * @param query query
	 * @return list
	 */
	list<uint8_t> *get_check_list(char *query);

	/**
	 * Performs update query
	 * @param stmt query
	 */
	void update_query(char *stmt);

	/**
	 * Initialize memory tables
	 */
	void init_mem_tables();

	/**
	 * Initialize database events
	 */
	void init_db_events();

	/**
	 * De-initialize database events
	 */
	void deinit_db_events();

	/**
	 * Synchronize database tables before terminate program
	 */
	void sync_db_tables();

public:
	/********************* packet_lookup *********************/
	/* (uygulamadan herhangi bir sorgu almıyor) */

	/********************* mem_modules *********************/
	/**
	 * Gets active modules from database
	 * @return active module list
	 */
	list<active_modules_t*>* get_active_modules();

	/**
	 * Frees active module list
	 * @param actives active module list
	 */
	void free_active_modules(list<active_modules_t*> *actives);

	/**
	 * Gets safe timeout value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_tosafe(uint8_t id, uint8_t type);

	/**
	 * Gets warning timeout value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_towarning(uint8_t id, uint8_t type);

	/**
	 * Gets critical timeout value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_tocritical(uint8_t id, uint8_t type);

	/**
	 * Gets danger timeout value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_todanger(uint8_t id, uint8_t type);


	/********************* mem_thresholds *********************/
	/**
	 * Gets safe threshold value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_safeth(uint8_t id, uint8_t type);

	/**
	 * Gets warning threshold value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_warningth(uint8_t id, uint8_t type);

	/**
	 * Gets critical threshold value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_criticalth(uint8_t id, uint8_t type);

	/**
	 * Gets danger threshold value from database
	 * @param id id
	 * @param type type
	 */
	uint8_t get_dangerth(uint8_t id, uint8_t type);

	/********************* mem_module_data *********************/
	/**
	 * Gets check1 list from database
	 * @param epoch epoch
	 * @return check1 list
	 */
	list<uint8_t> *get_check1_list(uint32_t epoch);

	/**
	 * Gets check2 list from database
	 * @param epoch epoch
	 * @return check1 list
	 */
	list<uint8_t> *get_check2_list(uint32_t epoch);

	/**
	 * Gets check3 list from database
	 * @param epoch epoch
	 * @return check1 list
	 */
	list<uint8_t> *get_check3_list(uint32_t epoch);

	/**
	 * Frees check lists
	 * @param check list
	 */
	void free_check_list(list<uint8_t> *check);

	/**
	 * Gets status value from database
	 * @param id id
	 * @return status
	 */
	uint8_t get_status(uint8_t id);

	/**
	 * Gets counter value from database
	 * @param id id
	 * @return counter
	 */
	uint8_t get_counter(uint8_t id);

	/**
	 * Sets status value
	 * @param id id
	 * @param val value
	 */
	void set_status(uint8_t id, uint8_t val);

	/**
	 * Sets counter value
	 * @param id id
	 * @param val value
	 */
	void set_counter(uint8_t id, uint8_t val);

	/**
	 * Increments counter value
	 * @param id id
	 */
	void set_counter_incremet(uint8_t id);

	/**
	 * Updates module data
	 * @param id id
	 * @param epoch epoch
	 * @param status status
	 * @param counter counter
	 */
	void update_module_data(uint8_t id, uint32_t epoch, uint8_t status,
			uint8_t counter);

	/********************* module_data_all *********************/
	/**
	 * Inserts module data
	 * @param id id
	 * @param type type
	 * @param value value
	 * @param epoch epoch
	 */
	void insert_module_data_all(uint8_t id, uint8_t type, uint8_t value,
			uint32_t epoch);
};

#endif /* LIVEDATABASE_H_ */
