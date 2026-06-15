#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);
extern char nondet_char(void);
extern bool nondet_bool(void);

void aws_byte_cursor_from_c_str_harness(void) {
    size_t max_len = nondet_size_t();
    __CPROVER_assume(max_len < 256);

    bool is_null = nondet_bool();
    char *c_str = NULL;
    if (!is_null) {
        c_str = malloc(max_len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < max_len; ++i) {
            c_str[i] = nondet_char();
        }
        size_t null_pos = nondet_size_t();
        __CPROVER_assume(null_pos <= max_len);
        c_str[null_pos] = '\0';
        c_str[max_len] = '\0';
    }

    char *c_str_snapshot = NULL;
    if (c_str != NULL) {
        c_str_snapshot = malloc(max_len + 1);
        __CPROVER_assume(c_str_snapshot != NULL);
        memcpy(c_str_snapshot, c_str, max_len + 1);
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    assert(aws_byte_cursor_is_valid(&cur));

    if (c_str != NULL) {
        size_t expected_len = strlen(c_str);
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == expected_len);
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    if (c_str != NULL) {
        for (size_t i = 0; i < max_len + 1; ++i) {
            assert(c_str[i] == c_str_snapshot[i]);
        }
    }

    if (c_str != NULL) {
        free(c_str);
        free(c_str_snapshot);
    }
}
