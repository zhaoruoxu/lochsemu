#pragma once

#ifndef __CORE_LOCHSEMU_H__
#define __CORE_LOCHSEMU_H__

#pragma warning(disable: 4996 4099 4251)


const unsigned int LOCHSEMU_VERSION = 0x20120905;

//////////////////////////////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_LOCHSEMU()  namespace LochsEmu {
#define END_NAMESPACE_LOCHSEMU() }

/*
 * !!!ATTENTION!!!
 * DO NOT USE ANY STATEMENT IN 'Assert' MACRO, FOR 'Assert' WON'T BE COMPILED
 * IN RELEASE MODE
 * TRY USING  'B( ... )'  INSTEAD
 */
#ifdef Assert
#undef Assert
#endif
#define Assert _ASSERT

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT _ASSERT

#define INLINE  __inline

#ifdef _DEBUG
#ifdef new
#undef new
#endif // new
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif // _DEBUG

#ifndef V
#define V(x)                    { if ( LX_RESULT_SUCCESS != (x) ) { LxFatal(#x "\n"); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x)             { lr = (x); if ( LX_RESULT_SUCCESS != lr ) { LxReportError(lr); LxFatal(#x "\n"); return lr; } }
#endif

#ifndef B
#define B(x)                    { if (!(x) ) { LxFatal(#x "\n"); } }
#endif
#ifndef B_RETURN
#define B_RETURN(x)             { if ( !(x) ) { LxFatal(#x "\n"); return false; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)          { if (p) { delete (p);     (p)=NULL; } }
#endif  

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)    { if (p) { delete[] (p);   (p)=NULL; } }
#endif  

#ifndef SAFE_DELETE_VECTOR
#define SAFE_DELETE_VECTOR(v)   { if ((v).size() > 0) { for (uint i = 0; i < (v).size(); i++) delete (v)[i]; (v).clear(); } }
#endif

#define IN_RANGE(x, begin, size)  ((x) >= (begin) && (x) < (begin) + (size))

#define NOT_IMPLEMENTED()   LxFatal("%s(%d): %s  Not Implemented\n", __FILE__, __LINE__, __FUNCTION__)

#ifdef LOCHSEMU_EXPORTS
#define LX_API      __declspec(dllexport)
#else
#define LX_API      __declspec(dllimport)
#endif

#define INIT_8          0xcc
#define INIT_16         0xcccc
#define INIT_32         0xcccccccc
#define INIT_64         0xcccccccccccccccc

#define     LX_MAIN_MODULE      0
#define     LX_UNKNOWN_MODULE   (u32)-1

//////////////////////////////////////////////////////////////////////////
// typedefs
//////////////////////////////////////////////////////////////////////////

typedef __int8              i8;
typedef unsigned __int8     u8;
typedef __int16             i16;
typedef unsigned __int16    u16;
typedef __int32             i32;
typedef unsigned __int32    u32;
typedef __int64             i64;
typedef unsigned __int64    u64;
struct u128 { 
    u32 dat[4]; 
    u128() { ZeroMemory(dat, sizeof(u32)*4); }
    u128 &operator =(const u128 &rhs) { memcpy(dat, rhs.dat, sizeof(u32)*4); return *this; }
};
typedef unsigned char       byte;
typedef unsigned int        uint;

typedef i8*                 i8p;
typedef u8*                 u8p;
typedef i16*                i16p;
typedef u16*                u16p;
typedef i32*                i32p;
typedef u32*                u32p;
typedef i64*                i64p;
typedef u64*                u64p;
typedef u128*               u128p;
typedef byte*               pbyte;
typedef const byte*         cpbyte;

typedef DWORD               ThreadID;
typedef uint                HeapID;



//////////////////////////////////////////////////////////////////////////
// LochsEmu utility functions
//////////////////////////////////////////////////////////////////////////

BEGIN_NAMESPACE_LOCHSEMU()


#define LX_ALLOC_GRAN       0x10000
#define LX_PAGE_SIZE        0x1000
#define LX_PAGE_COUNT       0x100000
#define LX_PAGE_MASK        0xfffff000
#define PAGE_ALLOC_MASK     0xffff0000
#define PAGE_HIGH(x)        ((x) & LX_PAGE_MASK)
#define PAGE_LOW(x)         ((x) & ~LX_PAGE_MASK)
#define PAGE_NUM(x)         (((u32) x) >> 12)
#define PAGE_ADDR(x)        ((x) << 12)

#define LX_CHR_FREE         0x00000000
#define LX_CHR_RESERVED     0x00000001
#define LX_CHR_COMMITTED    0x00000002

