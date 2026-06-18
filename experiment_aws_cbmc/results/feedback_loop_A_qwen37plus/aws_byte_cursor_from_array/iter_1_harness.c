#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    void *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);
    assert(aws_byte_cursor_is_valid(&cur));
}
