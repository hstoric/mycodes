#ifndef __LOCKGUARD_H__
#define __LOCKGUARD_H__

template <typename T>
class LockGuard final {
private:
	T &_lock;

public:
	inline LockGuard(T &lock) : _lock(lock) { _lock.lock(); }
	LockGuard(const LockGuard<T> &) = delete;
	LockGuard(const LockGuard<T> &&) = delete;

	inline ~LockGuard() { _lock.tryUnlock(); }

	LockGuard<T> &operator=(const LockGuard<T> &) = delete;
	LockGuard<T> &operator=(const LockGuard<T> &&) = delete;

	bool operator==(const LockGuard<T> &) const = delete;
	bool operator==(const LockGuard<T> &&) const = delete;

	inline void unlock() { _lock.unlock(); }
	inline void tryUnlock() { _lock.tryUnlock(); }
};

#endif
