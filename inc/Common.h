/** 
 *
 * Created on:
 *   Nov 5, 2011
 *
 * Written or modified by:
 *   Mesut Yalcin 	<mes****@gmail.com>
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <list>
#include <iostream>
#include <fstream>

/**
 * TODO:: Enhance packet_t structure
 */

#ifdef __cplusplus
extern "C" {
#include "utils.h"
}
#endif

using namespace std;



/*typedef struct packet_raw_s {
	uint8_t len;
	char *buf;
} packet_raw_t;

typedef struct packet_s {
	uint8_t mid;
	uint8_t type;
	uint8_t value;
	uint8_t crc;
} packet_t;*/

#endif /* COMMON_H_ */
