#ifndef ___HOOLIB_HPP___
#define ___HOOLIB_HPP___

#include <sstream>
#include <stdexcept>

namespace HooLib
{

using int64 = long long int;
using uint64 = unsigned long long int;
int rotateIndex(int index, int size);

#define THROW_ERROR_MSG(msg) { \
	std::stringstream ss; \
	ss << "ERROR!\t" << std::endl \
	<< "__FILE__:\t" << __FILE__ << std::endl \
	<< "__LINE__:\t" << __LINE__ << std::endl \
	<< "Message:\t" << (msg) << std::endl; \
	throw std::runtime_error(ss.str()); \
}

#define THROW_ERROR_IF(expr) \
	if((expr)) { THROW_ERROR_MSG(#expr); }
#define THROW_ERROR_MSG_IF(expr, msg) \
	if((expr)) { THROW_ERROR_MSG(msg); }

#define THROW_ERROR_UNLESS(expr) \
	if(!(expr)) { THROW_ERROR_MSG(#expr); }
#define THROW_ERROR_MSG_UNLESS(expr, msg) \
	if(!(expr)) { THROW_ERROR_MSG(msg); }

}

#endif


