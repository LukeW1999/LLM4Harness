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
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Nondeterministically decide whether the input string pointer is NULL */
    bool is_null = nondet_bool();

    const char *c_str = NULL;
    size_t allocated_len = 0;               /* length of the allocated buffer (excluding the terminating NUL) */

    if (!is_null) {
        /* 2. Allocate a bounded, null‑terminated string */
        __CPROVER_assume(allocated_len <= MAX_BUFFER_SIZE);
        c_str = malloc(allocated_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Fill the allocated region with nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)c_str;
        for (size_t i = 0; i < allocated_len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        /* Ensure null termination */
        bytes[allocated_len] = 0;
    }

    /* 3. Save the original contents of the string (including the terminating NUL) */
    struct store_byte_from_buffer old_bytes;
    if (!is_null) {
        save_byte_from_array((const uint8_t *)c_str, allocated_len + 1, &old_bytes);
    }

    /* 4. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 5. Post‑condition checks */
    if (is_null) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (const uint8_t *)c_str);
        /* strlen on the concrete string must equal the length stored in the cursor */
        assert(cur.len == strlen(c_str));
    }

    /* 6. The input string must remain unchanged */
    if (!is_null) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_bytes);
    }

    /* 7. The resulting cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
