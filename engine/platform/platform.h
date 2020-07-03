#ifndef MAPLE_PLATFORM_PLATFORM_H
#define MAPLE_PLATFORM_PLATFORM_H

typedef struct platform_window* window_t;
typedef struct file* file_t;
typedef struct thread_safe_memory* safe_memory_t;

typedef enum
{
    ConsoleColor_White,
    ConsoleColor_DarkGrey,
    ConsoleColor_Grey,
    ConsoleColor_DarkRed,
    ConsoleColor_Red,
    ConsoleColor_DarkGreen,
    ConsoleColor_Green,
    ConsoleColor_DarkBlue,
    ConsoleColor_Blue,
    ConsoleColor_DarkCyan,
    ConsoleColor_Cyan,
    ConsoleColor_DarkPurple,
    ConsoleColor_Purple,
    ConsoleColor_DarkYellow,
    ConsoleColor_Yellow,
} console_color;

// Platform Memory allocation
void* PlatformRequestMemory(u64 Size);
void PlatformReleaseMemory(void *Ptr, u64 Size);

// Safe memory allocations - only occur in global memory pools
void PlatformSafeMemoryAllocation(safe_memory_t *Memory, void *Ptr, u64 Size);
void PlaformSafeMemoryCopy(safe_memory_t Memory, u32 SrcOffset, void *Dst, u32 DstOffset, u64 CopySize);
void PlaformSafeMemoryWrite(safe_memory_t Memory, u32 DstOffset, void *Src, u32 SrcOffset, u64 WriteSize);
void PlatformSafeMemoryMemset(safe_memory_t Memory, u32 DstOffset, u32 Value, u32 WriteSize);
void PlatformSafeMemoryRelease(safe_memory_t *Memory);

// Log/Printing
#define mformat PlatformFormatString
inline void mprint(char *fmt, ...);
inline void mprinte(char *fmt, ...);
i32  PlatformFormatString(char *buff, i32 len, char* fmt, ...);
void PlatformPrintMessage(console_color text_color, console_color background_color, char* fmt, ...);
void PlatformPrintError(console_color text_color, console_color background_color, char* fmt, ...);

// File I/O
file_t PlatformLoadFile(const char *Filename, bool Append = false);
file_t PlatformOpenFile(const char *Filename, bool Append = false);
void* GetFileBuffer(file_t File);
void PlatformWriteToFile(file_t File, const char *Fmt, ...);
void PlatformWriteBinaryStreamToFile(file_t File, void *DataPtr, u64 DataSize);
void PlatformWriteBinaryToFile(file_t File, const char *Fmt, void *Data, u32 DataLen);
u64 PlatformGetFileSize(file_t File);
void PlatformFlushFile(file_t File);
void PlatformCloseFile(file_t File);
void PlatformCopyFileIfChanged(const char *Dst, const char *Src);

// Timing
u64 PlatformGetWallClock();
r32 PlatformGetElapsedSeconds(u64 Start, u64 End);

// Bit shifting
__inline u32 PlatformClzl(u64 Value);
u32 PlatformCtzl(u64 Value);

void PlatformGetClientWindowDimensions(u32 *Width, u32 *Height);

struct frame_params;

#define GAME_STAGE_ENTRY(fn) void fn(frame_params *FrameParams)
typedef GAME_STAGE_ENTRY(game_stage_entry);

#endif //MAPLE_PLATFORM_PLATFORM_H
