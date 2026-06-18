#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Maximum length for the nondeterministic string (to keep verification tractable) */
#define MAX_STR_LEN 256

void aws_byte_buf_from_c_str_harness(void) {
    /* Nondeterministic choice: either NULL or a valid C string */
    bool is_null = nondet_bool();
    const char *c_str = NULL;

    /* Buffers to hold the original string and a copy for later comparison */
    char *orig_str = NULL;
    size_t str_len = 0;

    if (!is_null) {
        /* Choose a nondeterministic length for the string (including the terminating NUL) */
        str_len = nondet_uint();
        __CPROVER_assume(str_len <= MAX_STR_LEN);

        /* Allocate memory for the string (+1 for the terminating NUL) */
        char *tmp = malloc(str_len + 1);
        __CPROVER_assume(tmp != NULL);
        /* Fill with nondeterministic bytes */
        for (size_t i = 0; i < str_len; ++i) {
            tmp[i] = nondet_char();
        }
        /* Ensure NUL termination */
        tmp[str_len] = '\0';

        c_str = tmp;

        /* Keep a copy of the original contents for frame condition checks */
        orig_str = malloc(str_len + 1);
        __CPROVER_assume(orig_str != NULL);
        memcpy(orig_str, tmp, str_len + 1);
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 1. Return value / error code correctness (validity predicate) */
    assert(aws_byte_buf_is_valid(&result));

    /* 2. Output buffer invariants */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity >= expected_len);
        if (expected_len > 0) {
            assert(result.buffer != NULL);
            assert(memcmp(result.buffer, c_str, expected_len) == 0);
        }
    }

    /* 3. Memory not modified beyond the function's contract (frame conditions) */
    if (c_str != NULL) {
        /* The original string should remain unchanged */
        assert(memcmp(c_str, orig_str, str_len + 1) == 0);
    }

    /* Clean up */
    if (c_str != NULL) {
        free((void *)c_str);
        free(orig_str);
        /* If the function allocated a new buffer, free it */
        if (result.buffer != (uint8_t *)c_str) {
            free((void *)result.buffer);
        }
    }

    return;
}
