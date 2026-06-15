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
    /* nondeterministic length bounded by MAX_BUFFER_SIZE */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* nondeterministic choice of NULL or a valid C string */
    bool is_null = nondet_bool();
    const char *c_str = NULL;

    struct store_byte_from_buffer storage;
    if (!is_null) {
        /* allocate a buffer of len+1 bytes (including terminating '\\0') */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);
        c_str = tmp;

        /* ensure the string has exactly 'len' non‑zero characters followed by '\\0' */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)tmp)[i] = nondet_uint8_t();
            __CPROVER_assume(((uint8_t *)tmp)[i] != 0);
        }
        ((uint8_t *)tmp)[len] = 0;

        /* save the original bytes for immutability check */
        if (len > 0) {
            save_byte_from_array((const uint8_t *)c_str, len, &storage);
        }
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* post‑conditions */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        assert(buf.len == len);
        assert(buf.capacity == len);
        if (len == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer == (uint8_t *)c_str);
            /* the input string must remain unchanged */
            assert_byte_from_buffer_matches((const uint8_t *)c_str, &storage);
        }
    }
    assert(buf.allocator == NULL);

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
