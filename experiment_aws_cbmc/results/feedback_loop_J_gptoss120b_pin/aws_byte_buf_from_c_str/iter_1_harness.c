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
    /* 1. Nondeterministically choose a NULL or a valid C string */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = nondet_size_t();
        __CPROVER_assume(max_len <= MAX_BUFFER_SIZE);
        /* allocate space for max_len characters plus a terminating NUL */
        char *buf = malloc(max_len + 1);
        __CPROVER_assume(buf != NULL);
        /* fill with nondeterministic bytes */
        for (size_t i = 0; i < max_len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        /* place a NUL somewhere within the allocated range */
        size_t null_pos = nondet_size_t();
        __CPROVER_assume(null_pos <= max_len);
        buf[null_pos] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Post‑condition checks */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    /* allocator is always set to NULL */
    assert(result.allocator == NULL);

    /* 4. Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&result));
}
