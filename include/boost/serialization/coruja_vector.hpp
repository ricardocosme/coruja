
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <coruja/container/vector.hpp>

#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/nvp.hpp>

namespace coruja {
    
struct vector_serialization
{
    template<typename Archive,
             typename T,
             typename Allocator,
             template <typename, typename> class Observed,
             typename Derived,
             template <typename> class Signal>
    static void save(Archive& ar,
                     const coruja::vector<
                     T,
                     Allocator,
                     Observed,
                     Derived,
                     Signal>& o,
                     unsigned int /*version*/)
    { boost::serialization::stl::save_collection(ar, o.observed()); }
 
    template<typename Archive, typename T>
    typename boost::enable_if<
        typename boost::serialization::detail::is_default_constructible<
            typename T::value_type>,
        void
        >::type
    static stl_collection_load_impl
    (Archive& ar,
     T& o,
     boost::serialization::collection_size_type count,
     boost::serialization::item_version_type item_version)
    {
        boost::serialization::stl::collection_load_impl
            (ar, o, count, item_version);
        o._after_insert(o.as_derived(), o.begin(), o.end());
    }
    
    template<typename Archive, typename T>
    typename boost::disable_if<
        typename boost::serialization::detail::is_default_constructible<
            typename T::value_type>,
        void
        >::type
    static stl_collection_load_impl
    (Archive& ar,
     T& o,
     boost::serialization::collection_size_type count,
     boost::serialization::item_version_type item_version)
    { boost::serialization::stl::collection_load_impl
            (ar, o, count, item_version); }
    
    template<typename Archive,
             typename T,
             typename Allocator,
             template <typename, typename> class Observed,
             typename Derived,
             template <typename> class Signal>
    static void load(Archive& ar,
                     coruja::vector<
                     T,
                     Allocator,
                     Observed,
                     Derived,
                     Signal>& o,
                     unsigned int /*version*/)
    {
        using namespace boost::serialization;
        
        const boost::archive::library_version_type library_version
            (ar.get_library_version());
        item_version_type item_version(0);
        collection_size_type count;
        ar >> BOOST_SERIALIZATION_NVP(count);
        if(boost::archive::library_version_type(3) < library_version)
            ar >> BOOST_SERIALIZATION_NVP(item_version);
        o.reserve(count);
        stl_collection_load_impl(ar, o, count, item_version);
    }
};
    
} //namespace coruja

namespace boost { namespace serialization {

template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void save(Archive& ar,
                 const coruja::vector<
                 T,
                 Allocator,
                 Observed,
                 Derived,
                 Signal>& o,
                 unsigned int version)
{ coruja::vector_serialization::save(ar, o, version); }
 
template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void load(Archive& ar,
                 coruja::vector<
                 T,
                 Allocator,
                 Observed,
                 Derived,
                 Signal>& o,
                 unsigned int version)
{ coruja::vector_serialization::load(ar, o, version); }
    
template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void serialize (Archive& ar,
                       coruja::vector<
                           T,
                           Allocator,
                           Observed,
                           Derived,
                           Signal>& o,
                       unsigned int version)
{ split_free(ar, o, version); }
    
}//namespace serialization
}//namespace boost
