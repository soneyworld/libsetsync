/*
 * FileSystem.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include <inttypes.h>
namespace utils {

class FileSystem {
public:
	static int rmDirRecursive(const char *path);
	static uint64_t fileSize(const char *file);
};

}

#endif /* FILESYSTEM_H_ */
