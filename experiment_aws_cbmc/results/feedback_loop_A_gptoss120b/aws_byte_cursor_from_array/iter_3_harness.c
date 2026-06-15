#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

size_t nondet_size_t(void) {
    return __CPROVER_nondet_size_t();
}

void aws_byte_cursor_from_array_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    assert(cur.ptr == bytes);
    assert(cur.len == len);
    assert(aws_byte_cursor_is_valid(&cur));
}

int main(void) {
    aws_byte_cursor_from_array_harness();
    return 0;
}
