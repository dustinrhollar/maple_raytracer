#ifndef ENGINE_THREADING_THREAD_MANAGER_H
#define ENGINE_THREADING_THREAD_MANAGER_H

// Tracks the value of a semaphore
struct atomic_counter
{
    CONDITION_VARIABLE Signal;
    CRITICAL_SECTION   CounterWrite;
    u32                Value;
};

// Create a semaphore with the max value
void AtomicCounterInit(atomic_counter *Counter, u32 InitialValue);
// Waits until the specified value is reached by the counter
void AtomicCounterWait(atomic_counter *Counter, u32 Value);
void AtomicCounterIncrement(atomic_counter *Counter);
void AtomicCounterDecrement(atomic_counter *Counter);


struct thread_safe_memory
{
    void            *Handle;
    u64              Size;
    CRITICAL_SECTION BufferWrite;
};

void ThreadSafeMemoryInit(thread_safe_memory *Memory, void *Ptr, u64 Size);
void ThreadSafeMemoryFree(thread_safe_memory *Memory);
// Copy a section of the thread_safe_memory to another location
void ThreadSafeMemoryCopy(thread_safe_memory *Memory, u32 SrcOffset, void *Dst, u32 DstOffset, u64 CopySize);
// Copy another buffer into the thread_safe_emory
void ThreadSafeMemoryWrite(thread_safe_memory *Memory, u32 DstOffset, void *Src, u32 SrcOffset, u64 WriteSize);
void ThreadSafeMemoryMemset(thread_safe_memory *Memory, u32 DstOffset, u32 Value, u32 WriteSize);

struct thread_job
{
    u32              ImageOffset;
    u32              ImageWidth;
    u32              ImageHeight;
    u32              PixelXOffset;
    u32              PixelYOffset;
    u32              ScanWidth;    // stored outside of render_settings because is dependent on job
    u32              ScanHeight;   // stored outside of render_settings because is dependent on job
    render_settings *Settings;
};

struct thread_safe_ringbuffer
{
    thread_job        *Jobs;
    u32                JobsCap;
    u32                Head;
    u32                Tail;
    CRITICAL_SECTION   BufferWrite;
    CONDITION_VARIABLE BufferEmpty;
};

void ThreadSafeRingbufferInit(thread_safe_ringbuffer *Ringbuffer, free_allocator *Allocator);
void ThreadSafeRingbufferFree(thread_safe_ringbuffer *Ringbuffer, free_allocator *Allocator);
void ThreadSafeRingbufferClear(thread_safe_ringbuffer *Ringbuffer);

// Push adds to the back of the ring.
// Pop removes from the front of the ring.
bool ThreadSafeRingbufferPush(thread_safe_ringbuffer *Ringbuffer, thread_job *Job);
bool ThreadSafeRingbufferPop(thread_safe_ringbuffer *Ringbuffer, thread_job *Job);

struct thread_storage
{
    game_stage_entry     **Callback;
    
    struct thread_manager *ThreadManager; // Needed to check if the manager is still active
    tagged_heap_block      Heap;
    
    thread_safe_memory    *Image;
    u32                    ImageWidth;
    u32                    ImageHeight;
    
    u32                    ThreadId;
};

struct thread_manager
{
    // number of threads currently running a job
    atomic_counter         ActiveThreads;
    
    u32                    ThreadCount;
    HANDLE                *Threads;
    thread_storage        *ThreadStorage;
    
    thread_safe_ringbuffer Jobs;
    tag_id_t               HeapTag;
    
    bool                   IsActive;
    SRWLOCK                IsManagerActiveLock;
};

void ThreadManagerInit(thread_manager *Manager,
                       free_allocator *Allocator,
                       u32 ThreadCount,
                       tagged_heap       *Heap,
                       game_stage_entry **Callback,
                       thread_safe_memory *Image,
                       u32 Width,
                       u32 Height);
void ThreadManagerFree(thread_manager *Manager, free_allocator *Allocator, tagged_heap *Heap);

// Checks to see if any of the threads are active
bool ThreadManagerActive(thread_manager *Manager);
void ThreadManagerAddJob(thread_manager *Manager, thread_job Job);
void ThreadManagerClearJobs(thread_manager *Manager);
void ThreadManagerWaitForJobs(thread_manager *Manager, u32 Value);

#endif //ENGINE_THREADING_THREAD_MANAGER_H
