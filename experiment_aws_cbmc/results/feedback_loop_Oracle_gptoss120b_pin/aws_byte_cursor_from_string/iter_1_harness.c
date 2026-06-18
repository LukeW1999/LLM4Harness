#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Maximum length for the nondeterministic string */
#define MAX_STR_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* Allocate a nondeterministic C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_STR_LEN);

    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = nondet_char();
    }
    c_str[c_len] = '\0';

    /* Create a valid aws_string from the C string */
    struct aws_string *src = aws_string_new_from_c_str(aws_default_allocator(), c_str);
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_string_is_valid(src));

    /* Snapshot the string's contents for frame condition checks */
    size_t src_len_before = src->len;
    uint8_t *src_bytes_before = malloc(src_len_before);
    __CPROVER_assume(src_bytes_before != NULL);
    memcpy(src_bytes_before, aws_string_bytes(src), src_len_before);

    /* Call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* Post‑condition 1: returned cursor points to the string's bytes */
    assert(cur.ptr == aws_string_bytes(src));

    /* Post‑condition 2: returned cursor length equals the string length */
    assert(cur.len == src->len);

    /* Post‑condition 3: the source string is unchanged (frame condition) */
    assert(src->len == src_len_before);
    assert(memcmp(src_bytes_before, aws_string_bytes(src), src_len_before) == 0);

    /* Clean up */
    free(src_bytes_before);
    aws_string_destroy(src);
    free(c_str);

    return 0;
}
