/********************************************************************************************
 * Twitch Broadcasting SDK
 *
 * This software is supplied under the terms of a license agreement with Justin.tv Inc. and
 * may not be copied or used except in accordance with the terms of that agreement
 * Copyright (c) 2012-2013 Justin.tv Inc.
 *********************************************************************************************/

#ifndef TTVSDK_CONCURRENT_QUEUE
#define TTVSDK_CONCURRENT_QUEUE

#include <queue>
#include <mutex>


// Concurrent queue implementation for Mac which uses a regular std::queue with a std::mutex. Note that it
// doesn't support all the features of the standard concurrent_queue (for example it doesn't take an allocator
// template parameter and also doesn't implement the unsafe_* methods).
//
namespace Concurrency
{
	template <typename item_t> class concurrent_queue;
}

template <class item_t>
class Concurrency::concurrent_queue
{
public:
	concurrent_queue() {}
	~concurrent_queue() {}

public:
	void clear()
    {
        std::lock_guard<std::mutex> lock(mMutex);
        while (!mQueue.empty())
        {
            mQueue.pop();
        }
    }
    
	bool empty() const
    {
        std::lock_guard<std::mutex> lock(mMutex);
        return mQueue.empty();
    }

	void push(const item_t& src)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mQueue.push(src);
    }
    
	bool try_pop(item_t& dest)
    {
        bool ret = false;

        std::lock_guard<std::mutex> lock(mMutex);
        if (!mQueue.empty())
        {
            dest = mQueue.front();
            mQueue.pop();
            ret = true;
        }
        
        return ret;
    }
	
	/**
	 * Retrieves the size of the queue.  This is not thread safe so use it only to determine the approximate size of the queue.
	 */
	size_t unsafe_size() const
	{
		return mQueue.size();
	}
	

private:
	std::queue<item_t> mQueue;
    mutable std::mutex mMutex;
};

#endif // TTVSDK_CONCURRENT_QUEUE
