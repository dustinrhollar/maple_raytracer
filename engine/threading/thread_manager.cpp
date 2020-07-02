

void ThreadSafeMemoryInit(thread_safe_memory *Memory, void *Ptr, u64 Size)
{
    InitializeCriticalSection(&Memory->BufferWrite);
    
    EnterCriticalSection(&Memory->BufferWrite);
    {
        Memory->Size   = Size;
        Memory->Handle = Ptr;
    }
    LeaveCriticalSection(&Memory->BufferWrite);
}

void ThreadSafeMemoryFree(thread_safe_memory *Memory)
{
    EnterCriticalSection(&Memory->BufferWrite);
    {
        Memory->Size   = 0;
        Memory->Handle = NULL;
    }
    LeaveCriticalSection(&Memory->BufferWrite);
}

void ThreadSafeMemoryCopy(thread_safe_memory *Memory, u32 SrcOffset, void *Dst, u32 DstOffset, u64 CopySize)
{
    if (Dst && Memory->Handle)
    {
        EnterCriticalSection(&Memory->BufferWrite);
        {
            memcpy((char*)Dst + DstOffset, (char*)Memory->Handle + SrcOffset, CopySize);
        }
        LeaveCriticalSection(&Memory->BufferWrite);
    }
}

void ThreadSafeMemoryReverseWrite(thread_safe_memory *Memory, u32 DstOffset, void *Src, u32 SrcOffset, u64 WriteSize)
{
    if (Src && Memory->Handle)
    {
        u32 MaxWriteIdx = SrcOffset + WriteSize - 1;
        
        u32 MaxIdx = (WriteSize / sizeof(vec3)) - 1;
        
        EnterCriticalSection(&Memory->BufferWrite);
        {
            vec3 *Dst = (vec3*)((char*)Memory->Handle + DstOffset);
            vec3 *cSrc = (vec3*)((char*)Src + SrcOffset);
            
            for (u32 i = 0; i <= MaxIdx; ++i)
                Dst[i] = cSrc[MaxIdx - i];
            //memcpy((char*)Memory->Handle + DstOffset, (char*)Src + SrcOffset, WriteSize);
        }
        LeaveCriticalSection(&Memory->BufferWrite);
    }
}

void ThreadSafeMemoryWrite(thread_safe_memory *Memory, u32 DstOffset, void *Src, u32 SrcOffset, u64 WriteSize)
{
    if (Src && Memory->Handle)
    {
        EnterCriticalSection(&Memory->BufferWrite);
        {
            memcpy((char*)Memory->Handle + DstOffset, (char*)Src + SrcOffset, WriteSize);
        }
        LeaveCriticalSection(&Memory->BufferWrite);
    }
}

void ThreadSafeRingbufferInit(thread_safe_ringbuffer *Ringbuffer, free_allocator *Allocator, u32 MaxJobs)
{
    Ringbuffer->Head = 0;
    Ringbuffer->Tail = 0;
    Ringbuffer->JobsCap = MaxJobs;
    Ringbuffer->Jobs = palloc<thread_job>(Allocator, MaxJobs);
    
    InitializeConditionVariable(&Ringbuffer->BufferEmpty);
    InitializeCriticalSection(&Ringbuffer->BufferWrite);
}

void ThreadSafeRingbufferFree(thread_safe_ringbuffer *Ringbuffer, free_allocator *Allocator)
{
    EnterCriticalSection(&Ringbuffer->BufferWrite);
    {
        Ringbuffer->Head = 0;
        Ringbuffer->Tail = 0;
        Ringbuffer->JobsCap = 0;
        pfree<thread_job>(Allocator, Ringbuffer->Jobs);
        Ringbuffer->Jobs = NULL;
    }
    LeaveCriticalSection(&Ringbuffer->BufferWrite);
    WakeAllConditionVariable(&Ringbuffer->BufferEmpty);
}

// Push adds to the back of the ring.
// Pop removes from the front of the ring.
bool ThreadSafeRingbufferPush(thread_safe_ringbuffer *Ringbuffer, thread_job Job)
{
    bool Result = false;
    
    // Acquire mutex
    EnterCriticalSection(&Ringbuffer->BufferWrite);
    {
        u32 Next = (Ringbuffer->Head + 1) % Ringbuffer->JobsCap;
        if (Next != Ringbuffer->Tail)
        {
            Ringbuffer->Jobs[Ringbuffer->Head] = Job;
            Ringbuffer->Head = Next;
            Result = true;
        }
    }
    LeaveCriticalSection(&Ringbuffer->BufferWrite);
    
    WakeAllConditionVariable(&Ringbuffer->BufferEmpty);
    
    return Result;
}

