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

    /* 2. Declare and bound the C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_len + 1));
    c_str[c_len] = '\0'; /* ensure null‑termination */

    /* 3. Save old state */
    struct aws_byte_buf old = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.len, &old_byte);
    size_t str_len = strlen(c_str);
    struct store_byte_from_buffer old_byte_from_str;
    save_byte_from_array((uint8_t *)c_str, str_len, &old_byte_from_str);

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Post‑condition checks */
    if (result) {
        assert(buf.len == str_len);
        if (buf.len > 0) {
            assert_bytes_match(buf.buffer, (uint8_t *)c_str, buf.len);
        }
    }

    /* 6. Unchanged fields (function is pure) */
    assert(buf.buffer == old.buffer);
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* 7. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
    assert_byte_buf_equivalence(&buf, &old, &old_byte);
    if (str_len > 0) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_byte_from_str);
    }

    /* 8. Clean up */
    free(c_str);
}
