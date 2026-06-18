#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    void *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        if (nondet_bool()) {
            bytes = malloc(1);
        }
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);
    assert(aws_byte_cursor_is_valid(&result));
}
