#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_CURSOR_LEN 256

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_byte_cursor_advance_harness(void) {
    /* Allocate a bounded buffer for the cursor */
    uint8_t *buf = malloc(MAX_CURSOR_LEN);
    __CPROVER_assume(buf != NULL);

    /* Nondeterministically choose a length that fits in the buffer */
    size_t init_len = nondet_size_t();
    __CPROVER_assume(init_len <= MAX_CURSOR_LEN);

    struct aws_byte_cursor cursor = {
        .ptr = buf,
        .len = init_len
    };

    /* Ensure the cursor is valid according to the library's invariants */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Choose an amount to advance */
    size_t amount = nondet_size_t();

    /* Branch to make both success and failure paths reachable */
    bool succeed = nondet_bool();
    if (succeed) {
        __CPROVER_assume(amount <= cursor.len);
    } else {
        __CPROVER_assume(amount > cursor.len);
    }

    struct aws_byte_cursor old_cursor = cursor;

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.ptr == (const uint8_t *)old_cursor.ptr + amount);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));

    free(buf);
}
