#ifndef __DYNARRAY_HH__
#define __DYNARRAY_HH__

#include <cstdint>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <memory>

template <typename T>
class DynArray {
public:
	struct Iterator {
		size_t index;
		DynArray *array;

		inline Iterator(const Iterator &it) : index(it.index) {}
		inline Iterator(const Iterator &&it) : index(it.index) {}
		inline Iterator(size_t index) : index(index) {}

		inline Iterator &operator=(const Iterator &rhs) noexcept {
			index.rhs = index;
			return *this;
		}

		inline Iterator &operator=(const Iterator &&rhs) noexcept {
			index.rhs = index;
			return *this;
		}

		inline Iterator &operator++() {
			if ((index++) > array->_len)
				throw std::logic_error("Increasing the end iterator");
			return *this;
		}

		inline Iterator operator++(int) {
			Iterator it = *this;
			++(*this);
			return it;
		}

		inline Iterator &operator--() {
			if (!(index--))
				throw std::logic_error("Dereasing the begin iterator");
			return *this;
		}

		inline Iterator operator--(int) {
			Iterator it = *this;
			--(*this);
			return it;
		}

		inline bool operator==(const size_t index) const noexcept {
			return this->index == index;
		}

		inline bool operator==(const Iterator &it) const {
			return index == it.index;
		}

		inline bool operator==(const Iterator &&rhs) const {
			const Iterator it = rhs;
			return *this == it;
		}

		inline bool operator!=(const size_t index) const noexcept {
			return this->index != index;
		}

		inline bool operator!=(const Iterator &it) const {
			return index != it.index;
		}

		inline bool operator!=(Iterator &&rhs) const {
			Iterator it = rhs;
			return *this != it;
		}

		inline T &operator*() {
			if (index >= array->_len)
				throw std::logic_error("Deferencing the end iterator");
			return array->_elements[index];
		}

		inline const T &operator*() const {
			if (index >= array->_len)
				throw std::logic_error("Deferencing the end iterator");
			return array->_elements[index];
		}

		inline T *operator->() {
			if (index >= array->_len)
				throw std::logic_error("Deferencing the end iterator");
			return array->_elements + index;
		}

		inline const T *operator->() const {
			if (index >= array->_len)
				throw std::logic_error("Deferencing the end iterator");
			return array->_elements + index;
		}
	};

	template <bool doAppend>
	inline size_t _insert(size_t begin, size_t size) {
		if constexpr (doAppend) {
			assert(begin < _len);
		}

		size_t newSize = _len + size;

		std::unique_ptr<T[]> newElements(new T[newSize]);

		if (_elements) {
			size_t idxLatterElements;
			if constexpr (doAppend)
				idxLatterElements = begin + size + 1;
			else
				idxLatterElements = begin + size;

			if constexpr (std::is_literal_type<T>::value) {
				if constexpr (doAppend) {
					// Copy former elements.
					memcpy(newElements.get(), _elements, (begin + 1) * sizeof(T));
					// Copy latter elements.
					if (idxLatterElements < newSize)
						memcpy(newElements.get() + idxLatterElements, newSize - idxLatterElements);

					// `begin' will be used as the result.
					++begin;
				} else {
					// Copy former elements.
					if (begin)
						memcpy(newElements.get(), _elements, (begin) * sizeof(T));
					// Copy latter elements.
					memcpy(newElements.get() + idxLatterElements, _elements + begin, newSize - idxLatterElements);
				}
			} else {
				if constexpr (doAppend) {
					// Copy former elements.
					for (size_t i = 0; i < begin; ++i)
						newElements.get()[i] = std::move(_elements[i]);
					// Copy latter elements.
					for (size_t i = idxLatterElements; i < newSize; ++i)
						newElements.get()[i] = std::move(_elements[i]);

					// `begin' will be used as the result.
					++begin;
				} else {
					// Copy former elements.
					for (size_t i = 0; i < begin; ++i)
						newElements.get()[i] = std::move(_elements[i]);
					// Copy latter elements.
					for (size_t i = idxLatterElements; i < newSize; ++i)
						newElements.get()[i] = std::move(_elements[i]);
				}
			}

			delete _elements;
		}
		_elements = newElements.release();
		_len = newSize;

		return begin;
	}

	/// @brief Remove elements from range [begin, end)
	/// @param begin Index of the first element to be removed.
	/// @param end Next index of the last element to be removed.
	inline void _remove(size_t begin, size_t end) {
		assert(begin < end);
		assert(begin < _len);
		assert(end < _len);

		size_t newSize = _len - begin - end;
		if (!newSize) {
			delete _elements;
			_elements = nullptr;
		} else {
			std::unique_ptr<T[]> newElements(new T[newSize]);

			if (begin) {
				if constexpr (std::is_literal_type<T>::value) {
					memcpy(newElements.get(), _elements, begin * sizeof(T));
				} else {
					for (size_t i = 0; i < begin; ++i)
						newElements.get()[i] = std::move(_elements[i]);
				}
			}

			if (end < _len) {
				if constexpr (std::is_literal_type<T>::value) {
					memcpy(newElements.get() + begin, _elements + end, _len - end);
				} else {
					const size_t nElementsToCopy = _len - end;
					for (size_t i = 0; i < nElementsToCopy; ++i) {
						newElements[begin + i] = std::move(_elements[end + i]);
					}
				}
			}

			if (_elements)
				delete _elements;
			_elements = newElements.release();
		}
		_len = newSize;
	}

	inline void _remove(size_t where) {
		_remove(where, where + 1);
	}

protected:
	size_t _len = 0;
	T *_elements = nullptr;

public:
	inline DynArray() {
	}

	inline ~DynArray() {
		if (_elements)
			delete[] _elements;
	}

	inline Iterator begin() {
		return Iterator(0);
	}
	inline Iterator end() {
		return Iterator(_len);
	}

	inline Iterator prepend(Iterator where, T data) {
		auto index = _insert<false>(where.index, 1);
		_elements[index] = data;

		return Iterator(index);
	}
	inline Iterator append(Iterator where, T data) {
		auto node = _insert<true>(where.index, 1);
		node->v = data;

		return Iterator(node);
	}

	inline void remove(Iterator where) {
		_remove(where.index);
	}
	inline void remove(Iterator begin, Iterator end) {
		_remove(begin.index, end.index);
	}
	inline void remove(Iterator begin, size_t nElements) {
		_remove(begin.index, begin.index + nElements);
	}

	inline T &at(size_t i) {
		if (i >= _len)
			throw std::out_of_range("Out of array range");
		return _elements[i];
	}

	inline const T &at(size_t i) const {
		if (i >= _len)
			throw std::out_of_range("Out of array range");
		return _elements[i];
	}

	inline T get(size_t i) {
		return at(i);
	}

	inline const T get(size_t i) const {
		return at(i);
	}

	inline void clear() {
		delete[] _elements;
		_elements = nullptr;
	}

	inline size_t size() const {
		return _len;
	}
};

#endif
