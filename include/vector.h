#ifndef VECTOR_H
#define VECTOR_H

#include <string.h>


template<typename T>
class vector {
public:
    vector() {}
	
	vector(size_t cap)
		: mSize(0)
		, mCapacity(cap)
		, mData(nullptr)
	{
		mData = new T[mCapacity];
	}
    
    vector(vector const &other)
        : mSize(other.mSize)
        , mCapacity(other.mCapacity)
        , mData(nullptr)
    {
		mData = new T[mCapacity];
        memcpy(mData, other.mData, mSize*sizeof(T)); 
    };

    ~vector() {
        delete[] mData;
		mData = nullptr;
    };

    vector &operator=(vector const &other) {
        delete[] mData;
        mSize = other.mSize; 
        mCapacity = other.mCapacity; 
        mData =  new T[mCapacity];
        memcpy(mData, other.mData, mSize*sizeof(T)); 
        return *this; 
    };

    void push_back(const T& x) {
        if (mCapacity == mSize) 
            resize(); 
        mData[mSize++] = x; 
    };

    //order doesn't matter for my purposes, swapping once with the last one is fine by me
    void erase(size_t idx) {
        if(idx >= mSize)
            return;
        const auto temp = mData[idx];
        mData[idx] = mData[--mSize];
        mData[mSize] = temp;
    }

    size_t size() const { 
        return mSize; 
	}
	
    T const &operator[](size_t idx) const { 
        return mData[idx]; 
    }
	
    T& operator[](size_t idx) { 
        return mData[idx];
	}

private:        
    void resize() {
        mCapacity = mCapacity ? mCapacity*2 : 1; 
        T* newData = new T[mCapacity]; 
        memcpy(newData, mData, mSize * sizeof(T)); 
        delete[] mData;
        mData = newData; 
    };
    
    size_t mSize{ 0 };
    size_t mCapacity{ 0 };
    T* mData { nullptr };

};

#endif // VECTOR_H