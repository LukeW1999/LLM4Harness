#include <aws/common/string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Helper to model a valid aws_string */
struct aws_string *nondet_aws_string_with_max_len(size_t max_len) {
    size_t total_size = sizeof(struct aws_string) + max_len + 1; /* extra byte for null terminator */
    struct aws_string *str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Non-deterministic allocator: either NULL (static) or a valid allocator */
    if (nondet_bool()) {
        str->allocator = NULL;
    } else {
        str->allocator = malloc(sizeof(struct aws_allocator));
        __CPROVER_assume(str->allocator != NULL);
    }

    /* Non-deterministic length, bounded by max_len */
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= max_len);

    /* The bytes array is part of the allocation; we do not need to initialize it */
    return str;
}

void aws_string_destroy_harness() {
    /* Maximum length to bound the state space */
    const size_t MAX_STRING_LEN = 64;

    /* Non-deterministically create a string */
    struct aws_string *str = nondet_aws_string_with_max_len(MAX_STRING_LEN);

    /* Record whether allocator is non-null before call */
    bool had_allocator = (str != NULL && str->allocator != NULL);

    /* Call the function */
    aws_string_destroy(str);

    /* Postcondition: if allocator was non-null, the memory is freed */
    if (had_allocator) {
        /* After freeing, the memory is no longer readable */
        assert(!AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
    } else {
        /* If str was NULL, nothing happens */
        /* If str had a NULL allocator, the memory remains allocated (static string) */
        /* In that case, we can still read it */
        if (str != NULL) {
            assert(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
        }
    }

    /* Ensure no further accesses to the freed pointer are possible - this is ensured by the above assertion */
}
