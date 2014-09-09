#include "hoolib.hpp"

namespace HooLib
{

int rotateIndex(int index, int size)
{
	THROW_ERROR_UNLESS(size > 0);
	if(index > 0)	return index % size;
	return (size - (-index) % size) % size;
}

}

