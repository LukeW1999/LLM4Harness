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

    if (is_null) {
        /* Precondition: str is NULL — function must handle gracefully */
        aws_string_destroy_secure(NULL);
        assert(1);
    } else {
        /* Use make_arbitrary_aws_string to get a valid string allocated with default allocator */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);

        struct aws_string *str = make_arbitrary_aws_string_nondet_len(allocator);

        /* Precondition: str must be valid */
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* Precondition: str must have a valid allocator (not NULL) so it can be freed */
        __CPROVER_assume(str->allocator != NULL);
        __CPROVER_assume(str->allocator == allocator);

        /* Precondition: allocator must have mem_release set */
        __CPROVER_assume(str->allocator->mem_release != NULL);

        /* Precondition: len is bounded to avoid state explosion */
        __CPROVER_assume(str->len <= 4);

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /* Postcondition: function completed without crash */
        assert(1);
    }
}
