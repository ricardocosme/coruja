#pragma once

#include <boost/serialization/coruja_boost_variant.hpp>

namespace boost { namespace serialization { namespace detail {

//Communicates to the archive that the subobject's address has been
//moved to a new address. This a no-op for any T without
//specialization.
template<typename Archive, typename T, typename U>
inline void reset_subobject_address(Archive&,
                                    T& /*to*/,
                                    U& /*from*/)
{ /*empty*/ }

//Specialization for variant            
template<typename Archive, BOOST_VARIANT_ENUM_PARAMS(/* typename */ class T)>
inline void reset_subobject_address(
    Archive& ar,
    coruja::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& to,
    coruja::variant<BOOST_VARIANT_ENUM_PARAMS(T)>& from)
{
    assert(to.which() == from.which());
    using model =
        typename coruja::variant<BOOST_VARIANT_ENUM_PARAMS(T)>::observed_t;
    action_extracted_value<model>::invoke
        (from.which(), 
         reset_variant_content_address<Archive, model>
         {ar, to.observed(), from.observed()});
}
            
}}}
