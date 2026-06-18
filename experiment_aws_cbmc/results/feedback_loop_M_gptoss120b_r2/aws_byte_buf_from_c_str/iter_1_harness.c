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

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministically decide whether the input string is NULL */
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t str_len = 0;

    if (!is_null) {
        /* allocate a buffer large enough for the maximum allowed length plus NUL */
        c_str = malloc(MAX_BUFFER_SIZE + 1);
        __CPROVER_assume(c_str != NULL);

        /* nondeterministic length bounded by MAX_BUFFER_SIZE */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* fill the buffer with nondeterministic bytes */
        for (size_t i = 0; i < str_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        /* NUL‑terminate */
        c_str[str_len] = '\0';
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Expected values derived from the specification / implementation */
    size_t expected_len = (c_str == NULL) ? 0 : strlen(c_str);
    size_t expected_capacity = expected_len;

    /* Post‑condition asserts */
    assert(buf.len == expected_len);
    assert(buf.capacity == expected_capacity);

    if (expected_len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* allocator is always set to NULL by the function */
    assert(buf.allocator == NULL);

    /* Validity invariant must hold for the resulting buffer */
    assert(aws_byte_buf_is_valid(&buf));
}
