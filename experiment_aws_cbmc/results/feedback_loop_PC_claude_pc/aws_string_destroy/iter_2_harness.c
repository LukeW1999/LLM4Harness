#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    bool is_null = nondet_bool();

    if (is_null) {
        /* Case 1: str is NULL — should be a no-op */
        aws_string_destroy(NULL);
    } else {
        /* Case 2 & 3: str is non-NULL, use make_arbitrary_aws_string to get a valid string */
        struct aws_string *str = nondet_bool() ? make_arbitrary_aws_string() : make_arbitrary_aws_string_nondet_allocator();

        /* Ensure the string is valid per aws_string_is_valid */
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        aws_string_destroy(str);
    }
}
