#pragma once
#include <atomic>
#include <vector>
#include <mutex>
#ifdef _DEBUG
#include <iostream>

#endif // _DEBUG

template<class T>
class MemIterator // Only use this is the memory pool bcs for auto loop with cpp standard...
{
public:
	MemIterator(T* aDataPtr, T* aEndPtr) {
		myDataPtr = aDataPtr;
		myEndPtr = aEndPtr;
	}
	__forceinline bool operator==(const MemIterator& aOther) const
	{
		return myDataPtr == aOther.myDataPtr;
	}
	__forceinline bool operator!=(const MemIterator& aOther) const
	{
		return myDataPtr != aOther.myDataPtr;
	}
	__forceinline T& operator*()
	{
		return *myDataPtr;
	}
	__forceinline const T& operator*() const
	{
		return *myDataPtr;
	}
	__forceinline T* operator->()
	{
		return myDataPtr;
	}
	__forceinline const T* operator->() const
	{
		return myDataPtr;
	}
	MemIterator& operator++() // Prefix
	{
		static T nullObject = T();
		memset(&nullObject, 0x00, sizeof(T));
		do
		{
			myDataPtr++;
		} while (memcmp(myDataPtr, &nullObject, sizeof(T)) == 0 && myDataPtr != myEndPtr);
		return *this;
	}

	MemIterator operator++(int) // Postfix
	{
		static const T nullObject;
		memset(&nullObject, 0x00, sizeof(T));

		MemIterator iteratorCopy = *this;
		do
		{
			myDataPtr++;
		} while (memcmp(myDataPtr, &nullObject, sizeof(T)) == 0 && myDataPtr != myEndPtr);
		return iteratorCopy;
	}
private:
	T* myDataPtr = nullptr;
	T* myEndPtr = nullptr;
};

template<class T, size_t maxSize, bool threadSafe = true>
class MemoryPool
{
public:
	MemoryPool()
	{
		myMemoryPtr = (T*)malloc(maxSize * sizeof(T));
		myMemStepper = (unsigned char*)myMemoryPtr;
	}

	~MemoryPool()
	{
		free(myMemoryPtr);
	}
	T* StoreCopy(T* aValueToAdd)
	{
		myElementCount++;
		if (myElementCount >= maxSize)
		{
#ifdef _DEBUG
			std::cout << "Memory pool reached max size: " << myElementCount << " / " << maxSize << std::endl;
#endif // _DEBUG
			return nullptr;
		}
		
		if constexpr (threadSafe)
		{
			if (!myMemQueue.empty())
			{
				std::unique_lock<std::mutex> guard(myQueueLock); // WARNING: Empty test need to be checked after we locked to prevent race condition //Andre Bruce
				if (!myMemQueue.empty())
				{

					T* returnVal = myMemQueue[0];
					memcpy(returnVal, aValueToAdd, sizeof(T));

					myMemQueue.erase(myMemQueue.begin());
		
					return returnVal;
				}
			}
		}
		else
		{
			if (!myMemQueue.empty())
			{
				T* returnVal = myMemQueue[0];
				memcpy(returnVal, aValueToAdd, sizeof(T));
				myMemQueue.erase(myMemQueue.begin());
				return returnVal;
			}
		}
		const int byteOffset = (myNextFreeIndex++) * sizeof(T);
		memcpy((myMemStepper + byteOffset), aValueToAdd, sizeof(T));
		return (T*)(myMemStepper + byteOffset);
	}
	T* Create()
	{
		myElementCount++;
		if (myElementCount >= maxSize)
		{
#ifdef _DEBUG

			std::cout << "Memory pool reached max size: " << myElementCount << " / " << maxSize << std::endl;
#endif // _DEBUG
			return nullptr;
		}
		if constexpr (threadSafe)
		{
			if (!myMemQueue.empty())
			{
				std::unique_lock<std::mutex> guard(myQueueLock); // WARNING: Empty test need to be checked after we locked to prevent race condition //Andre Bruce
				if (!myMemQueue.empty())
				{
					T* const valueToAdd = new (myMemQueue[0]) T();
					myMemQueue.erase(myMemQueue.begin());
					return valueToAdd;
				}
			}
		}
		else
		{
			if (!myMemQueue.empty())
			{
				T* const valueToAdd = new (myMemQueue[0]) T();
				myMemQueue.erase(myMemQueue.begin());
				return valueToAdd;
			}
		}
		const int byteOffset = (myNextFreeIndex++) * sizeof(T);
		return new (myMemStepper + byteOffset) T();
	}
	inline void Remove(T* aItemToRemove)
	{
		myElementCount--;
		if constexpr(threadSafe)
		{
			std::unique_lock<std::mutex> guard(myQueueLock);
			myMemQueue.push_back(aItemToRemove);
			aItemToRemove->~T();
			memset(aItemToRemove, 0x00, sizeof(T));
		
		}
		else
		{
			myMemQueue.push_back(aItemToRemove);
			aItemToRemove->~T();
			memset(aItemToRemove, 0x00, sizeof(T));

		}
		
	}
	inline const size_t Size() const
	{
		return myElementCount;
	}
	__forceinline T* operator[](const size_t aIndex)
	{		
		return myMemoryPtr + aIndex;
	}
	__forceinline const T* operator[](const size_t aIndex) const
	{
		return myMemoryPtr + aIndex;
	}
	__forceinline void Clear()
	{	
		memset(myMemStepper, 0x00, sizeof(T) * maxSize);
		myMemStepper = (unsigned char*)myMemoryPtr;
		myElementCount = 0;
		myNextFreeIndex = 0;
		myMemQueue.clear();
	}
	inline MemIterator<T> begin() {
		return MemIterator<T>( myMemoryPtr, myMemoryPtr + myNextFreeIndex );
	}
	inline MemIterator<T> end() {
		return MemIterator<T>( myMemoryPtr + myNextFreeIndex, myMemoryPtr + myNextFreeIndex );
	}
	
private:
	std::mutex myQueueLock;
	std::atomic<int> myElementCount = 0;
	std::atomic<int> myNextFreeIndex = 0;
	T* myMemoryPtr = nullptr;
	std::atomic<unsigned char*> myMemStepper = nullptr;
	std::vector<T*> myMemQueue = {};
};