class   PeLoader;
class   Section;
class   Heap;
class   Stack;
class   Emulator;
class   Process;
class   RefProcess;
class   Thread;
class   Processor;
class   Coprocessor;
struct  X86SIMD;
class   Memory;
class   Instruction;
class   PeModule;
struct  ModuleInfo;
struct  PageDesc;
struct  SectionDesc;
class   PluginManager;
class   Config;
class   ExceptionManager;
class   Mutex;
class   MutexCS;
class   MutexCSLock;
class   Semaphore;


enum LxResult : uint {
    LX_RESULT_SUCCESS = 0,
    LX_RESULT_INVALID_ADDRESS,
    LX_RESULT_INVALID_OPERATION,
    LX_RESULT_ACCESS_VIOLATION,
    LX_RESULT_PERMISSION_DENIED,
    LX_RESULT_NO_MEMORY,
    LX_RESULT_NOT_AVAILABLE,
    LX_RESULT_NOT_IMPLEMENTED,
    LX_RESULT_NOT_INITIALIZED,
    LX_RESULT_FILE_NOT_EXIST,
    LX_RESULT_ERROR_OPEN_FILE,
    LX_RESULT_ERROR_READ_FILE,
    LX_RESULT_ERROR_WRITE_FILE,
    LX_RESULT_INVALID_FORMAT,
    LX_RESULT_ERROR_INIT_STACK,
    LX_RESULT_ERROR_LOAD_MODULE,
};

/*
 * Error-proofing
 */
#ifdef FAILED
#undef FAILED
#endif
#define LX_FAILED(x)            ((x) != LX_RESULT_SUCCESS)
#define RET_SUCCESS()           return (LX_RESULT_SUCCESS)
#define RET_FAIL(x)             do { LxReportError(x); ;LxFatal("%d\n", x); return (x); } while(0)
#define RET_NOT_IMPLEMENTED()   RET_FAIL(LX_RESULT_NOT_IMPLEMENTED)

enum LogLevel {
    LOG_LEVEL_DEBUG     = 0,
    LOG_LEVEL_INFO      = 1,
    LOG_LEVEL_WARNING   = 2,
    LOG_LEVEL_FATAL     = 3,
};

typedef LxResult (*InstHandler)(Processor *cpu, const Instruction *inst);

/************************************************************************/
/* Log stuff                                                            */
/************************************************************************/
#ifdef _DEBUG
#define LxDebug( x, ... ) _LxDebug( x, __VA_ARGS__ )
#else
#define LxDebug( x, ... ) 
#endif

// #define LxFatal( x, ... ) do {LxWarning("%s(%d), in function %s\n", __FILE__, __LINE__, __FUNCTION__); \
//     _LxFatal( x, __VA_ARGS__ ); }while (0)

LX_API void             LxInfo(const char *format, ...);
LX_API void             _LxDebug(const char *format, ...);
LX_API void             LxWarning(const char *format, ...);
LX_API void             LxError(const char *format, ...);
LX_API void             LxFatal(const char *format, ...);
LX_API void             LxReportError(LxResult lr);

/************************************************************************/
/* Utility functions                                                    */
/************************************************************************/
LX_API const std::string &  LxGetRuntimeDirectory();
LX_API std::string      LxGetModuleDirectory(HMODULE hModule);
LX_API void             LxFormatTimeMS(DWORD time, char *buf);
LX_API void             LxFormatTimeS(DWORD time, char *buf);
LX_API void             LxGetDateTime(char *buffer);
LX_API bool             LxFileExists(LPCSTR filepath);
LX_API bool             LxWideToByte(LPCWSTR w, LPSTR b, int len);
LX_API bool             LxByteToWide(LPCSTR b, LPWSTR w, int len);
LX_API std::string      LxFilePath(const char *fullPath);
LX_API std::string      LxFileName(const char *fullPath);
LX_API std::string      LxModulePath(const char *name);
LX_API uint             LxNNStrLen(LPCWSTR ptr);
LX_API uint             LxNNStrLen(LPCSTR ptr);
LX_API INLINE bool      LxDecode(byte data[], Instruction *inst, u32 eip = 0);
extern "C" LX_API bool  LxDecodePInvoke(byte data[], Instruction *inst, u32 eip = 0);
extern "C" LX_API u32   LxVersion();
LX_API INLINE u32       LxRegConvert(u32 beaReg);

/************************************************************************/
/* Call these to rock'n'roll                                            */
/************************************************************************/
bool                    LxInit();
LX_API void             LxRun(int argc, LPSTR argv[]);
LX_API void             LxRun(int argc, LPWSTR argv[]);
LX_API void             LxReset();


/************************************************************************/
/* Global objects                                                       */
/************************************************************************/
LX_API extern Emulator     LxEmulator;
LX_API extern Config       LxConfig;


END_NAMESPACE_LOCHSEMU()


#endif // __CORE_LOCHSEMU_H__
