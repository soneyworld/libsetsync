/*
 * FileSystem.h
 *
 *      Author: Till Lorentzen
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_
#include <inttypes.h>
#include <string>
namespace utils {
/**
 * Utilities for file system operations
 */
class FileSystem {
public:
	/**
	 * Removes a complete directory recursively
	 * \param path to the directory, which should be deleted
	 * \return 0 on success
	 */
	static int rmDirRecursive(const std::string& path);
	/**
	 * Removes a complete directory recursively
	 * \param path to the directory, which should be deleted
	 * \return 0 on success
	 */
	static int rmDirRecursive(const char *path);
	/**
	 * Returns the size of a given file
	 * \param file which size should be returned
	 * \return size of the given file
	 */
	static uint64_t fileSize(const std::string& file);
	/**
	 * Returns the size of a given file
	 * \param file which size should be returned
	 * \return size of the given file
	 */
	static uint64_t fileSize(const char *file);
};

}

#endif /* FILESYSTEM_H_ */
