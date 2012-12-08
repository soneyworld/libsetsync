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
	/**
	 * Returns the sum of all files in the given directory
	 * \param path which size should be returned
	 * \return sum size of the files in the given path
	 */
	static uint64_t dirSize(const std::string& path);
	/**
	 * Returns the sum of all files in the given directory
	 * \param path which size should be returned
	 * \return sum size of the files in the given path
	 */
	static uint64_t dirSize(const char *path);
	/**
	 * Creates a directory with the given prefix and returns
	 * the path of a unique created directory. It won't be
	 * deleted on program exit.
	 *
	 * \param prefix of the new directory
	 * \return the correct path of the newly created directory
	 */
	static std::string mkTempDir(const std::string& prefix);
	/**
	 * Creates a directory with the given prefix and returns
	 * the path of a unique created directory. It won't be
	 * deleted on program exit.
	 *
	 * \param prefix of the new directory
	 * \return the correct path of the newly created directory
	 */
	static std::string mkTempDir(const char * prefix);
	/**
	 * A class for simple using temporary created directories.
	 * All files and folder inside the newly created directory
	 * will also be deleted on destroying this object.
	 */
	class TemporaryDirectory {
	private:
		/// Path to the created directory
		std::string path_;
		/**
		 * Creates the new path from the prefix, saved in path
		 */
		void init();
	public:
		/**
		 * Creates a new directory with the given prefix
		 * and adds a 6 digit random number to it.
		 *
		 * \param prefix of the new folder
		 */
		TemporaryDirectory(const std::string& prefix);
		/**
		 * Creates a new directory with the given prefix
		 * and adds a 6 digit random number to it.
		 *
		 * \param prefix of the new folder
		 */
		TemporaryDirectory(const char * prefix);
		/**
		 * Deletes all files and directories existing in the
		 * temporary path, if creation has been successfully
		 */
		~TemporaryDirectory();
		/**
		 * \return the temporary directory path
		 */
		std::string getPath() const;
	};
};

}

#endif /* FILESYSTEM_H_ */
