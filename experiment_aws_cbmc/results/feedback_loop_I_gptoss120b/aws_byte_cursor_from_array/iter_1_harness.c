#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length, bounded to keep state space reasonable */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate a buffer of the chosen length (may be NULL when len == 0) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* postconditions */
    assert(cur.ptr == bytes);
    assert(cur.len == len);

    /* validity invariant must hold for the returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));

    /* clean up */
    free(bytes);
}
