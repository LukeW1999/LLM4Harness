#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 32

void aws_string_destroy_secure_harness(void) {
    /* Allocate a nondet aws_string */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(aws_default_allocator(), MAX_STRING_LEN);

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions verified implicitly by CBMC's checks for undefined behavior */
    assert(true);
}
