#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_from_array_harness(void) {
    /* 1. Declare a nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Declare a nondet array pointer */
    uint8_t *array;

    /* If len > 0, array must be non-NULL and valid */
    if (len > 0) {
        array = (uint8_t *)malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        /* len == 0: array can be anything (nondet) */
        /* array is already nondet */
    }

    /* 3. Declare dest buffer (uninitialized) */
    struct aws_byte_buf buf;

    /* 4. Call function under test */
    aws_byte_buf_from_array(&buf, array, len);

    /* 5. Assert postconditions */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(buf.buffer == (uint8_t *)array);
    assert(aws_byte_buf_is_valid(&buf));
}
