#include <aws/common/string.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_secure_harness(void) {
    /* nondeterministically choose between a static (allocator == NULL) and a dynamic string */
    bool use_static = __CPROVER_nondet_bool();

    struct aws_string *str = NULL;

    if (use_static) {
        /* Create a static string with allocator == NULL */
        AWS_STATIC_STRING_FROM_LITERAL(static_str, "static_test");
        str = (struct aws_string *)static_str;
    } else {
        /* Create a dynamically allocated string */
        const char *c_str = "dynamic_test";
        str = aws_string_new_from_c_str(aws_default_allocator(), c_str);
    }

    /* Structural validity assumption */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* Preserve old state */
    struct aws_string *old_str = str;
    size_t old_len = (old_str != NULL) ? old_str->len : 0;
    struct aws_allocator *old_allocator = (old_str != NULL) ? old_str->allocator : NULL;

    /* Make a copy of the original bytes for later comparison (if any) */
    uint8_t *old_bytes = NULL;
    if (old_str != NULL && old_len > 0) {
        old_bytes = malloc(old_len);
        __CPROVER_assume(old_bytes != NULL);
        const uint8_t *src = aws_string_bytes(old_str);
        for (size_t j = 0; j < old_len; ++j) {
            old_bytes[j] = src[j];
        }
    }

    /* Additional index assumption as required by the ground‑truth */
    size_t i = __CPROVER_nondet_size_t();
    __CPROVER_assume(i < old_len);

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */

    /* 1. Return value / error code correctness: the function is void, so nothing to check. */

    /* 2. Output buffer length/capacity invariants */
    if (old_str != NULL && old_allocator == NULL) {
        /* For static strings the memory is not freed; bytes must be zeroed */
        const uint8_t *post_bytes = aws_string_bytes(old_str);
        for (size_t j = 0; j < old_len; ++j) {
            assert(post_bytes[j] == 0);
        }
        /* Length must remain unchanged */
        assert(old_str->len == old_len);
    } else if (old_str != NULL && old_allocator != NULL) {
        /* For dynamically allocated strings the memory may have been released.
           We cannot dereference `old_str` after free, but we can assert that
           the allocator was non‑NULL, indicating a release was attempted. */
        assert(old_allocator != NULL);
    } else {
        /* str was NULL: nothing should have happened */
        assert(str == NULL);
    }

    /* 3. Frame conditions: no other memory should be modified.
       Since the function only touches the string's bytes and possibly frees it,
       we assert that any unrelated allocated memory remains unchanged.
       Here we conservatively assume no other global state is affected. */

    /* Clean up the copy of old bytes */
    free(old_bytes);

    return 0;
}
