#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <thread>
#include <cassert>
#include <stdexcept>

class Mutex final {
private:
	bool _locked = false;
#ifndef NDEBUG
	std::thread::id _ownerThreadId;
#endif

public:
	Mutex() = default;
	~Mutex() = default;

	Mutex(const Mutex &) = delete;
	Mutex(const Mutex &&) = delete;

	Mutex &operator=(const Mutex &) = delete;
	Mutex &operator=(const Mutex &&) = delete;

	bool operator==(const Mutex &) const = delete;
	bool operator==(const Mutex &&) const = delete;

	inline void lock() {
		while (_locked)
			std::this_thread::yield();
		_locked = true;
#ifndef NDEBUG
		_ownerThreadId = std::this_thread::get_id();
#endif
	}

	inline void unlock() {
#ifndef NDEBUG
		if (!_locked)
			throw std::logic_error("Unlocking an unlocked mutex");
		if (_ownerThreadId != std::this_thread::get_id())
			throw std::logic_error("Unlocking a mutex which is not owned by current thread");
#endif
		_locked = false;
	}

	inline bool tryLock() {
		if (_locked)
			return false;
		return _locked = true;
	}

	inline void tryUnlock() {
#ifndef _NDEBUG
		if (_ownerThreadId != std::this_thread::get_id())
			throw std::logic_error("Unlocking a mutex which is not owned by current thread");
#endif
		_locked = false;
	}

	inline bool isLocked() const {
		return _locked;
	}
};

#endif
