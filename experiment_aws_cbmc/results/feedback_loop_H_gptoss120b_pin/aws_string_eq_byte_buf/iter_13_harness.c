#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness() {
    /* 1. Declare and bound inputs */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        unsigned char str_mem[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_mem;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= MAX_BUFFER_SIZE);
    buf.len = buf_len;
    __CPROVER_assume(buf.capacity >= buf.len);
    __CPROVER_assume(buf.buffer != NULL || buf.len == 0);

    /* Save old state of buf */
    struct aws_byte_buf old_buf = buf;

    /* Save old state of str if not NULL */
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
    }

    /* Save byte contents for immutability checks */
    struct store_byte_from_buffer str_bytes_store;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(str->bytes, str->len, &str_bytes_store);
    }
    struct store_byte_from_buffer buf_bytes_store;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &buf_bytes_store);
    }

    /* 2. Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* 3. Postcondition checks */
    if (str == NULL && buf.buffer == NULL && buf.len == 0) {
        assert(result);
    } else if (str == NULL || (buf.buffer == NULL && buf.len != 0)) {
        assert(!result);
    } else {
        bool expected = false;
        if (str->len == buf.len) {
            if (str->len == 0) {
                expected = true;
            } else {
                expected = (memcmp(str->bytes, buf.buffer, str->len) == 0);
            }
        }
        assert(result == expected);
        if (expected) {
            assert(str->len == buf.len);
        }
    }

    /* 4. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &str_bytes_store);
        }
    }
    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &buf_bytes_store);
    }

    /* 5. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
