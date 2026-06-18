#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness() {
    /* nondeterministic input: possibly NULL string */
    bool is_null = nondet_bool();
    const char *c_str = NULL;
    size_t len = 0;

    if (!is_null) {
        /* bound the length of the string */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate a buffer for the string (including terminating NUL) */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);

        /* fill with non‑zero characters to guarantee strlen == len */
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = 'a';
        }
        tmp[len] = '\0';
        c_str = tmp;
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* post‑conditions */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        size_t actual_len = strlen(c_str);
        assert(buf.len == actual_len);
        /* capacity is expected to be at least len+1 (includes NUL) */
        assert(buf.capacity == actual_len + 1);
        assert(buf.buffer != NULL);
        /* allocator may be NULL or non‑NULL depending on implementation */
    }

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
