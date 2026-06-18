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

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Declare and bound a C‑string */
    char *c_str;
    size_t c_str_len = nondet_size_t();
    /* bound the string length to keep the state space finite */
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* nondet contents */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0'; /* null‑terminate */

    /* 3. Save old state of the buffer */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer buf_storage;
    if (buf.buffer != NULL && buf.capacity > 0) {
        save_byte_from_array(buf.buffer, buf.capacity, &buf_storage);
    }

    /* 4. Save old state of the C‑string */
    char *old_c_str = c_str;
    struct store_byte_from_buffer cstr_storage;
    if (c_str_len > 0) {
        save_byte_from_array((uint8_t *)c_str, c_str_len, &cstr_storage);
    }

    /* 5. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);
    (void)result; /* result is not used for further reasoning in this harness */

    /* 6. Assert that nothing in the buffer changed */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    if (buf.buffer != NULL && buf.capacity > 0) {
        assert_bytes_match(buf.buffer, old_buf.buffer, buf.capacity);
    }

    /* 7. Assert that the C‑string did not change */
    assert(c_str == old_c_str);
    if (c_str_len > 0) {
        assert_bytes_match((uint8_t *)c_str, (uint8_t *)old_c_str, c_str_len);
    }

    /* 8. The buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean up */
    free(c_str);
}
