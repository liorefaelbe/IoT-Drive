#ifndef __ILRD_PQ_ADAPRER_HPP__
#define __ILRD_PQ_ADAPRER_HPP__

#include <queue> // std::priority_queue

/**
 * @class  PQAdapter
 * @brief  Adapter class for std::priority_queue with queue-like interface
 *         This adapter converts the std::priority_queue interface to match a standard
 *         queue interface by mapping top() to front(). This allows code written for
 *         standard queue interfaces to work with priority queues without modification.
 * 
 * @tparam The adapter preserves all template parameters of std::priority_queue:
 *          - T: Value type to be stored
 *          - Compare: Comparison function object type (defaults to std::less)
 *          - Container: Container type (defaults to std::vector)
 * 
 * @author Lior Reafael Berkovits
 * @date   24/04/2025
 */

namespace ilrd_166_7
{
template <typename T, typename Compare = std::less<T>, typename Container = std::vector<T>>
class PQAdapter
{
public:
    void pop() { pq.pop(); }
    
    void push(const T& value) { pq.push(value); }

    bool empty() const { return (pq.empty()); }
    
    const T& front() const { return (pq.top()); }

    typedef typename std::priority_queue<T, Container, Compare>::value_type value_type;
private:
    std::priority_queue<T, Container, Compare> pq;
}; // PQAdapter
} // namespace ilrd_166_7

#endif // __ILRD_PQ_ADAPRER_HPP__