///////////////////////////////////////////////////////////////
// @file            utildefine.h
// @author      gancan <gancan@kingsoft.com>
// @date            2013-07-12  10:44
// @brief          get source code from chromium
///////////////////////////////////////////////////////////////

#ifndef KBASE_UTILDEFINE_H_
#define KBASE_UTILDEFINE_H_

#define count_array_size(x) ( sizeof(x) / sizeof((x)[0]) )

#define k_max(a,b)  (((a) > (b)) ? (a) : (b))
#define k_min(a,b)  (((a) < (b)) ? (a) : (b))

// Put this in the private: declarations for a class to be uncopyable.
#define K_DISALLOW_COPY(TypeName) \
	TypeName(const TypeName&)

// Put this in the private: declarations for a class to be unassignable.
#define K_DISALLOW_ASSIGN(TypeName) \
	void operator=(const TypeName&)

// Put this in the private: declarations for a class to be not public construct.
#define K_DISALLOW_PUBLIC_CONSTRUCT(TypeName) \
    TypeName()

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define K_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

template<bool>
struct CompileAssert
{
};

// The macro can be used to verify that a compile time expression is true
#undef K_COMPILE_ASSERT
#define K_COMPILE_ASSERT(expr, msg) \
	typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]

///< get the function return address

#define GET_FUNC_RETURN_ADDRESS(dwRetAddress)       \
    do                                              \
{                                                   \
    __asm push eax                                  \
    __asm mov  eax, [ebp + 4]                       \
    __asm mov  dwRetAddress, eax                    \
    __asm pop  eax                                  \
} while(0);

#define GET_FUNC_RETURN_ADDRESS_LL(dwRetAddress)    \
    do                                              \
{                                                   \
    __asm push eax                                  \
    __asm mov  eax, [ebp + 0x0C]                    \
    __asm mov  dwRetAddress, eax                    \
    __asm pop  eax                                  \
} while(0);

// 以下两个转换函数是为了消除警告，并不是安全的
// 将 PBYTE转换为DWORD示例
// PBYTE addr;
// DWORD num = ptr_to_num_cast<DWORD>(addr)
// 将DWORD转换为PBYTE
// DWORD num;
// PBYTE addr = num_to_ptr_cast<PBYTE, DWORD_PTR>(num)
template<typename To, typename From>
inline To ptr_to_num_cast(From const &f)
{
    return (To) (PCHAR(f) - PCHAR(0));
}
template<typename PTR, typename NumPTR, typename From>
inline PTR num_to_ptr_cast(From const &f)
{
    return (PTR) (NumPTR) (f);
}

//

template<class Dest, class Source>
inline Dest bit_cast(const Source& source)
{
    // Compile time assertion: sizeof(Dest) == sizeof(Source)
    // A compile error here means your Dest and Source have different sizes.
    typedef char VerifySizesAreEqual[sizeof(Dest) == sizeof(Source) ? 1 : -1];

    Dest dest;
    memcpy(&dest, &source, sizeof(dest));
    return dest;
}


//////////////////////////////////////////////////////////////////////////
///< 定义几个常用的位操作宏
#define KIS_BIT_ENABLED(KEY, BIT) (((KEY) & (BIT)) != 0)
#define KIS_BIT_DISABLED(KEY, BIT) (((KEY) & (BIT)) == 0)
#define KIS_SET_BITS(KEY, BITS) (KEY |= (BITS))
#define KIS_CLR_BITS(KEY, BITS) (KEY &= ~(BITS))



#endif // KBASE_UTILDEFINE_H_
