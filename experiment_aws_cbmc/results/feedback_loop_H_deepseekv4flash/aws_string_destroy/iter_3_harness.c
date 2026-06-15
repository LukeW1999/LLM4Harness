#include <stdlib.h>
#include <aws/common/string.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    int has_allocator = 0;

    // Nondeterministically choose one of three cases
    int choice;
    __CPROVER_assume(choice >= 0 && choice <= 2);

    if (choice == 0) {
        // str is NULL
        str = NULL;
    } else if (choice == 1) {
        // str is non-NULL with allocator
        size_t len;
        __CPROVER_assume(len <= 64);
        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        str->len = len;
        str->allocator = malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(str->allocator != NULL);
        has_allocator = 1;
    } else {
        // str is non-NULL without allocator
        size_t len;
        __CPROVER_assume(len <= 64);
        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        str->len = len;
        str->allocator = NULL;
        has_allocator = 0;
    }

    aws_string_destroy(str);

    // Postcondition assertions
    if (str == NULL) {
        // Nothing to assert, but ensure reachable
        __CPROVER_assert(1, "NULL case reachable");
    } else {
        if (has_allocator) {
            // Memory should be freed (not readable)
            __CPROVER_assert(!__CPROVER_r_ok(str, sizeof(struct aws_string)),
                             "String with allocator is freed");
        } else {
            // Memory should still be readable
            __CPROVER_assert(__CPROVER_r_ok(str, sizeof(struct aws_string)),
                             "String without allocator remains readable");
        }
    }
}
