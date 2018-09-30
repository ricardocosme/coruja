#pragma once

#include <coruja/boost_variant.hpp>

#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/empty.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost { namespace serialization {
        
template<class Archive>
struct variant_save
{
    using result_type = void;
    
    variant_save(Archive& ar)
        : _ar(ar)
    {}
    
    template<class T>
    result_type operator()(const T& value) const
    { _ar << BOOST_SERIALIZATION_NVP(value); }
private:
    Archive& _ar;
};
        
template<class S>
struct action_extracted_value_impl
{
    struct action_null {
        template<typename F>
        static void invoke(int /*which*/, F)
        {}
    };

    struct action_impl
    {
        template<typename F>
        static void invoke(int which, F f)
        {
            if(which == 0)
            {
                using head_type = typename boost::mpl::front<S>::type;
                head_type value;
                f(value);
                return;
            }
            using type = typename boost::mpl::pop_front<S>::type;
            action_extracted_value_impl<type>::action
                (which - 1, std::move(f));
        }
    };

    template<typename F>
    static void action(int which, F f)
    {
        typedef typename boost::mpl::eval_if<boost::mpl::empty<S>,
            boost::mpl::identity<action_null>,
            boost::mpl::identity<action_impl>
        >::type typex;
        typex::invoke(which, std::move(f));
    }
};

template<typename Variant>        
struct action_extracted_value
{
    template<typename F>
    static void invoke(int which, F f)
    {
        action_extracted_value_impl<typename Variant::types>::action
            (which, std::move(f));
    }
};
        
template<typename Archive, typename Variant>        
struct assign_to_variant
{
    Archive& ar;
    Variant& variant;
    template<typename T>
    void operator()(T& value)
    {
        ar >> BOOST_SERIALIZATION_NVP(value);
        variant = std::move(value);
        ar.reset_object_address(&boost::get<T>(variant.observed()), &value);
    }
};
        
template<typename Archive, typename Variant>        
struct reset_variant_content_address
{
    Archive& ar;
    const Variant& to;
    const Variant& from;
    template<typename T>
    void operator()(T&)
    {
        ar.reset_object_address(&boost::get<T>(to), &boost::get<T>(from));
    }
};
        
}}

namespace coruja { namespace serialization {
        
struct variant
{        
    template<typename Archive, typename...T>
    static void save(Archive& ar,
                     const coruja::variant<T...>& o,
                     unsigned int /*version*/)
    {
        auto& v = o.observed(); 
        auto which = v.which();
        ar << BOOST_SERIALIZATION_NVP(which);
        boost::serialization::variant_save<Archive> visitor(ar);
        v.apply_visitor(visitor);
    }
 
    template<typename Archive, typename...T>
    static void load(Archive& ar,
                     coruja::variant<T...>& o,
                     unsigned int version)
    {
        int which;
        ar >> BOOST_SERIALIZATION_NVP(which);
        using observed_t = typename coruja::variant<T...>::observed_t; 
        boost::serialization::action_extracted_value<observed_t>::invoke
            (which,
             boost::serialization::assign_to_variant<Archive, coruja::variant<T...>>{ar, o});
    }
};
        
}}

namespace boost { namespace serialization {
                        
template<typename Archive, typename...T>
inline void save(Archive& ar,
                 const coruja::variant<T...>& o,
                 unsigned int version)
{
    ::coruja::serialization::variant::save(ar, o, version);
}
 
template<typename Archive, typename...T>
inline void load(Archive& ar,
                 coruja::variant<T...>& o,
                 unsigned int version)
{
    ::coruja::serialization::variant::load(ar, o, version);
}
 
template<typename Archive, typename...T>
inline void serialize(Archive& ar, coruja::variant<T...>& o,
                      unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
