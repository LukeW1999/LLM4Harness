#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 32

void aws_string_destroy_secure_harness(void) {
    /* Use the default allocator which has valid function pointers */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate a nondet aws_string using the default allocator */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(allocator, MAX_STRING_LEN);

    /* Only proceed if str is valid */
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    /* Ensure the allocator stored in the string is the default allocator */
    __CPROVER_assume(str->allocator == allocator);

    /* Call the function under test */
    aws_string_destroy_secure(str);
}
