#pragma once

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif // !NODISCARD

template <typename T>
class AbstractDataStructureInterface {
public:
	inline virtual void insert(size_t index, T value) = 0;
	inline virtual void remove(size_t value) = 0;
	inline virtual void remove(size_t val1, size_t val2) = 0;
	inline virtual bool contains(T value) const = 0;
	NODISCARD inline virtual T front() const = 0;
	NODISCARD inline virtual T back() const = 0;
	inline virtual void clear() = 0;
	NODISCARD inline virtual size_t size() const = 0;
	inline virtual bool empty() const = 0;

	// ÐéÎö¹¹º¯Êý
	virtual ~AbstractDataStructureInterface() {}
};
