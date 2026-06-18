#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static bool aws_byte_cursor_eq_spec(const struct aws_byte_cursor *a, const struct aws_byte_cursor *b) {
    if (a->len != b->len) {
        return false;
    }

    for (size_t i = 0; i < a->len; ++i) {
        if (a->ptr[i] != b->ptr[i]) {
            return false;
        }
    }

    return true;
}

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    __CPROVER_assume(AWS_MEM_IS_READABLE(a.ptr, a.len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(b.ptr, b.len));

    bool expected = aws_byte_cursor_eq_spec(&a, &b);

    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;
    save_byte_from_array(a.ptr, a.len, &old_a_byte);
    save_byte_from_array(b.ptr, b.len, &old_b_byte);

    bool result = aws_byte_cursor_eq(&a, &b);

    if (result) {
        assert(expected);
        assert(old_a.len == old_b.len);
        assert_bytes_match(old_a.ptr, old_b.ptr, old_a.len);
    } else {
        assert(!expected);
        if (old_a.len == old_b.len) {
            bool mismatch_found = false;
            for (size_t i = 0; i < old_a.len; ++i) {
                if (old_a.ptr[i] != old_b.ptr[i]) {
                    mismatch_found = true;
                }
            }
            assert(mismatch_found);
        } else {
            assert(old_a.len != old_b.len);
        }
    }

    assert(result == expected);

    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    assert_byte_from_buffer_matches(a.ptr, &old_a_byte);
    assert_byte_from_buffer_matches(b.ptr, &old_b_byte);

    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
