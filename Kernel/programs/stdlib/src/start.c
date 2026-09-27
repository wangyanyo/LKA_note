#include "kernel.h"

extern int main(int argc, char** argv);
extern void kernel_exit(int res);

void c_start()
{
	struct process_argument argument;
	kernel_process_get_argument(&argument);

	int res = main(argument.argc, argument.argv);
	kernel_exit(res);
}