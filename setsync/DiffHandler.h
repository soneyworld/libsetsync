/*
 * DiffHandler.h
 *
 *      Author: Till Lorentzen
 */

#ifndef DIFFHANDLER_H_
#define DIFFHANDLER_H_
#define _unused(x) ((void)x)
#include <vector>
#include <ostream>
#include <setsync/set.h>

namespace setsync {

/**
 * Instances of this abstract class must implement the handle method.
 * The handle method is called, if a local hash is saved and the remote
 * set doesn't contain this hash.
 */
class AbstractDiffHandler {
public:
	/**
	 * This method will be called if the given hash exists only on the local
	 * set or on the remote one. This can be called multiple times with the
	 * same key, if the bloom filter has multiple positions found where this
	 * hash hasn't been found. So duplicated calls must be handled by the
	 * implementation of the AbstractDiffHandler of the using instance.
	 *
	 * \param hash to be handled by a DiffHandler
	 * \param hashsize of the given hash in bytes
	 * \param existsLocally is true, if the returned hash has to be send to the remote set, false if it exists only on the remote side
	 */
	virtual void
	handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally) = 0;

	/**
	 * All instances can be called directly like a function. This calls
	 * directly the handle method. So it is just a little code saving method
	 *
	 * \param hash to be handled by this DiffHandler
	 * \param hashsize of the given hash in bytes
	 */
	void operator()(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally);
};

/**
 * A DiffHandler, which just ignores every diff and returns immediately.
 */
class IgnoringDiffHandler: public AbstractDiffHandler {
	/**
	 * Ignore all calls and return
	 */
	virtual void handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally) {
		_unused(hash);
		_unused(hashsize);
		_unused(existsLocally);
	}
};

/**
 * Simple list implementation for a AbstractDiffHandler. All distinct hashes
 * which are given by the handle method, are saved in a vector. So the size
 * can be used to get the number of distinct hashes. There are no multiple
 * entries per hash. The entries can be accessed like elements in an array.
 */
class ListDiffHandler: public AbstractDiffHandler {
private:
	/// list of hashes to be sent to remote set
	std::vector<std::pair<unsigned char *, bool> > list_;
public:
	/**
	 * Creates a simple DiffHandler which saves all distinct hashes, which should be
	 * handled.
	 */
	ListDiffHandler();

	/**
	 * Adds the given hash to the list of hashes, which should be sent to remote
	 * set. Only distinct hashes will be saved.
	 *
	 * \param hash which has to be sent to remote set
	 * \param hashsize of the given hash
	 */
	virtual void handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally);

	/**
	 * Destroys all allocated and copied hashes in the list
	 */
	virtual ~ListDiffHandler();

	/**
	 * \param index
	 * \return pointer to the hash or NULL if index is larger then returned hashes
	 */
	virtual const std::pair<unsigned char *, bool> operator[](
			unsigned int const& index) const;

	/**
	 * \return the count of the distinct hashes to be sent to remote
	 */
	virtual std::size_t size();

	/**
	 * Removes all entries from list
	 */
	virtual void clear(void);
};

/**
 * A simple implementation of AbstractDiffHandler to write each given hash to
 * the given output stream. Multiple hashes with the same value will be written
 * multiple times to the output stream. There is no filter for distinct hashes.
 */
class OutputStreamDiffHandler: public AbstractDiffHandler {
private:
	/// output stream to write the hashes to
	std::ostream& out_;
public:
	/**
	 * Constructor
	 *
	 * \param out output stream where the hashes will be written to
	 */
	OutputStreamDiffHandler(std::ostream& out);

	/**
	 * Destructor
	 */
	virtual ~OutputStreamDiffHandler();

	/**
	 * Writes the given hash to the output stream, specified on the constructor
	 *
	 * \param hash to be written to out
	 * \param hashsize of the given hash
	 */
	virtual void handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally);
};

/**
 * A simple Wrapper for ANSI C usage. If the plain C interface will be used,
 * this class is used to wrap the way a callback is implemented in C.
 */
class C_DiffHandler: public AbstractDiffHandler {
protected:
	/// Pointer to the calling instance
	void *closure_;
	/// Callback function pointer to be called by handle
	diff_callback * callback_;
public:

	/**
	 * Constructor of the C Wrapper. The given callback function
	 * will be called by the handle method and the given closure
	 * will be added as parameter to the diff_callback function.
	 *
	 * \param callback to be called by handler method
	 * \param closure pointer to the calling instance
	 */
	C_DiffHandler(diff_callback *callback, void *closure);

	/**
	 * Default destructor
	 */
	virtual ~C_DiffHandler();

	/**
	 * Calls directly the diff_callback with the given hash and hashsize.
	 * Also the closure will be added to the diff_callback function as
	 * parameter.
	 *
	 * \param hash
	 * \param hashsize
	 */
	virtual void handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally);
};
/**
 * A filtered version of the simple wrapper for c callbacks. A handled
 * hash will only be reported once. So duplicated hashes will be filtered.
 *
 * This implementation should only be chosen, if the difference is small
 * and the list of hashes fits into the memory. Otherwise, the normal
 * C_DiffHandler should be used, because it is stateless.
 */
class UniqueFilterDiffHandler: public C_DiffHandler {
private:
	/// list of hashes to which has been handled already
	std::vector<unsigned char *> list_;
public:
	/**
	 * Constructor of a filtered version of the C_DiffHandler.
	 * The given callback function will be called by the handle method and
	 * the given closure will be added as parameter to the diff_callback function.
	 *
	 * \param callback to be called by handler method
	 * \param closure pointer to the calling instance
	 */
	UniqueFilterDiffHandler(diff_callback *callback, void *closure);

	/**
	 * Default destructor cleans the list
	 */
	virtual ~UniqueFilterDiffHandler();
	/**
	 * Calls the diff_callback with the given hash and hashsize, if it is
	 * a new unhandled hash.
	 * Also the closure will be added to the diff_callback function as
	 * parameter.
	 *
	 * \param hash
	 * \param hashsize
	 */
	virtual void handle(const unsigned char * hash, const std::size_t hashsize,
			const bool existsLocally);
};
}

#endif /* DIFFHANDLER_H_ */
