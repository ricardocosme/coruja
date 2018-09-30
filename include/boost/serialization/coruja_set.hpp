#pragma once

#include <boost/serialization/coruja_boost_variant.hpp>

#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/detail/reset_subobject_address.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <coruja/container/set.hpp>

namespace boost { namespace serialization {
        
template<typename Archive,
         typename Key,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename> class Model,
         typename Derived>
inline void save(Archive& ar,
                 const coruja::set<Key, Compare, Allocator, Model, Derived>& o,
                 const unsigned int version)
{
    boost::serialization::stl::save_collection<
        Archive, 
        typename coruja::set<Key, Compare, Allocator, Model, Derived>::observed_t
        >(ar, o.observed());
}
 
template<typename Archive,
         typename Key,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename> class Model,
         typename Derived>
inline void load(Archive& ar,
                 coruja::set<
                     Key, Compare, Allocator, Model, Derived>& o,
                 const unsigned int version)
{
    auto library_version(ar.get_library_version());
        
    using namespace boost::serialization;
        
    // retrieve number of elements
    item_version_type item_version(0);
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);

    if(boost::archive::library_version_type(3) < library_version)
        ar >> BOOST_SERIALIZATION_NVP(item_version);

    using Container = typename coruja::set<
        Key, Compare, Allocator, Model, Derived>::observed_t;
        
    auto hint = o.begin();
    while(count-- > 0)
    {
        using type = typename Container::value_type;
        boost::serialization::detail::stack_construct<Archive, type> t
            (ar, item_version);
        ar >> make_nvp("item", t.reference());
            
        hint = o.insert(hint, std::move(t.reference()));
        ar.reset_object_address(&(*hint), &t.reference());
        detail::reset_subobject_address(ar, *hint, t.reference());
    }
}
 
template<typename Archive,
         typename Key,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename> class Model,
         typename Derived>
inline void serialize (Archive& ar,
                       coruja::set<
                           Key, Compare, Allocator, Model, Derived>& o,
                       const unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
