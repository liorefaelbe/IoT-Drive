#include <iostream>                 // std::cout
#include <cstring>                  // memcpy

#include "master_write_args.hpp"    // MasterWriteArgs
#include "master_read_args.hpp"     // MasterReadArgs
#include "master_commands.hpp"      // ReadCommand, WriteCommand
#include "handleton.hpp"            // Handleton
#include "response_manager.hpp"     // 
#include "raid_manager.hpp"         // GetMinionProxy
#include "uid.hpp"                  // Uid
#include "async_functor.hpp"        // AsyncFunctor

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** Read Command **********************************/

async_injection_functor ReadCommand::Run(shared_ptr<IKeyArgs> args)
{
    auto read_args = dynamic_pointer_cast<MasterReadArgs>(args);
    
    auto data = Handleton<RaidManager>::GetInstance()->GetMinionProxy(read_args->GetOffset());

    Uid commandUid;

    Handleton<ResponseManager>::GetInstance()->RegisterUID(commandUid, read_args->GetUid());

    data.m_original->Read(commandUid, data.m_originalOffset, read_args->GetSize());


    return (AsyncFunctorRead(commandUid, read_args->GetUid()));
}

shared_ptr<ICommand> ReadCommand::CreateReadCommand()
{
	return (make_shared<ReadCommand>());
}

/***************************** Write Command **********************************/

async_injection_functor WriteCommand::Run(shared_ptr<IKeyArgs> args)
{
    auto write_args =  dynamic_pointer_cast<MasterWriteArgs>(args);   

    auto data = Handleton<RaidManager>::GetInstance()->GetMinionProxy(write_args->GetOffset());

    Uid commandUid_1;
    Uid commandUid_2;

    auto responseManager = Handleton<ResponseManager>::GetInstance();
    responseManager->RegisterUID(commandUid_1, write_args->GetUid());
    responseManager->RegisterUID(commandUid_2, write_args->GetUid());

    data.m_original->Write(commandUid_1, data.m_originalOffset, write_args->GetSize(), write_args->GetBuffer());
    data.m_backup->Write(commandUid_2, data.m_backupOffset, write_args->GetSize(), write_args->GetBuffer());

    return (AsyncFunctorWrite(commandUid_1, commandUid_2, write_args->GetUid()));
}

shared_ptr<ICommand> WriteCommand::CreateWriteCommand()
{
	return (make_shared<WriteCommand>());
}
}