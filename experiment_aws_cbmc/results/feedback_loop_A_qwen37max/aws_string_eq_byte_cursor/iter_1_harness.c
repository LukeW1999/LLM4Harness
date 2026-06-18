#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness() {
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        union {
            struct aws_string *c;
            struct {
                struct aws_allocator *allocator;
                size_t len;
                uint8_t bytes[1];
            } *m;
        } u;
        
        u.c = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(u.c != NULL);
        u.m->allocator = NULL;
        u.m->len = len;
        str = u.c;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        if (str->len != cur->len) {
            assert(result == false);
        }
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
