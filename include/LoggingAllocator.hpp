#pragma once
#include "ThreadSafeLogger.hpp"
#include <cstddef>
#include <memory>
#include <typeinfo>

//noexcept gurantees that no exceptions will be thrown by that method
//gurantee can be broken, it which case the program will just end immidiately without handling the exception
// = default means that compiler will generate the implementation of the constructor, ie. sets members with their default initializers or leaves unit primitives
//this simple stateless allocator does not throw exceptions
//for constr because then std containers can just swap pointers instead of creating full copies during moving
//for dealloc because std containers destructors are noexcept as cleanup ops are simply not allowed to fail ...
//std::terminate is called on dual exceptions or on exception thrown in noexcept function, which terminates the program at once to prevent propagation of assumed corrupted internal state (like writing corrupted data on disk)
template <typename T>
class LoggingAllocator {
public:
    using value_type = T;

    LoggingAllocator() noexcept = default;

    template <typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    T* allocate(size_t n) {
        ThreadSafeLogger::log_alloc(typeid(T).name(), n, n * sizeof(T));
        //creates instace of std::allocator on stack
        //allocates n elems of type T on heap via the allocate(n) method
        //returns a pointer to that mem
        return std::allocator<T>{}.allocate(n); 
    }

    void deallocate(T* p, std::size_t n) noexcept {
        //creates an instance of std::allocator on stack
        //dealloc n elems of type T from heap starting at p
        std::allocator<T>{}.deallocate(p, n);
    }

    //hidden friend, typename T is already known
    //also the compiler does not have to consider this on every == operation
    //lack of named params, since they are not used inside the function
    //params are there only to satisfy the compiler which reqs a specific sig
    template <typename U>
    friend bool operator==(const LoggingAllocator&, const LoggingAllocator<U>&) { return true; }
    template < typename U>
    friend bool operator!=(const LoggingAllocator&, const LoggingAllocator<U>&) { return false; }
};
