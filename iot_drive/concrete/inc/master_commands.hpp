#ifndef __ILRD_MASTER_COMMANDS_HPP__
#define __ILRD_MASTER_COMMANDS_HPP__

#include "framework.hpp"    // ICommand

/**
 * @class  Master Commands
 * @brief  Master Commands for the TPTask
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class ReadCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override;
    static std::shared_ptr<ICommand> CreateReadCommand();
}; // ReadCommand

class WriteCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override;
    static std::shared_ptr<ICommand> CreateWriteCommand();
}; // WriteCommand
} // namespace ilrd_166_7

#endif // __ILRD_MASTER_COMMANDS_HPP__