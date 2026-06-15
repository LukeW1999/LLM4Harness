#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str;
    size_t str_len = 0;
    char str_data[MAX_BUFFER_SIZE + 1];

    /* Non-deterministically choose whether c_str is NULL or a valid string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Choose a non-deterministic length for the string (0 to MAX_BUFFER_SIZE) */
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Fill the string with non-deterministic characters */
        for (size_t i = 0; i < str_len; i++) {
            str_data[i] = nondet_uint8_t();
        }
        str_data[str_len] = '\0';  /* Null-terminate */

        c_str = str_data;
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* Postcondition: returned cursor is valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* Postcondition: ptr and len correctly reflect the input */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
        /* Check that the string remains null-terminated */
        assert(c_str[str_len] == '\0');
    }

    /* Additional: the function does not modify global state (none) */
}
