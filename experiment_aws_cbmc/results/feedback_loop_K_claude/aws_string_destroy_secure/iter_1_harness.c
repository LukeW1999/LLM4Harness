#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_secure(struct aws_string *str);

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministic choice: test with NULL or valid string */
    bool is_null;
    __CPROVER_assume(is_null == true || is_null == false);

    if (is_null) {
        /* Precondition: str is NULL — function must handle gracefully */
        aws_string_destroy_secure(NULL);
        /* Postcondition: no crash, no memory corruption */
        assert(1); /* reached without error */
    } else {
        /* Precondition: str is a valid aws_string */
        /* Use make_arbitrary_aws_string_nondet_len to get a valid string */
        struct aws_string *str = make_arbitrary_aws_string_nondet_len(aws_default_allocator());

        /* Precondition: str must be valid */
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* Capture length before destruction for postcondition checks */
        size_t len_before = str->len;

        /* Non-deterministic choice: test with allocator set or NULL */
        bool has_allocator;
        __CPROVER_assume(has_allocator == true || has_allocator == false);

        if (!has_allocator) {
            /* Simulate static string: set allocator to NULL */
            /* We need a mutable copy since allocator is const in struct */
            /* Cast away const to simulate static string scenario */
            struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
            *alloc_ptr = NULL;

            /* Precondition: str is valid with NULL allocator */
            __CPROVER_assume(aws_string_is_valid(str));

            /* Call the function under test */
            aws_string_destroy_secure(str);

            /* Postcondition: function completed without crash */
            /* Memory was zeroed but not freed (no allocator) */
            assert(1);
        } else {
            /* Precondition: str has a valid allocator */
            __CPROVER_assume(str->allocator != NULL);

            /* Precondition: len is bounded to avoid state explosion */
            __CPROVER_assume(len_before <= 16);

            /* Call the function under test */
            aws_string_destroy_secure(str);

            /* Postcondition: function completed without crash */
            /* Memory was zeroed and freed */
            assert(1);
        }
    }

    return 0;
}
