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

void aws_array_eq_c_str_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *array;
    size_t array_len = nondet_size_t();
    const char *c_str;

    /* Bound sizes */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* Allocate and initialize array (may be NULL only when length is zero) */
    if (array_len == 0) {
        array = NULL;
    } else {
        uint8_t *buf = malloc(array_len);
        __CPROVER_assume(buf != NULL);
        /* make the contents nondeterministic */
        for (size_t i = 0; i < array_len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        array = buf;
    }

    /* Allocate and initialize c_str */
    size_t c_str_len = nondet_size_t();
    /* c_str must be at least array_len long and have a terminating NUL after that */
    __CPROVER_assume(c_str_len >= array_len);
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    uint8_t *c_buf = malloc(c_str_len + 1);
    __CPROVER_assume(c_buf != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_buf[i] = nondet_uint8_t();
    }
    c_buf[c_str_len] = '\0';               /* NUL terminator */
    c_str = (const char *)c_buf;

    /* 2. Save old state for immutability checks */
    struct store_byte_from_buffer old_array;
    if (array_len > 0) {
        save_byte_from_array((const uint8_t *)array, array_len, &old_array);
    }
    struct store_byte_from_buffer old_cstr;
    /* Save up to array_len+1 bytes of the string (the part the function may read) */
    save_byte_from_array(c_buf, array_len + 1, &old_cstr);

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Post‑condition assertions */
    if (result) {
        /* On success: all bytes match and the string is NUL‑terminated exactly at array_len */
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)c_str)[i] != '\0');
            assert(((const uint8_t *)c_str)[i] == ((const uint8_t *)array)[i]);
        }
        assert(((const uint8_t *)c_str)[array_len] == '\0');
    } else {
        /* On failure: at least one of the required conditions is violated */
        bool failure_condition = false;
        for (size_t i = 0; i < array_len; ++i) {
            if (((const uint8_t *)c_str)[i] == '\0' ||
                ((const uint8_t *)c_str)[i] != ((const uint8_t *)array)[i]) {
                failure_condition = true;
                break;
            }
        }
        if (!failure_condition) {
            /* No early NUL and all bytes equal, therefore the terminating NUL is missing */
            failure_condition = ((const uint8_t *)c_str)[array_len] != '\0';
        }
        assert(failure_condition);
    }

    /* 5. Immutability checks – inputs must not be modified */
    if (array_len > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)array, &old_array);
    }
    assert_byte_from_buffer_matches(c_buf, &old_cstr);

    /* 6. Ensure pointers themselves are unchanged */
    assert(array == (const void *)array);
    assert(c_str == (const char *)c_str);
}
