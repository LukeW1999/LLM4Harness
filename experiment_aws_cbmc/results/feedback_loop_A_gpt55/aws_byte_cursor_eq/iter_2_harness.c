#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

size_t nondet_size_t(void);

static void make_bounded_allocated_byte_cursor(struct aws_byte_cursor *cursor) {
    cursor->len = nondet_size_t();
    __CPROVER_assume(cursor->len <= MAX_BUFFER_SIZE);

    if (cursor->len > 0) {
        cursor->ptr = malloc(cursor->len);
        __CPROVER_assume(cursor->ptr != NULL);
    } else {
        cursor->ptr = NULL;
    }

    __CPROVER_assume(aws_byte_cursor_is_bounded(cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(cursor));
}

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    make_bounded_allocated_byte_cursor(&a);
    make_bounded_allocated_byte_cursor(&b);

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;
    save_byte_from_array(a.ptr, a.len, &old_a_byte);
    save_byte_from_array(b.ptr, b.len, &old_b_byte);

    bool result = aws_byte_cursor_eq(&a, &b);

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    assert_byte_from_buffer_matches(a.ptr, &old_a_byte);
    assert_byte_from_buffer_matches(b.ptr, &old_b_byte);

    assert(result == aws_array_eq(a.ptr, a.len, b.ptr, b.len));

    if (result) {
        assert(a.len == b.len);
        assert_bytes_match(a.ptr, b.ptr, a.len);
    } else {
        assert(!aws_array_eq(a.ptr, a.len, b.ptr, b.len));
    }

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