bool ThreadSafeRingbufferPop(thread_safe_ringbuffer *Ringbuffer, thread_job *Job)
{
    bool Result = false;
    
    EnterCriticalSection(&Ringbuffer->BufferWrite);
    {
        if (Ringbuffer->Tail != Ringbuffer->Head)
        {
            *Job = Ringbuffer->Jobs[Ringbuffer->Tail];
            Ringbuffer->Tail = (Ringbuffer->Tail + 1) % Ringbuffer->JobsCap;
            Result = true;
        }
    }
    LeaveCriticalSection(&Ringbuffer->BufferWrite);
    
    return Result;
}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
    CRITICAL_SECTION DummyCritSection;
    InitializeCriticalSection(&DummyCritSection);
    
    thread_storage *Storage = (thread_storage*)lpParameter;
    
    while (true)
    {
        // Read into local stack if the thread needs to exit
        bool ShouldExitThread;
        EnterCriticalSection(&Storage->ThreadManager->ActiveWrite);
        {
            ShouldExitThread = !Storage->ThreadManager->IsActive;
        }
        LeaveCriticalSection(&Storage->ThreadManager->ActiveWrite);
        
        if (ShouldExitThread) break;
        
        thread_job Job;
        EnterCriticalSection(&DummyCritSection);
        if (!ThreadSafeRingbufferPop(&Storage->ThreadManager->Jobs, &Job))
        {
            mprint("Putting thread %d to sleep!\n", Storage->ThreadId);
            BOOL Ret = SleepConditionVariableCS(&Storage->ThreadManager->Jobs.BufferEmpty,
                                                &DummyCritSection, // NOTE(Dustin): Will this work?
                                                1000);
            if (!Ret)
            {
                DWORD Err = GetLastError();
                
                if (Err == ERROR_TIMEOUT)
                {
                    mprinte("thread %d sleep timeout...waking up!\n", Storage->ThreadId);
                }
                else
                {
                    mprinte("Error putting thread %d to sleep!\n", Storage->ThreadId);
                }
            }
            
            // We continue so that it can be checked agin if the thread manager
            // is still active. A thread can possible sleep all the way until
            // the manager is shutdown, in which case the Ringbuffer tells all
            // threads to wake up.
            continue;
        }
        LeaveCriticalSection(&DummyCritSection);
        
        
        mprint("Running a job on thread %d!\n", Storage->ThreadId);
        
        u32 BytesPerPixel = sizeof(vec3);
        
        // Do some stuff...sleep for 1s
        frame_params FrameParams = {};
        FrameParams.TextureWidth      = Storage->ImageWidth;
        FrameParams.TextureHeight     = Storage->ImageHeight;
        FrameParams.PixelXOffset      = Job.PixelXOffset;
        FrameParams.PixelYOffset      = Job.PixelYOffset;
        FrameParams.ScanWidth         = Job.ScanWidth;
        FrameParams.ScanHeight        = Job.ScanHeight;
        FrameParams.Camera            = Job.Camera;
        CopyAssets(&FrameParams.Assets, &FrameParams.AssetsCount, Job.AssetRegistry, &Storage->Heap);
        
        FrameParams.TextureBackbuffer = TaggedHeapBlockAlloc(&Storage->Heap, Job.ScanWidth * BytesPerPixel);
        assert(FrameParams.TextureBackbuffer && "Could not allocate memory for the backbuffer");
        
        
        u32 BaseXOffset = Storage->ImageWidth - Job.ScanWidth - Job.PixelXOffset;
        u32 BaseYOffset = Storage->ImageHeight - Job.PixelYOffset - 1; // - Job.ScanHeight
        for (u32 j = 0; j < Job.ScanHeight; ++j)
        {
            FrameParams.ScanHeight = 1;
            FrameParams.PixelYOffset = Job.PixelYOffset + j;
            Storage->Callback(&FrameParams);
            
            u32 ImageOffset
                = ((BaseYOffset - j) * Storage->ImageWidth * BytesPerPixel)
                + (Job.PixelXOffset * BytesPerPixel);
            
            ThreadSafeMemoryReverseWrite(Storage->Image, ImageOffset, FrameParams.TextureBackbuffer, 0,
                                         Job.ScanWidth * BytesPerPixel);
        }
        
        mprint("Finishing the job on thread %d!\n", Storage->ThreadId);
        
        // Reset the memory
        Storage->Heap.Brkp = Storage->Heap.Start;
    }
    
    // Clean up local thread storage items (if necessary)
    mprint("Exiting thread %d!\n", Storage->ThreadId);
    
    return 0;
}

