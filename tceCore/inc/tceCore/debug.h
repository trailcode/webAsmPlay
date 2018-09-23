#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

#undef NDEBUG
#define assure(a) if ((a) == -1) { perror (#a); exit (1); }

#define EBUG
#ifdef EBUG
#       define dmess(a) std::cout << "in " << __FILE__ << \
        " at " << __LINE__ << " " << a << std::endl;

#       define dprint(a) std::cout << "in " << __FILE__ << \
        " at " << __LINE__ << " " << (#a) << " = " << a << std::endl;

#       define dprintt(a,b) std::cout << "in " << __FILE__ << \
        " at " << __LINE__ << " " << a << " " << (#b) << " = " \
        << b << std::endl
#else
#define dprint(a)
#endif /* EBUG */

#endif /*DEBUG_H_*/
