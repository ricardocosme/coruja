
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/signal.hpp"
#include "coruja/support/type_traits.hpp"
#include "coruja/support/signal/scoped_blocked_connection.hpp"

#include <memory>

namespace coruja {
    
class any_connection
{
public:    
    struct connection_base
    {
        void (*dtor)(void*) noexcept;
        void (*disconnect)(void*);
        bool (*blocked)(const void*) noexcept;
        void (*block)(void*) noexcept;
        void (*unblock)(void*) noexcept;
        void (*copy)(const void*, void*);
        void (*_move)(void*, void*) noexcept;
        bool (*less_than)(const void*, const void*);
        bool (*equal_to)(const void*, const void*);
        const std::type_info& (*type)(const void*) noexcept;
    };
    
    template<typename T, bool small>
    struct model_t;
    
    template<typename T>
    struct model_t<T, true> {
        model_t() = default;
        model_t(T o) : _conn(std::move(o)) {}
        static void dtor(void* self) noexcept
        { static_cast<model_t*>(self)->~model_t(); }
        static void disconnect(void* self)
        { static_cast<model_t*>(self)->_conn.disconnect(); }
        static bool blocked(const void* self) noexcept
        { return static_cast<const model_t*>(self)->_conn.blocked(); }
        static void block(void* self) noexcept
        { static_cast<model_t*>(self)->_conn.block(); }
        static void unblock(void* self) noexcept
        { static_cast<model_t*>(self)->_conn.unblock(); }
        static void copy(const void* self, void* p) 
        { new (p) model_t(static_cast<const model_t*>(self)->_conn); }
        static void _move(void* self, void* p) noexcept
        { new (p) model_t(std::move(static_cast<model_t*>(self)->_conn)); }
        static bool less_than(const void* self, const void* rhs) 
        { return static_cast<const model_t*>(self)->_conn < static_cast<const model_t*>(rhs)->_conn; }
        static bool equal_to(const void* self, const void* rhs) 
        { return static_cast<const model_t*>(self)->_conn == static_cast<const model_t*>(rhs)->_conn; }
        static const std::type_info& type(const void* self) noexcept
        { return typeid(model_t); }
        static constexpr connection_base vtable
        {dtor, disconnect, blocked, block, unblock, copy, _move, less_than,
         equal_to, type};
        T _conn;
    };

    template<typename T>
    struct model_t<T, false> {
        model_t() = default;
        model_t(T o) : _conn(new T{std::move(o)}) {}
        static void dtor(void* self) noexcept
        { static_cast<model_t*>(self)->~model_t(); }
        static void disconnect(void* self)
        { static_cast<model_t*>(self)->_conn->disconnect(); }
        static bool blocked(const void* self) noexcept
        { return static_cast<const model_t*>(self)->_conn->blocked(); }
        static void block(void* self) noexcept
        { static_cast<model_t*>(self)->_conn->block(); }
        static void unblock(void* self) noexcept
        { static_cast<model_t*>(self)->_conn->unblock(); }
        static void copy(const void* self, void* p) 
        { new (p) model_t(*(static_cast<const model_t*>(self)->_conn)); }
        static void _move(void* self, void* p) noexcept
        { new (p) model_t(std::move(*static_cast<model_t*>(self)->_conn)); }
        static bool less_than(const void* self, const void* rhs) 
        { return static_cast<const model_t*>(self)->_conn < static_cast<const model_t*>(rhs)->_conn; }
        static bool equal_to(const void* self, const void* rhs) 
        { return static_cast<const model_t*>(self)->_conn == static_cast<const model_t*>(rhs)->_conn; }
        static const std::type_info& type(const void* self) noexcept
        { return typeid(model_t); }
        static constexpr connection_base vtable
        {dtor, disconnect, blocked, block, unblock, copy, _move, less_than,
         equal_to, type};
        std::unique_ptr<T> _conn;
    };
    