void ThreadManagerInit(thread_manager   *Manager,
                       free_allocator   *Allocator,
                       u32               ThreadCount,
                       tagged_heap       *Heap,
                       game_stage_entry *Callback,
                       thread_safe_memory *Image,
                       u32 Width,
                       u32 Height)
{
    Manager->ThreadCount = ThreadCount;
    ThreadSafeRingbufferInit(&Manager->Jobs, Allocator, 1000);
    
    Manager->IsActive = true;
    InitializeCriticalSection(&Manager->ActiveWrite);
    
    Manager->HeapTag = { 0, TAG_ID_THREAD, 0 };
    Manager->Threads = palloc<HANDLE>(Allocator, Manager->ThreadCount);
    Manager->ThreadStorage = palloc<thread_storage>(Allocator, Manager->ThreadCount);
    for (u32 i = 0; i < Manager->ThreadCount; ++i)
    {
        Manager->ThreadStorage[i].ThreadId = i;
        Manager->ThreadStorage[i].ThreadManager = Manager;
        Manager->ThreadStorage[i].Callback = Callback;
        Manager->ThreadStorage[i].Heap = TaggedHeapRequestAllocation(Heap, Manager->HeapTag);
        Manager->ThreadStorage[i].Image = Image;
        Manager->ThreadStorage[i].ImageWidth = Width;
        Manager->ThreadStorage[i].ImageHeight = Height;
        
        Manager->Threads[i] = CreateThread(NULL,
                                           0,
                                           &ThreadProc,
                                           &Manager->ThreadStorage[i],
                                           0,
                                           NULL);
    }
    
    // TODO(Dustin): TEMPORARY - check to see if the active threads check work
    for (u32 i = 0; i < Manager->ThreadCount; ++i)
    {
        DWORD Result = WaitForSingleObject(&Manager->ThreadStorage[i], 0);
        if (Result == WAIT_OBJECT_0)
        {
            mprinte("Created the thread manager, but thread %d has terminated!\n");
        }
    }
}

void ThreadManagerFree(thread_manager *Manager, free_allocator *Allocator, tagged_heap *Heap)
{
    EnterCriticalSection(&Manager->ActiveWrite);
    {
        Manager->IsActive = false;
    }
    LeaveCriticalSection(&Manager->ActiveWrite);
    
    ThreadSafeRingbufferFree(&Manager->Jobs, Allocator);
    
    // Wait for all threads to complete
    DWORD ThreadWait = WaitForMultipleObjects(Manager->ThreadCount,
                                              Manager->Threads,
                                              TRUE,
                                              INFINITE);
    
    if (ThreadWait != WAIT_OBJECT_0)
    {
        if (ThreadWait == WAIT_ABANDONED_0)
        {
            mprinte("When waiting for threads to finish, WAIT_ABANDONED_0 was returned!\n");
        }
        else if (ThreadWait == WAIT_TIMEOUT)
        {
            mprinte("When waiting for threads to finish, WAIT_TIMEOUT was returned!\n");
        }
        else if (ThreadWait == WAIT_FAILED)
        {
            mprinte("When waiting for threads to finish, WAIT_FAILED was returned!\n");
        }
    }
    
    Manager->ThreadCount = 0;
    pfree<HANDLE>(Allocator, Manager->Threads);
    pfree<thread_storage>(Allocator, Manager->ThreadStorage);
    
    TaggedHeapReleaseAllocation(Heap, Manager->HeapTag);
}

bool ThreadManagerActive(thread_manager *Manager)
{
    bool Result = false;
    
    for (u32 i = 0; i < Manager->ThreadCount; ++i)
    {
        DWORD Result = WaitForSingleObject(&Manager->ThreadStorage[i], 0);
        if (Result != WAIT_OBJECT_0)
        {
            Result = true;
        }
    }
    
    return Result;
}

void ThreadManagerAddJob(thread_manager *Manager, thread_job Job)
{
    bool Ret = ThreadSafeRingbufferPush(&Manager->Jobs, Job);
    
    if (!Ret)
    {
        mprinte("Attempted to add a job to the ThreadManager, but the queue was full!\n");
    }
}
