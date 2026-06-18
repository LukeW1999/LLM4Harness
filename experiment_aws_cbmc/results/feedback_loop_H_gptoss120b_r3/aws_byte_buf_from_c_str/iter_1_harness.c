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
    /* 1. Non‑deterministic input string */
    const char *c_str;
    bool is_null = nondet_bool();

    size_t str_len = 0; /* valid when c_str != NULL */

    if (is_null) {
        c_str = NULL;
    } else {
        /* bound the length of the string */
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* fill with non‑zero bytes and terminate */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)c_str)[i] = nondet_uint8_t();
            __CPROVER_assume(((uint8_t *)c_str)[i] != 0);
        }
        ((uint8_t *)c_str)[str_len] = 0;
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Expected values */
    size_t expected_len = (c_str == NULL) ? 0 : str_len;
    uint8_t *expected_buf = (expected_len == 0) ? NULL : (uint8_t *)c_str;

    /* 4. Post‑condition asserts */
    assert(buf.len == expected_len);
    assert(buf.capacity == expected_len);
    assert(buf.buffer == expected_buf);
    assert(buf.allocator == NULL);

    /* 5. Structural validity */
    assert(aws_byte_buf_is_valid(&buf));
}
