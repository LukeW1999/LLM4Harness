#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include <aws/common/byte_buf.h>
#include <aws/common/array.h>
#include <aws/common/common.h>
#include <aws/common/array_list.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

size_t nondet_size_t(void);
bool aws_array_eq_c_str(const uint8_t *array, size_t array_len, const char *c_str);

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
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    ((unsigned char *)c_str)[c_str_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_buf old = buf;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Post‑condition: result must equal the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 6. Unchanged fields (function is pure) */
    assert(buf.buffer   == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.len      == old.len);

    /* 7. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* clean up */
    free(c_str);
}
