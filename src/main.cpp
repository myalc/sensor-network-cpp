/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#include <stdlib.h>
#include <signal.h>
#include <iostream>

#include "Core.h"

int exit_counter = 0;
volatile int exit_req = 0;
Core *core;

void signal_handler(int sig)
{
	LOG(LG_WARN, "Signal(1) (%d)\n", sig);
	exit_req = 1;
	exit_counter++;
	if (exit_counter > 1)
		//raise(SIGTERM);
		exit_request = 1;
}

void signal_handler2(int sig)
{
	LOG(LG_WARN, "Signal(2) (%d)\n", sig);
	exit(FAILURE);
}

void set_signals()
{
	signal(SIGABRT, signal_handler2);
	signal(SIGSEGV, signal_handler2);
	signal(SIGTERM, signal_handler2);
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
}

void usage(const char *arg)
{
	fprintf(stderr, "Usage: %s <conf file full path>\n", arg);
	exit(FAILURE);
}


int main(int argc, char const *argv[])
{

  	if (argc < 2)
		usage(argv[0]);

	set_signals();

	core = new Core(argv[1]);

	while (!exit_req)
		sleep(1);
	core->m_Deinit();

	sleep(5);
	exit_request = 1;

	delete core;
	LOG(LG_DBG, "Main program terminated.\n");
	std::cout << "Main program terminated." << std::endl;
	return SUCCESS;
}

