/* CBMC harness for aws_byte_buf_from_empty_array() */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t *array;
    size_t capacity;

    /* Choose whether to provide a non‑NULL array */
    bool provide_array = nondet_bool();
    __CPROVER_assume(provide_array == true || provide_array == false);

    if (provide_array) {
        capacity = nondet_uint64();
        __CPROVER_assume(capacity > 0);
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        array = malloc(capacity);
        __CPROVER_assume(array != NULL);
    } else {
        array = NULL;
        capacity = 0;
    }

    /* Call the function under test */
    aws_byte_buf_from_empty_array(&buf, array, capacity);

    /* Post‑condition checks */
    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
