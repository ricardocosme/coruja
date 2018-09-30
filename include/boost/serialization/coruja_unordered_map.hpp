#pragma once

#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/unordered_collections_save_imp.hpp>
#include <boost/serialization/utility.hpp>
#include <coruja/container/unordered_map.hpp>

namespace boost { namespace serialization {
        
template<typename Archive,
         typename Key,
         typename T,
         typename Hash,
         typename KeyEqual,
         typename Allocator>
inline void save(Archive& ar,
                 const coruja::unordered_map<Key, T, Hash, KeyEqual, Allocator>& o,
                 const unsigned int version)
{
    boost::serialization::stl::save_unordered_collection<
        Archive, 
        typename coruja::unordered_map<Key, T, Hash, KeyEqual, Allocator>::observed_t
        >(ar, o.observed());
}
 
template<typename Archive,
         typename Key,
         typename T,
         typename Hash,
         typename KeyEqual,
         typename Allocator>
inline void load(Archive& ar,
                 coruja::unordered_map<
                     Key, T, Hash, KeyEqual, Allocator>& o,
                 const unsigned int version)
{
    using namespace boost::serialization;
        
    collection_size_type count;
    collection_size_type bucket_count;
    item_version_type item_version(0);
    auto library_version(ar.get_library_version());
        
    // retrieve number of elements
    ar >> BOOST_SERIALIZATION_NVP(count);
    ar >> BOOST_SERIALIZATION_NVP(bucket_count);
    if(boost::archive::library_version_type(3) < library_version)
        ar >> BOOST_SERIALIZATION_NVP(item_version);

    o.clear();
    o.rehash(bucket_count);
    while(count-- > 0)
    {
        using value_type = typename coruja::unordered_map<
            Key, T, Hash, KeyEqual, Allocator>::observed_t::value_type;
        boost::serialization::detail::stack_construct<Archive, value_type>
            t(ar, item_version);
        ar >> make_nvp("item", t.reference());            
        auto result = o.insert(std::move(t.reference()));
        if(result.second)
            ar.reset_object_address(&(result.first->second),
                                    &t.reference().second);
    }
}
 
template<typename Archive,
         typename Key,
         typename T,
         typename Hash,
         typename KeyEqual,
         typename Allocator>
inline void serialize (Archive& ar,
                       coruja::unordered_map<
                           Key, T, Hash, KeyEqual, Allocator>& o,
                       const unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
