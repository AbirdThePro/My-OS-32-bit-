#include <stddef.h>
#include "drivers.h"

extern void kmain() {
	char* str = "Hello from the kernel.\n";
	char* str1 = "Hello";
	puts(str);
	puts(str1);
}
