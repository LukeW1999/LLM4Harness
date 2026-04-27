#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_from_array_harness(void) {
    /* 1. Declare a nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate array - use malloc so CBMC tracks validity */
    uint8_t *array = NULL;
    if (len > 0) {
        array = (uint8_t *)malloc(len);
        __CPROVER_assume(array != NULL);
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
}
