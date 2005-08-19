#ifndef __PyCXX_config_hh__
#define __PyCXX_config_hh__


//
// Microsoft VC++ has no traits
//
#if defined( _MSC_VER )

#define STANDARD_LIBRARY_HAS_ITERATOR_TRAITS 0



//
//	Assume all other compilers do
//
#else

// Macros to deal with deficiencies in compilers
#define STANDARD_LIBRARY_HAS_ITERATOR_TRAITS 1
#endif

#if STANDARD_LIBRARY_HAS_ITERATOR_TRAITS
#define random_access_iterator_parent(itemtype) std::random_access_iterator<itemtype, int>
#else
#define random_access_iterator_parent(itemtype) std::iterator<std::random_access_iterator_tag,itemtype,int>
#endif

#endif //  __PyCXX_config_hh__
