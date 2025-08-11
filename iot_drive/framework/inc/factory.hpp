#ifndef __ILRD_FACTORY_HPP__
#define __ILRD_FACTORY_HPP__

#include <memory>           // std::shared_ptr
#include <unordered_map>    // std::unordered_map
#include <functional>       // std::function
#include <stdexcept>        // std::runtime_error

/**
* @class  Factory
* @brief  Factory<BASE, KEY, ARGS...> - Creates objects derived from BASE class
*         identified by a KEY, and constructed with ARGS.
*
* Features:
*   - Type-safe creation of derived class instances
*   - Runtime selection of object types via keys
*   - Dynamic registration of object creation functions
*   - Automatic memory management with shared pointers
*
* @tparam  BASE: The type of obj base class to create in the factory
* @tparam  KEY: The name of obj to create in the factory
* @tparam  ARGS: Args for the obj Ctor
*
* 
* @author Lior Reafael Berkovits
* @date   28/04/2025
*/

/************************* Forward Declaration *******************************/

namespace ilrd_166_7
{
template <typename BASE, typename KEY, typename... ARGS>
class Factory
{
public:
    explicit Factory() = default;
    ~Factory() = default;

    Factory(const Factory& other) = delete;
    Factory& operator=(const Factory& other) = delete;

    std::shared_ptr<BASE> Create(KEY objectType, ARGS&&... args) const;

    void Add(KEY objectType, std::function<std::shared_ptr<BASE>(ARGS... args)> objectCreationFunction);
    
private:
    std::unordered_map<KEY, std::function<std::shared_ptr<BASE>(ARGS... args)>> m_objectsCreationMap;
}; // Factory

/**************************** Implementations *********************************/

template <typename BASE, typename KEY, typename... ARGS>
std::shared_ptr<BASE> Factory<BASE, KEY, ARGS...>::Create(KEY objectType, ARGS&&... args) const
{
	return (m_objectsCreationMap.at(objectType)(args...));
}

template <typename BASE, typename KEY, typename... ARGS>
void Factory<BASE, KEY, ARGS...>::Add(KEY objectType, std::function<std::shared_ptr<BASE>(ARGS... args)> objectCreationFunction)
{
	m_objectsCreationMap[objectType] = objectCreationFunction;
}       
} // namespace ilrd_166_7

#endif // __ILRD_FACTORY_HPP__