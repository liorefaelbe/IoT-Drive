#include "minion_write_args.hpp"    // MinionWriteArgs
#include "minion_read_args.hpp"     // MinionReadArgs
#include "response_message.hpp"     // AResponseMessage
#include "handleton.hpp"            // Handleton
#include "master_proxy.hpp"         // MasterProxy
#include "minion_commands.hpp"      // FileReadCommand, FileWriteCommand

using namespace std;
namespace ilrd_166_7
{

/**************************** Implementations *********************************/
/****************************** Read Command **********************************/

async_injection_functor FileReadCommand::Run(shared_ptr<IKeyArgs> args)
{
    MinionReadArgs* read_args = static_cast<MinionReadArgs*>(args.get());

    auto masterProxy = Handleton<MasterProxy>::GetInstance();

    char* buffer = new char[read_args->GetSize()];
    auto status = masterProxy->GetFile().Read(read_args->GetOffset(), read_args->GetSize(), buffer);
    
    masterProxy->SendResponse(make_shared<ResponseReadMessage>(read_args->GetUid(), status, read_args->GetSize(), buffer));
    
    return (nullptr);
}

shared_ptr<ICommand> FileReadCommand::CreateFileReadCommand()
{
	return (make_shared<FileReadCommand>());
}

/***************************** Write Command **********************************/

async_injection_functor FileWriteCommand::Run(shared_ptr<IKeyArgs> args)
{
    MinionWriteArgs* write_args = static_cast<MinionWriteArgs*>(args.get());

    auto masterProxy = Handleton<MasterProxy>::GetInstance();
    
    auto status = masterProxy->GetFile().Write(write_args->GetOffset(), write_args->GetSize(), write_args->GetBuffer());

    masterProxy->SendResponse(make_shared<ResponseWriteMessage>(write_args->GetUid(), status));

    return (nullptr);
}

shared_ptr<ICommand> FileWriteCommand::CreateFileWriteCommand()
{
	return (make_shared<FileWriteCommand>());
}
}