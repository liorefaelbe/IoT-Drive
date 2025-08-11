#ifndef __ILRD_MINION_COMMANDS_HPP__
#define __ILRD_MINION_COMMANDS_HPP__

#include "framework.hpp"    // IKeyArgs

/**
 * @class  MinionCommands
 * @brief  Read and Write command to the minion file
 * 
 * @author Lior Reafael Berkovits
 * @date   26/05/2025
 */

 /************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
class FileReadCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override;
    static std::shared_ptr<ICommand> CreateFileReadCommand();
}; // FileReadCommand

class FileWriteCommand : public ICommand
{
public:
    async_injection_functor Run(std::shared_ptr<IKeyArgs> args) override;
    static std::shared_ptr<ICommand> CreateFileWriteCommand();
}; // FileWriteCommand
} // namespace ilrd_166_7

#endif // __ILRD_MINION_COMMANDS_HPP__