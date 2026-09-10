#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* Non-deterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor cur_val;
    struct aws_byte_cursor *cur = NULL;

    if (!str_is_null) {
        /* Allocate a valid aws_string */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* Allocate memory for aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields (cast away const for initialization) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = str_len;
        /* bytes are nondet by default from malloc */
        /* ensure null terminator */
        ((uint8_t *)str->bytes)[str_len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        /* Set up a valid byte cursor */
        cur_val.len = nondet_size_t();
        __CPROVER_assume(cur_val.len <= MAX_BUFFER_SIZE);

        if (cur_val.len > 0) {
            cur_val.ptr = malloc(cur_val.len);
            __CPROVER_assume(cur_val.ptr != NULL);
        } else {
            cur_val.ptr = NULL;
        }

        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Expert postconditions */
    if (result && str) {
        assert(aws_string_is_valid(str));
        if (!cur_is_null) {
            assert(str->len == cur->len);
            if (str->len > 0) {
                assert_bytes_match(str->bytes, cur->ptr, str->len);
            }
        }
    }
    if (!cur_is_null) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
