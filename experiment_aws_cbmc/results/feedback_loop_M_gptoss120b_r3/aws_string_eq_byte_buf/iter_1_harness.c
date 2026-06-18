#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *str;
    struct aws_byte_buf buf;

    /* str may be NULL or a valid string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t str_len;
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        /* allocate space for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (str_len > 0 ? str_len - 1 : 0));
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        /* const fields can be set via cast away constness for harness */
        *((size_t *)&str->len) = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* buf may be NULL or a valid byte buffer */
    if (nondet_bool()) {
        /* represent NULL buffer by setting pointer to NULL and length 0 */
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL;
    } else {
        /* allocate buffer structure */
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        __CPROVER_assume(buf.capacity >= buf.len);
        /* fill buffer with nondet data */
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf.buffer != NULL) {
        save_byte_from_array(buf.buffer, buf.len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* 4. Assert postconditions */
    if (str == NULL && buf.buffer == NULL) {
        assert(result == true);
    } else if (str == NULL || buf.buffer == NULL) {
        assert(result == false);
    } else {
        assert(result == aws_array_eq(str->bytes, str->len, buf.buffer, buf.len));
    }

    /* 5. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    if (buf.buffer != NULL) {
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_bytes);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
