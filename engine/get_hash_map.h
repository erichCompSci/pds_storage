#ifndef _GET_HASH_MAP_H_
#define _GET_HASH_MAP_H_

/*
 *
 *  $Id: get_hash_map.h,v 1.5 2005-08-10 22:07:57 pmw Exp $
 *
 */

/*
 *  hash_map is a non-standard extension to the Standard C++ library.
 *  Because of this, GCC has managed to completely hack up the way
 *  they provide the type between versions 2 and 3 of the compiler, and
 *  now even between v3.0 and v3.1.  So with this file I'm trying to localize
 *  changes based on that pain.
 *
 *  Any module in the engine that needs the hash_map header should simply
 *  include this file to have the dirty work done for them.
 */

#ifndef PACKAGE
/* must not have config.h */
#include "../config.h"
#endif

/* 
 * This check has to happen before __GNUG__ because at least
 * some versions of clang apparently also define __GNUG__
 */
#ifdef __clang__
#include <unordered_map>
using namespace std;
#define hash_map unordered_map

#elif __GNUG__
#define GCC_VERSION (__GNUC__ * 10000 \
    + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

  #if (__GNUC__ < 3)
    #ifdef HAVE_HASH_MAP
      #include <hash_map>
    #elif HAVE_HASH_MAP_H
      #include <hash_map.h>
    #endif
  #else
    #if GCC_VERSION > 46300
      #include <unordered_map>
      using namespace std;
      #define hash_map unordered_map
    #else
      #include <ext/hash_map>
    #endif
  #endif

/*
 *  GCC changes the namespace of hash_map from std to __gnu_cxx
 *  between v3.0 and v3.1.  Bastards.
 */
  #if (__GNUC__ == 3 && __GNUC_MINOR__ == 0)
    using namespace std;
  #else
    using namespace __gnu_cxx;
  #endif

#else 
#include <hash_map>
#endif

#endif // _GET_HASH_MAP_H_
/*
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2003/12/04 18:28:54  chaos
 *  Include ../config.h so that this actually works....
 *
 *  Revision 1.3  2003/12/03 18:37:33  eisen
 *  look for both hash_map and hash_map.h
 *
 *  Revision 1.2  2003/01/24 19:31:28  eisen
 *  Some tweaks to make pds compile with Forte CC (version 7 with the
 *  -library=stlport4 argument).
 *
 *  Revision 1.1  2002/08/13 20:26:47  pmw
 *  Fixed craziness about including the hash_map header.  it's different
 *  between GCC 2.x and 3.0 and again between 3.0 and 3.1.  Introduced
 *  get_hash_map.h header to try and isolate the pain...
 *
 *
 */
