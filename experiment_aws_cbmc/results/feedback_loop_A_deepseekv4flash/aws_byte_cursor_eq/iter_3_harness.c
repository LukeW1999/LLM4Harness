#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Nondeterministic lengths, bounded to keep verification feasible */
    size_t len_a;
    size_t len_b;
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* Allocate buffers */
    uint8_t *buf_a = malloc(len_a ? len_a : 1);
    uint8_t *buf_b = malloc(len_b ? len_b : 1);
    __CPROVER_assume(buf_a != NULL);
    __CPROVER_assume(buf_b != NULL);

    a.len = len_a;
    a.ptr = buf_a;
    b.len = len_b;
    b.ptr = buf_b;

    /* Ensure cursors are valid (ptr non-null, len within bounds) */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save original state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call the function */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Postcondition: cursors unchanged */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* Postcondition: result depends on equality */
    if (a.len != b.len) {
        assert(!result);
    } else {
        bool all_equal = true;
        for (size_t i = 0; i < a.len; i++) {
            if (a.ptr[i] != b.ptr[i]) {
                all_equal = false;
                break;
            }
        }
        assert(result == all_equal);
    }

    /* Postcondition: cursors remain valid */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    free(buf_a);
    free(buf_b);
}
