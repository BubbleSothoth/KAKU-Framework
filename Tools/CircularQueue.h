#pragma once

#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

#include "AbstractDataStructureInterface.h"
#include <queue>

using namespace std;

template <class Type>
class CircularQueue {
private:
	queue<Type> q;
public:
	inline void insert(Type value) {
		q.push(value);
	}
	inline void remove(size_t value) {
		size_t i = 0;
		while (true) {
			if (i++ == q.size()) {
				return;
			}
			auto data = q.front();
			q.pop();
			if (data == value) {
				return;
			}
			else {
				q.push(data);
			}
		}
		return;
	}
	NODISCARD inline Type front() {
		return q.front();
	}
	void pop() {
		q.pop();
	}
	inline void clear() {
		while (!q.empty()) {
			q.pop();
		}
	}
	NODISCARD inline size_t size() {
		return q.size();
	}
	inline bool empty() {
		return q.empty();
	}
	inline Type Access() {
		auto val = q.front();
		q.pop();
		q.push(val);
		return val;
	}
};