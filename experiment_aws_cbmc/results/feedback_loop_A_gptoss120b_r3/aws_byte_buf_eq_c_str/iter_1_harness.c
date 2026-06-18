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
    /* 1. Declare and bound the aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Allocate and bound a null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper C string */
    c_str[c_str_len] = '\0';
    /* nondet contents for the characters */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }

    /* 3. Save old state for immutability checks */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_storage;
    save_byte_from_array(buf.buffer, buf.len, &old_buf_storage);

    struct store_byte_from_buffer old_cstr_storage;
    save_byte_from_array((const uint8_t *)c_str, c_str_len, &old_cstr_storage);

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* 5a. The function must not modify the buffer */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.len == old_buf.len);
    assert_bytes_match(buf.buffer, old_buf.buffer, buf.len);

    /* 5b. The C string must not be modified */
    assert(c_str != NULL);                     /* pointer unchanged */
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_cstr_storage);
    /* also ensure the terminating null byte is unchanged */
    assert(c_str[c_str_len] == '\0');

    /* 5c. Return value is a boolean – no additional constraints needed */

    /* 6. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean up */
    free(c_str);
}
