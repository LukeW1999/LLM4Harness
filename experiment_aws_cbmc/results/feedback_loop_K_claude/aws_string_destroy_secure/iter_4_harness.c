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
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t len;
    __CPROVER_assume(len <= 4);

    struct aws_string *str = aws_string_new_from_array(allocator, (const uint8_t *)"test", len);

    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    aws_string_destroy_secure(str);

    assert(1);
}
