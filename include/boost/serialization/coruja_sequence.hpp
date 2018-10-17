
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/nvp.hpp>

namespace coruja {
    
template<typename Sequence>
void pre_load(Sequence& o, typename Sequence::size_type) {}
    
struct sequence_serialization
{
    template<typename Archive, typename Sequence>
    static void save(Archive& ar,
                     const Sequence& o,
                     const unsigned int /*version*/)
    {
        boost::serialization::stl::save_collection<
            Archive, 
            typename Sequence::observed_t
        >(ar, o.observed());
    }
     
    template<typename Archive, typename T>
    static void stl_collection_load_impl
    (Archive& ar,
     T& o,
     boost::serialization::collection_size_type count,
     boost::serialization::item_version_type item_version)
    {
        o.clear();
        boost::serialization::stl::collection_load_impl
            (ar, o._container, count, item_version);
        o._after_insert(o.as_derived(), o.begin(), o.end());
    }
    
    template<typename Archive, typename Sequence>
    static void load(Archive& ar, Sequence& o,
                     const unsigned int version)
    {
        const boost::archive::library_version_type library_version(
            ar.get_library_version()
        );
        // retrieve number of elements
        boost::serialization::item_version_type item_version(0);
        boost::serialization::collection_size_type count;
        ar >> BOOST_SERIALIZATION_NVP(count);
        if(boost::archive::library_version_type(3) < library_version){
            ar >> BOOST_SERIALIZATION_NVP(item_version);
        }
        pre_load(o, count);
        stl_collection_load_impl(ar, o, count, item_version);
    }
};

}
