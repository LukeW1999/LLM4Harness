#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    /* nondeterministic input: possibly NULL string */
    bool is_null = nondet_bool();
    const char *c_str = NULL;
    size_t len = 0;

    if (!is_null) {
        /* bound the length of the string */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate a buffer for the string (including null terminator) */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);

        /* ensure the string has exactly 'len' characters before the terminator */
        for (size_t i = 0; i < len; ++i) {
            char ch = nondet_char();
            __CPROVER_assume(ch != '\0');
            tmp[i] = ch;
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
    } else {
        assert(buf.len == len);
        assert(buf.capacity == len);
        assert(buf.buffer == (uint8_t *)c_str);
    }

    /* allocator is always NULL for this constructor */
    assert(buf.allocator == NULL);

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
