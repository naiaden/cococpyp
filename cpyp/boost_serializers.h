#ifndef MAPTYPE_SERIALIZATION_HPP
#define MAPTYPE_SERIALIZATION_HPP

// this is an optional header if you want to load and
// save CRPs / MFCRPs with the boost serialization framework

#include <unordered_map>
#include "cpyp/msparse_vector.h"

#include <boost/config.hpp>

#include <new>
#include <memory>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/collections_load_imp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <patternmodel.h>
#include <pattern.h>

namespace boost { 
namespace serialization {



//template<class Archive>
//inline void serialize(
//    Archive & ar,
//    const Pattern & p,
//    const unsigned int file_version
//){
//    split_free(ar, p, file_version); 
//}
//
//template<class Archive>
//inline void save(Archive & ar, const Pattern & p, const unsigned int file_version)
//{
//    //ar << p.size();
//    //ar << &(p.data);
//    std::string blabla = p.toByteString();
//    ar & blabla; //p.toByteString();
//
//    std::cerr << "c~" << std::endl;
//}
//
//template<class Archive>
//inline void load(Archive & ar, const Pattern & p, const unsigned int file_version)
//{
//   // int size;
//   // ar >> size;
//   // 
//   // unsigned char* dataptr;
//   // ar >> dataptr;
//
//   // //p.set(dataptr, size);
//   // ::new(p)Pattern(4);
//   // //::new(p)Pattern(dataptr, size);
//
//    std::string byteString;
//    ar >> byteString;
//    p.set(byteString);
//}

/*
template<class Archive>
inline void save_construct_data(Archive & ar, const Pattern * p, const unsigned int file_version)
{
    std::cerr << "Saving the crap out of this pattern" << std::endl;
    ar << p->toByteString();
}

template<class Archive>
inline void load_construct_data(Archive & ar, Pattern * p, const unsigned int file_version)
{
    std::cerr << "Are getting here?" << std::endl;

    std::string byteString;
    ar >> byteString;

    p->set(byteString);

    //::new(p)Pattern(byteString);
}
*/


/*
template<class Archive>
inline void serialize(
    Archive & ar,
    Pattern & p,
    const unsigned int file_version
){
    split_free(ar, p, file_version); 
}

template<class Archive>
inline void save(Archive & ar, Pattern & p, const unsigned int file_version)
{
    //ar << p.size();
    //ar << p.data;
    std::string blabla = p.toByteString();
    ar << blabla;

    std::cerr << " ~" << std::endl;
}
*/

/*
template<class Archive>
inline void load(Archive & ar, Pattern & p, const unsigned int file_version)
{
//    int size;
//    ar >> size;
//    
//    unsigned char dataptr;
//    ar >> dataptr;

//    p.set(&dataptr, size);
//
    //::new(p)Pattern(dataptr, size);
    std::string byteString;

//    ar >> byteString;
//   p.set(byteString);
}
*/

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    boost::serialization::stl::save_collection<
        Archive, 
        std::unordered_map<Key, Type, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    boost::serialization::stl::load_collection<
        Archive,
        std::unordered_map<Key, Type, Compare, Allocator>,
        boost::serialization::stl::archive_input_map<
            Archive, std::unordered_map<Key, Type, Compare, Allocator> >,
            boost::serialization::stl::no_reserve_imp<std::unordered_map<
                Key, Type, Compare, Allocator
            >
        >
    >(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::unordered_map<Key, Type, Compare, Allocator> &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

template<class Archive, class Type>
inline void save(
    Archive & ar,
    const cpyp::SparseVector<Type> &t,
    const unsigned int file_version
){
  t.save(ar, file_version);
}

template<class Archive, class Type>
inline void load(
    Archive & ar,
    cpyp::SparseVector<Type> &t,
    const unsigned int file_version
){
  t.load(ar, file_version);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Type>
inline void serialize(
    Archive & ar,
    cpyp::SparseVector<Type> &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}
} // serialization
} // namespace boost

#endif // BOOST_SERIALIZATION_MAP_HPP
