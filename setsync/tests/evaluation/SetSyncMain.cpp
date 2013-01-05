/*
 * SetSyncMain.cpp
 *
 *      Author: Till Lorentzen
 */
#include "config.h"
#include "StopWatch.h"
#include "Test.h"
#include "SetSync.h"
#include <string>
#include <list>
#include <setsync/Set.hpp>
#include <setsync/config/Configuration.h>
int main(int ac, char **av) {
	std::list<std::string> args;
	std::list<std::string>::iterator iter;
	for (int i = 1; i < ac; i++) {
		std::string arg(av[i]);
		args.push_front(arg);
	}
}
