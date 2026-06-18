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

    /* 2. Allocate a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper NUL‑terminated string */
    c_str[c_str_len] = '\0';

    /* 3. Save old state of the buffer */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_storage;
    save_byte_from_array(buf.buffer, buf.capacity, &old_buf_storage);

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert that no fields of the buffer changed */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* 6. Assert that the buffer's contents are unchanged */
    assert_byte_from_buffer_matches(buf.buffer, &old_buf_storage);

    /* 7. Assert that the buffer remains valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* 8. Result must be a boolean value */
    assert(result == true || result == false);
}
