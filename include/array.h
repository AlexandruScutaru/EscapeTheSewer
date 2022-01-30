#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>

// This is an array with a fixed size set via templates
// I need the project to not dynamically allocate these and also limit the count of items depending on the case
// memory is expensive

template<typename T, uint8_t sz>
class array {
public:
	array()
		: mSize(0)
		, mCapacity(sz)
	{}

    ~array() = default;

    bool push_back(const T& x) {
        if (mCapacity == mSize)
            return false;
        mData[mSize++] = x;
        return true;
    };

    //order doesn't matter for my purposes, swapping once with the last one is fine by me
    void erase(uint8_t idx) {
        if(idx >= mSize)
            return;
        const auto temp = mData[idx];
        mData[idx] = mData[--mSize];
        mData[mSize] = temp;
    }

    void clear() {
        mSize = 0;
    }

    uint8_t size() const { 
        return mSize; 
	}
	
    T const &operator[](uint8_t idx) const { 
        return mData[idx]; 
    }
	
    T& operator[](uint8_t idx) { 
        return mData[idx];
	}

private:            
    T mData[sz];
    uint8_t mSize{ 0 };
    uint8_t mCapacity{ 0 };

};

#endif // ARRAY_H
