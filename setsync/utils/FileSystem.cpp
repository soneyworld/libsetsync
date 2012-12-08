/*
 * FileSystem.cpp
 *
 *      Author: Till Lorentzen
 */

#include "FileSystem.h"
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <cstdio>

namespace utils {

int FileSystem::rmDirRecursive(const std::string& path) {
	return rmDirRecursive(path.c_str());
}

int FileSystem::rmDirRecursive(const char *path) {
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;
	if (d) {
		struct dirent *p;
		r = 0;
		while (!r && (p = readdir(d))) {
			int r2 = -1;
			char *buf;
			size_t len;
			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
				continue;
			}
			len = path_len + strlen(p->d_name) + 2;
			buf = (char*) malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode)) {
						r2 = rmDirRecursive(buf);
					} else {
						r2 = unlink(buf);
					}
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if (!r) {
		r = rmdir(path);
	}
	return r;
}
uint64_t FileSystem::dirSize(const std::string& path) {
	return dirSize(path.c_str());
}

uint64_t FileSystem::dirSize(const char* path) {
	std::size_t sum = 0;
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;
	if (d) {
		struct dirent *p;
		r = 0;
		while (p = readdir(d)) {
			char *buf;
			size_t len;
			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
				continue;
			}
			len = path_len + strlen(p->d_name) + 2;
			buf = (char*) malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode)) {
						sum += dirSize(buf);
					} else if (S_ISREG(statbuf.st_mode)) {
						sum += fileSize(buf);
					}
				}
				free(buf);
			}
		}
		closedir(d);
	}
	return sum;
}

uint64_t FileSystem::fileSize(const std::string& file) {
	return fileSize(file.c_str());
}
uint64_t FileSystem::fileSize(const char* file) {
	struct stat filestatus;
	stat(file, &filestatus);
	return filestatus.st_size;
}

static std::string mkTempDir(const std::string& prefix) {
	std::string path;
	char p[prefix.size() + 7];
	strncpy(p, prefix.c_str(), prefix.size());
	strncpy(p + prefix.size(), "XXXXXX", 6);
	p[prefix.size() + 6] = '\0';
	char * tempdir = mkdtemp(p);
	if (tempdir != NULL) {
		path = tempdir;
	} else {
		throw "couldn't create temporary directory";
	}
}

static std::string mkTempDir(const char * prefix) {
	return mkTempDir(std::string(prefix));
}

void FileSystem::TemporaryDirectory::init() {
	char p[this->path_.size() + 7];
	strncpy(p, path_.c_str(), path_.size());
	strncpy(p + path_.size(), "XXXXXX", 6);
	p[this->path_.size() + 6] = '\0';
	char * tempdir = mkdtemp(p);
	if (tempdir != NULL) {
		this->path_ = tempdir;
	} else {
		this->path_ = "";
		throw "couldn't create temporary directory";
	}
}

FileSystem::TemporaryDirectory::TemporaryDirectory(const std::string& prefix) :
	path_(prefix) {
	init();
}

FileSystem::TemporaryDirectory::TemporaryDirectory(const char * prefix) :
	path_(prefix) {
	init();
}
FileSystem::TemporaryDirectory::~TemporaryDirectory() {
	if (path_.size() > 6) {
		FileSystem::rmDirRecursive(path_);
	}
}
std::string FileSystem::TemporaryDirectory::getPath() const {
	return this->path_;
}
}
