#include <cstring>                  // std::memcpy
#include <iostream>					// std::err

#include "colors.hpp"				// RED
#include "handleton.hpp"            // Handleton
#include "response_manager.hpp"     // ResponseManager                 
#include "async_functor.hpp"        // AsyncFunctor

using namespace std;
namespace ilrd_166_7
{

/***************************** AsyncFunctorRead *******************************/

AsyncFunctorRead::AsyncFunctorRead(const Uid& uid, const char* handle) : m_uid(uid), m_counter(0)
{
	memcpy(m_nbdUid, handle, S_NBD_SIZE);
}

bool AsyncFunctorRead::operator() ()
{
	auto response_mgr = Handleton<ResponseManager>::GetInstance();
	
	++m_counter;
	
	if (!response_mgr->IsUIDExists(m_uid))
	{
		return (false);
	}
	
	else if (m_counter > S_FAULT_TOLERANCE)
	{
		cerr << RED << "AsyncFunctorRead: FAILURE" << RESET << endl;
		Handleton<NBDProxy>::GetInstance()->StatusResponse(m_nbdUid, 1);
		return (false);
	}
	
	return (true);
}  

/***************************** AsyncFunctorWrite *******************************/

AsyncFunctorWrite::AsyncFunctorWrite(const Uid& main_uid, const Uid& backup_uid, const char* handle) : m_uid_1(main_uid), m_uid_2(backup_uid), m_counter(0)
{
	memcpy(m_nbdUid, handle, S_NBD_SIZE);
}

bool AsyncFunctorWrite::operator() () 
{
	auto response_mgr = Handleton<ResponseManager>::GetInstance();
	
	++m_counter;

	if (!response_mgr->IsUIDExists(m_uid_1) && !response_mgr->IsUIDExists(m_uid_2))
	{
		Handleton<NBDProxy>::GetInstance()->StatusResponse(m_nbdUid, 0);
		return (false);
	}
	
	else if (m_counter > S_FAULT_TOLERANCE)
	{
		cerr << RED << "AsyncFunctorWrite: FAILURE" << RESET << endl;
		Handleton<NBDProxy>::GetInstance()->StatusResponse(m_nbdUid, 1);
		return (false);
	}
		
	return (true);	
}
}