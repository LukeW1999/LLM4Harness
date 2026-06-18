#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Create a valid null-terminated C string */
    size_t c_str_max_len = MAX_BUFFER_SIZE;
    char *c_str = malloc(c_str_max_len);
    __CPROVER_assume(c_str != NULL);

    /* Fill with nondeterministic bytes */
    for (size_t i = 0; i < c_str_max_len; i++) {
        c_str[i] = nondet_uint8_t();
    }

    /* Place a null terminator at a nondeterministic position within bounds */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos < c_str_max_len);
    c_str[null_pos] = '\0';
    size_t c_str_len = null_pos; /* length of the string */

    /* 3. Save old state for immutability checks */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_byte_storage;
    if (buf.len > 0 && buf.buffer != NULL) {
        save_byte_from_array(buf.buffer, buf.len, &buf_byte_storage);
    }

    struct store_byte_from_buffer c_str_byte_storage;
    save_byte_from_array((uint8_t *)c_str, c_str_len + 1, &c_str_byte_storage);

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Postconditions */

    /* 5a. The byte buffer must remain unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    if (old_buf.len > 0 && old_buf.buffer != NULL) {
        assert_bytes_match(buf.buffer, old_buf.buffer, old_buf.len);
    }

    /* 5b. The C string must remain unchanged */
    assert_byte_from_buffer_matches((uint8_t *)c_str, &c_str_byte_storage);

    /* 5c. The byte buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* 5d. The return value must correctly reflect content equality */
    bool expected = (buf.len == c_str_len);
    if (expected) {
        for (size_t i = 0; i < buf.len; i++) {
            if (buf.buffer[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
    }
    assert(result == expected);

    /* 6. Free allocated memory */
    free(c_str);
    if (buf.buffer != NULL) {
        free(buf.buffer);
    }
}
