
#include "toy_errno.h"

int toy_errno;

void set_toy_errno(toy_error err)
{
	toy_errno = (int)err;
}
