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

/* Bounding constants are provided by the build system */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_array_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len > 0);               /* aws_array_eq_c_str requires non‑NULL array */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    /* Allocate array buffer (must be non‑NULL) */
    uint8_t *array_buf = malloc(array_len);
    __CPROVER_assume(array_buf != NULL);
    for (size_t i = 0; i < array_len; ++i) {
        array_buf[i] = nondet_uint8_t();
    }
    const void *array = (const void *)array_buf;

    /* Allocate a null‑terminated C string */
    char *c_str_buf = malloc(c_str_len + 1);
    __CPROVER_assume(c_str_buf != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    c_str_buf[c_str_len] = '\0';
    const char *c_str = (const char *)c_str_buf;

    /* 2. Save old state (pointers and lengths are immutable) */
    const void *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Unchanged fields */
    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    /* 5. Post‑condition: result reflects exact equality */
    bool all_match = true;
    for (size_t i = 0; i < array_len; ++i) {
        uint8_t a_byte = ((const uint8_t *)array)[i];
        uint8_t s_byte = (uint8_t)c_str[i];
        if (a_byte != s_byte) {
            all_match = false;
        }
    }
    bool condition = all_match && (c_str[array_len] == '\0');
    assert(result == condition);
}
