#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_cursor_harness() {
    /* parameters */
    struct aws_string *str;
    struct aws_byte_cursor cur;

    /* allocate and assume bounds for str */
    str = malloc(sizeof(*str) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    str->allocator = nondet_pointer();
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_STRING_LEN);
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)str);
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
    str->bytes[str->len] = 0; // null terminator

    /* allocate and assume bounds for cur */
    cur.ptr = malloc(MAX_STRING_LEN);
    __CPROVER_assume(cur.ptr != NULL);
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_STRING_LEN);
    __CPROVER_assume(AWS_MEM_IS_READABLE(cur.ptr, cur.len));

    /* save old state */
    struct aws_string old_str = *str;
    struct aws_byte_cursor old_cur = cur;

    /* operation under verification */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* assertions */
    if (str == NULL && cur.ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || cur.ptr == NULL) {
        assert(result == false);
    } else {
        if (result == true) {
            assert_bytes_match(str->bytes, cur.ptr, str->len);
            assert(str->len == cur.len);
        } else {
            assert(!bytes_match(str->bytes, cur.ptr, str->len) || str->len != cur.len);
        }
    }

    /* unchanged fields */
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes == old_str.bytes);

    /* validity invariants */
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_valid(&cur));
}
