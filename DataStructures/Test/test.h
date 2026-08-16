#include <stdio.h>

#define CHECK(expr)                                                                              \
	do {                                                                                     \
		if (!(expr)) {                                                                   \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #expr); \
			return false;                                                            \
		}                                                                                \
	} while (0)

#define RUN_TEST(fn)                                                                             \
	do {                                                                                     \
		++tests_run;                                                                     \
		if (!(fn())) {                                                                   \
			++tests_failed;                                                          \
			fprintf(stderr, "FAILED: %s\n", #fn);                                    \
		} else {                                                                         \
			fprintf(stdout, "PASSED: %s\n", #fn);                                    \
		}                                                                                \
	} while (0)

