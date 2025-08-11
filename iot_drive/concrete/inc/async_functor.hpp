#ifndef __ILRD_ASYNC_FUNCTOR_HPP__
#define __ILRD_ASYNC_FUNCTOR_HPP__

#include "uid.hpp"  // Uid

/**
 * @class  AsyncFunctor
 * 
 * @author Lior Reafael Berkovits
 * @date   09/06/2025
 */

/**************************** Implementations *********************************/

namespace ilrd_166_7
{
struct AsyncFunctorRead
{
public:
    static const int S_FAULT_TOLERANCE = 30;
    const static unsigned int S_NBD_SIZE = 8;

    AsyncFunctorRead(const Uid& uid, const char* handle);

    bool operator()();
     
private:
    Uid m_uid;
    int m_counter;
    char m_nbdUid[S_NBD_SIZE];
}; // AsyncFunctorRead

struct AsyncFunctorWrite
{
public:
    static const int S_FAULT_TOLERANCE = 30;
    const static unsigned int S_NBD_SIZE = 8;

    AsyncFunctorWrite(const Uid& main_uid, const Uid& backup_uid, const char* handle);

    bool operator()();
     
private:
    Uid m_uid_1;
    Uid m_uid_2;
    int m_counter;
    char m_nbdUid[S_NBD_SIZE];
}; // AsyncFunctorWrite
} // ilrd_166_7

#endif // __ILRD_ASYNC_FUNCTOR_HPP__