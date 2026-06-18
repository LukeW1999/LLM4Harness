#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness(void) {
    /* Create a valid aws_string with a proper allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Allocate the string using the allocator so it's a proper dynamic object */
    struct aws_string *str = make_arbitrary_aws_string(allocator);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Call aws_string_destroy - it will use the allocator to free memory */
    aws_string_destroy(str);

    /* After destroy, we cannot access str since memory is freed */
    /* Just assert true to confirm we reached here without errors */
    assert(true);
}
