#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "AbstractDataStructureInterface.h"

using namespace std;

class ByteStream : AbstractDataStructureInterface<BYTE>
{
private:
	vector<BYTE> stream;
public:
	ByteStream(void) {}
	ByteStream(const BYTE* lp, size_t length)
	{
		stream = vector<BYTE>(length);
		for (int i = 0; i < length; i++) {
			stream[i] = *(lp + i);
		}
	}

	ByteStream(const ByteStream&) = default;

	~ByteStream(void) = default;

	inline void push_back(BYTE val) { stream.push_back(val); }

	inline string str() {
		string _str;
		for (int i = 0; i < stream.size(); i++) {
			_str.push_back(stream[i]);
		}
		return _str;
	}

	inline vector<BYTE>::iterator begin(void) { return stream.begin(); }

	inline vector<BYTE>::iterator end(void) { return stream.end(); }

	void operator=(const string val) {
		for (int i = 0; i < val.size(); i++) {
			this->push_back(val[i]);
		}
	}

	inline virtual void insert(size_t index, BYTE value) { stream.insert(stream.begin() + index, value); }

	inline virtual void remove(size_t value) { stream.erase(stream.begin() + value); }

	inline virtual void remove(size_t val1, size_t val2) {
		auto it = stream.begin();
		stream.erase(it + val1, it + val2); 
	}

	inline virtual bool contains(BYTE value) const {
		for (auto it = stream.begin(); it != stream.end(); it++) {
			if (value == *it) { return true; }
		}
		return false;
	}

	NODISCARD inline virtual BYTE front() const { return stream.front(); }

	NODISCARD inline virtual BYTE back() const { return stream.back(); }

	inline virtual void clear() { stream.clear(); }

	NODISCARD inline virtual size_t size() const { return stream.size(); }

	inline virtual bool empty() const { return stream.empty(); }

	void GetByteList(BYTE* lp_byte, size_t size = UINT64_MAX) {
		if (size == UINT64_MAX) { size = this->size(); }
		for (int i = 0; i < size; i++) {
			*(lp_byte + i) = stream[i];
		}
		return;
	}
};

typedef ByteStream bstring;