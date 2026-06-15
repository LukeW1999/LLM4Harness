#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_string str;
    __CPROVER_assume(aws_string_is_valid(&str));
    struct aws_string old_str = str;

    aws_string_destroy(&str);

    if (str.allocator != NULL) {
        assert(str.allocator == old_str.allocator);
    }

    assert(str.len == old_str.len);
    assert(str.bytes == old_str.bytes);

    if (str.allocator == NULL) {
        assert(str.len == 0);
        assert(str.bytes[0] == '\0');
    }
}
