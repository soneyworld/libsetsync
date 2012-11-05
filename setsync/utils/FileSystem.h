/*
 * FileSystem.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

namespace utils {

class FileSystem {
public:
	static int rmDirRecursive(const char *path);
};

}

#endif /* FILESYSTEM_H_ */
