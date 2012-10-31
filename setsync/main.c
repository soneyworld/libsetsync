/*
 * main.c
 *
 *  Created on: 01.08.2012
 *      Author: Till Lorentzen
 */

#include "set.h"

#include <stdio.h>

int main( int argc, const char* argv[] )
{
	int result = 0;
	SET s;
	result = set_init(&s, NULL,1000,1,0.01);

	result = set_free(&s);
	return result;
}

