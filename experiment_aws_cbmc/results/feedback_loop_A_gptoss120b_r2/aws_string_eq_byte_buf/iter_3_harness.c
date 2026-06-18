#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs */
    struct aws_string *str;
    struct aws_byte_buf buf;
    const struct aws_byte_buf *buf_ptr;

    /* nondeterministically decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate on stack enough space for the flexible array member */
        uint8_t str_storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)str_storage;

        /* nondeterministically choose a length within bounds */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

        /* allocator may be NULL (static string) or a default allocator */
        str->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* the bytes are part of the allocation; no need to initialise contents */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* initialise a byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* nondeterministically set length within capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = aws_default_allocator();

    /* nondeterministically decide if the buffer pointer itself is NULL */
    if (nondet_bool()) {
        buf_ptr = NULL;
    } else {
        buf_ptr = &buf;
    }

    /* 2. Save old state */
    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    uint8_t old_str_bytes[MAX_BUFFER_SIZE];
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        for (size_t i = 0; i < str->len; ++i) {
            old_str_bytes[i] = str->bytes[i];
        }
    }

    struct aws_byte_buf old_buf = buf;
    uint8_t old_buf_bytes[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < buf.len; ++i) {
        old_buf_bytes[i] = buf.buffer[i];
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* 4. Post‑condition: return value matches the specification */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
    }
    assert(result == expected);

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->allocator == old_buf.allocator);
        assert(buf_ptr->capacity == old_buf.capacity);
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->buffer == old_buf.buffer);
        for (size_t i = 0; i < buf_ptr->len; ++i) {
            assert(buf_ptr->buffer[i] == old_buf_bytes[i]);
        }
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf_ptr != NULL) {
        assert(aws_byte_buf_is_bounded(buf_ptr, MAX_BUFFER_SIZE));
    }
}
