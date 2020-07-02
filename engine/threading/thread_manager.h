#ifndef ENGINE_THREADING_THREAD_MANAGER_H
#define ENGINE_THREADING_THREAD_MANAGER_H

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

struct thread_job
{
    u32   ImageOffset;
    u32   ImageWidth;
    u32   ImageHeight;
    u32   PixelXOffset;
    u32   PixelYOffset;
    u32   ScanWidth;
    u32   ScanHeight;
    camera *Camera;
    asset_registry *AssetRegistry; // needed to copy the assets over into local mem
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

// Push adds to the back of the ring.
// Pop removes from the front of the ring.
bool ThreadSafeRingbufferPush(thread_safe_ringbuffer *Ringbuffer, thread_job *Job);
bool ThreadSafeRingbufferPop(thread_safe_ringbuffer *Ringbuffer, thread_job *Job);

struct thread_storage
{
    game_stage_entry      *Callback;
    struct thread_manager *ThreadManager; // Needed to check if the manager is still active
    tagged_heap_block      Heap;
    
    thread_safe_memory    *Image;
    u32                    ImageWidth;
    u32                    ImageHeight;
    
    u32                    ThreadId;
};

struct thread_manager
{
    u32                    ThreadCount;
    HANDLE                *Threads;
    thread_storage        *ThreadStorage;
    
    thread_safe_ringbuffer Jobs;
    tag_id_t HeapTag;
    
    bool IsActive;
    CRITICAL_SECTION ActiveWrite;
};

void ThreadManagerInit(thread_manager *Manager,
                       free_allocator *Allocator,
                       u32 ThreadCount,
                       tagged_heap       *Heap,
                       game_stage_entry *Callback,
                       thread_safe_memory *Image,
                       u32 Width,
                       u32 Height);
void ThreadManagerFree(thread_manager *Manager, free_allocator *Allocator, tagged_heap *Heap);

// Checks to see if any of the threads are active
bool ThreadManagerActive(thread_manager *Manager);
void ThreadManagerAddJob(thread_manager *Manager, thread_job Job);

#endif //ENGINE_THREADING_THREAD_MANAGER_H