    friend bool operator<(const any_connection::connection_base& lhs,
                          const any_connection::connection_base& rhs);
    friend bool operator<(const any_connection& lhs,
                          const any_connection& rhs);
    friend bool operator==(const any_connection::connection_base& lhs,
                           const any_connection::connection_base& rhs);
    friend bool operator==(const any_connection& lhs,
                           const any_connection& rhs);
    friend bool operator!=(const any_connection::connection_base& lhs,
                           const any_connection::connection_base& rhs);

    static constexpr std::size_t small_size =
        sizeof(connection<signal<void()>::slots_t>);
    
    std::aligned_storage<small_size, sizeof(void*)>::type _storage;

    const connection_base* _base{nullptr};

    template<typename T>
    struct is_small_t {
        static constexpr bool value =
            sizeof(model_t<typename std::decay<T>::type, true>) <= small_size;
    };
    
public:
    
    any_connection()
        : _base(&model_t<connection<signal<void()>::slots_t>, true>::vtable)
    {
        new (&_storage) model_t<connection<signal<void()>::slots_t>, true>();
    }
    
    template<typename T,
             typename Enable = enable_if_t<
                 !std::is_same<
                     any_connection,
                     typename std::decay<T>::type
                 >::value
             >>
    any_connection(T&& o)
        : _base(&model_t<typename std::decay<T>::type, is_small_t<T>::value>::vtable)
    {
        new (&_storage) model_t<typename std::decay<T>::type, is_small_t<T>::value>
            (std::forward<T>(o));
    }
    
    ~any_connection()
    { _base->dtor(&_storage); }
    
    any_connection(const any_connection& rhs)
        : _base(rhs._base)
    { _base->copy(&(rhs._storage), &_storage); }
    
    any_connection& operator=(const any_connection& rhs)
    { return *this = any_connection(rhs); }
    
    any_connection(any_connection&& rhs) noexcept
        : _base(rhs._base)
    { _base->_move(&(rhs._storage), &_storage); }
    
    any_connection& operator=(any_connection&& rhs) noexcept
    {
        _base->dtor(&_storage);
        _base = rhs._base;
        rhs._base->_move(&(rhs._storage), &_storage);
        return *this;
    }
        
    void disconnect()
    {  _base->disconnect(&_storage); }

    bool blocked() const noexcept
    { return _base->blocked(&_storage); }

    void block() noexcept
    { _base->block(&_storage); }

    void unblock() noexcept
    { _base->unblock(&_storage); }

    const std::type_info& type() const noexcept
    { return _base->type(&_storage); }

};

inline bool operator==(const any_connection& lhs,
                       const any_connection& rhs)
{
    return lhs.type() == rhs.type()
        && lhs._base->equal_to(&lhs._storage, &rhs._storage);
}

inline bool operator!=(const any_connection& lhs,
                       const any_connection& rhs)
{ return !(lhs == rhs); }
    
inline bool operator<(const any_connection& lhs,
                      const any_connection& rhs)
{
    return lhs.type() == rhs.type()
        ? lhs._base->less_than(&lhs._storage, &rhs._storage)
        : std::type_index(lhs.type()) < std::type_index(rhs.type());
}    

struct scoped_blocked_any_connection : scoped_blocked_connection<any_connection>
{
    scoped_blocked_any_connection() = default;
    template<typename Connection>
    scoped_blocked_any_connection(Connection conn)
        : scoped_blocked_connection<any_connection>(std::move(conn))
    {}
};

struct scoped_any_connection : scoped_connection<any_connection>
{
    scoped_any_connection() = default;
    template<typename Connection>
    scoped_any_connection(Connection conn)
        : scoped_connection<any_connection>(std::move(conn))
    {}
};

}

namespace coruja {
template<typename T>
constexpr any_connection::connection_base any_connection::model_t<T, true>::vtable;
template<typename T>
constexpr any_connection::connection_base any_connection::model_t<T, false>::vtable;

}
