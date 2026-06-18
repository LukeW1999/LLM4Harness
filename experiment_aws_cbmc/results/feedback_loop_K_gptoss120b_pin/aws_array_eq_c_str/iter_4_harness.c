#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/* prototype of the function under test */
bool aws_array_eq_c_str(const void *const array, const size_t array_len, const char *const c_str);

void aws_array_eq_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* bound the lengths to avoid overflow and zero‑size allocations */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len > 0 && array_len < 256);

    const uint8_t *array = NULL;
    char *c_str = NULL;

    /* allocate and initialize array */
    array = aws_mem_acquire(allocator, array_len);
    __CPROVER_assume(array != NULL);
    for (size_t i = 0; i < array_len; ++i) {
        ((uint8_t *)array)[i] = nondet_uint8_t();
    }

    /* allocate and initialize c_str */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len >= array_len && c_str_len < 256);
    c_str = aws_mem_acquire(allocator, c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_char();
    }
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos >= array_len && null_pos <= c_str_len);
    c_str[null_pos] = '\0';
    for (size_t i = null_pos + 1; i <= c_str_len; ++i) {
        c_str[i] = nondet_char();
    }

    /* make copies for frame checking */
    uint8_t *array_copy = aws_mem_acquire(allocator, array_len);
    __CPROVER_assume(array_copy != NULL);
    memcpy(array_copy, array, array_len);

    char *c_str_copy = aws_mem_acquire(allocator, c_str_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, c_str_len + 1);

    /* call the function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* compute expected result */
    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        if (c_str[i] == '\0' ||
            ((const uint8_t *)array)[i] != (uint8_t)c_str[i]) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    /* assert postcondition */
    assert(result == expected);

    /* frame checks */
    assert(memcmp(array, array_copy, array_len) == 0);
    assert(memcmp(c_str, c_str_copy, c_str_len + 1) == 0);

    /* clean up */
    aws_mem_release(allocator, (void *)array);
    aws_mem_release(allocator, c_str);
    aws_mem_release(allocator, array_copy);
    aws_mem_release(allocator, c_str_copy);
}
