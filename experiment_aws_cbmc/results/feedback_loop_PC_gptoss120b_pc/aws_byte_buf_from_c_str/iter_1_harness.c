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
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. nondeterministic input string (may be NULL) */
    const char *c_str;
    if (nondet_bool()) {
        /* allocate a nondeterministic sized, null‑terminated buffer */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }
        buf[len] = '\0';
        c_str = buf;
    } else {
        c_str = NULL;
    }

    /* 2. Save old state of the input memory (if any) */
    struct store_byte_from_buffer old_mem;
    if (c_str != NULL) {
        /* store up to MAX_BUFFER_SIZE bytes – enough for any possible strlen */
        save_byte_from_array((const uint8_t *)c_str, MAX_BUFFER_SIZE, &old_mem);
    }

    /* 3. Call function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 4. Post‑conditions for the returned struct */
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
    assert(result.allocator == NULL);

    /* 5. Input memory must remain unchanged */
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_mem);
    }

    /* 6. Structural validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
