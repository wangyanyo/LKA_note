#include "kernel.h"

extern int main(int argc, char** argv);

void c_start()
{
	struct process_argument argument;
	kernel_process_get_argument(&argument);

	int res = main(argument.argc, argument.argv);
	if (res == 0) {

	}
}