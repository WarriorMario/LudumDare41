#include "Utilities/ParallelForLoop.h"
#include "Typedefs.h"
#include "Math/MathUtilities.h"
#include <vector>
#include <thread>
#include <mutex>

// forward declerations
void WorkerThreadFunc(int32 index);
// the parallelforloop class, holds which loops still need to be executed
class ParallelForLoop
{
public:
    ParallelForLoop(std::function<void(int32)> func, int64 maxIndex, int32 chunkSize)
        :
        func(std::move(func)),
        maxIndex(maxIndex),
        chunkSize(chunkSize)
    {}

    bool Finished()
    {
        return nextIndex >= maxIndex &&
            activeWorkers == 0;
    }

public:
    // const data
    std::function<void(int32)> func;
    const int64 maxIndex;
    const int32 chunkSize;

    // dynamic data
    int64 nextIndex = 0;
    int32 activeWorkers = 0;
    ParallelForLoop* next = nullptr;
};

// the threads and wether they should die or not
static std::vector<std::thread> threads;
static bool shutdownThreads = false;

// the index of the current thread running the WorkerThreadFunc()
extern thread_local int32 threadIndex = 0;

// the loops you have to go through. Can be a linked list, but is usually all by itself
static ParallelForLoop* workList = nullptr;
// the mutex you must hold to access the work list
std::mutex workListMutex;

// the condition that the threads check to see if there's work
std::condition_variable workListCondition;

// the big boy, the function that's actually used
void ParallelFor(std::function<void(int32)>& func, int32 count, int32 chunkSize)
{
    // handle weird input for which no multithreading is necessary
    if (count < chunkSize)
    {
        for (int32 i = 0; i < count; ++i)
        {
            func(i);
        }
        return;
    }

    // open up worker threads if none exist yet
    if (threads.size() == 0)
    {
        int32 loops = NumSystemCores();
        for (int32 i = 0; i < loops -1; ++i)
        {
            threads.push_back(std::thread(WorkerThreadFunc, i + 1));
        }
    }

    // create the loop
    ParallelForLoop loop(func, count, chunkSize);
    workListMutex.lock();
    loop.next = workList;
    workList = &loop;
    workListMutex.unlock();

    // notivy workers of work to be done
    std::unique_lock<std::mutex> lock(workListMutex);
    workListCondition.notify_all();

    // do the actual work (per thread)
    while (loop.Finished() == false)
    {
        // find iteration range
        int64 indexStart = loop.nextIndex;
        int64 indexEnd = Min(indexStart + loop.chunkSize, loop.maxIndex);

        // update the loop
        loop.nextIndex = indexEnd;
        if (loop.nextIndex == loop.maxIndex)
        {
            workList = loop.next;
        }
        loop.activeWorkers++;

        // run your own work
        lock.unlock();
        if (loop.func)
        {
            for (int64 i = indexStart; i < indexEnd; i++)
            {
                loop.func((int32)i);
            }
        }
        lock.lock();

        // make sure the loop knows you're done

        loop.activeWorkers--;
    }
}

static void WorkerThreadFunc(int32 index)
{
    // set the thread index
    threadIndex = index;
    // create a local lock using the workListMutex
    std::unique_lock<std::mutex> lock(workListMutex);

    // the while loop this function is in permanently
    while (shutdownThreads == false)
    {
        if (!workList)
        {
            // sleep untill there is work for me
            workListCondition.wait(lock);
        }
        else
        {
            // get work from the workList and run my own iterations
            ParallelForLoop& loop = *workList;
            
            // find iteration range
            int64 indexStart = loop.nextIndex;
            int64 indexEnd = Min(indexStart + loop.chunkSize, loop.maxIndex);

            // update the loop
            loop.nextIndex = indexEnd;
            if (loop.nextIndex == loop.maxIndex)
            {
                workList = loop.next;
            }
            loop.activeWorkers++;

            // run your own work
            lock.unlock();
            if (loop.func)
            {
                for (int64 i = indexStart; i < indexEnd; i++)
                {
                    loop.func((int32)i);
                }
            }
            lock.lock();

            // make sure the loop knows you're done
            loop.activeWorkers--;

            // notify the rest if the loop is finished
            if (loop.Finished())
            {
                workListCondition.notify_all();
            }
        }
    }
}

int32 NumSystemCores()
{
  return Max(1u, std::thread::hardware_concurrency());
}

