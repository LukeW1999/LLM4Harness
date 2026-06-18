#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. Non‑deterministic allocation of aws_string (may be NULL) */
    struct aws_string *str = NULL;
    uint8_t *str_bytes = NULL;
    size_t str_len = 0;

    if (!nondet_bool()) {                     /* str is NULL */
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        str_bytes = malloc(str_len);
        __CPROVER_assume(str_bytes != NULL);
        for (size_t i = 0; i < str_len; ++i) {
            str_bytes[i] = nondet_uint8_t();
        }

        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = str_len;
        memcpy((void *)str->bytes, str_bytes, str_len);

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Non‑deterministic allocation of aws_byte_cursor (may be NULL) */
    struct aws_byte_cursor *cur = NULL;
    struct aws_byte_cursor cur_obj;

    if (!nondet_bool()) {                     /* cur is NULL */
        cur = NULL;
    } else {
        ensure_byte_cursor_has_allocated_buffer_member(&cur_obj);
        __CPROVER_assume(cur_obj.len <= MAX_BUFFER_SIZE);
        for (size_t i = 0; i < cur_obj.len; ++i) {
            ((uint8_t *)cur_obj.ptr)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_obj, MAX_BUFFER_SIZE));
        cur = &cur_obj;
    }

    /* 3. Save old state for immutability checks */
    struct aws_string old_str_struct;
    uint8_t *old_str_bytes = NULL;
    if (str != NULL) {
        old_str_struct = *str;
        old_str_bytes = malloc(str_len);
        __CPROVER_assume(old_str_bytes != NULL);
        memcpy(old_str_bytes, (void *)str->bytes, str_len);
    }

    struct aws_byte_cursor old_cur;
    uint8_t *old_cur_bytes = NULL;
    if (cur != NULL) {
        old_cur = *cur;
        old_cur_bytes = malloc(cur->len);
        __CPROVER_assume(old_cur_bytes != NULL);
        memcpy(old_cur_bytes, cur->ptr, cur->len);
    }

    /* 4. Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 5. Post‑condition: return value semantics */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    } else {
        /* both non‑NULL: result must reflect byte‑wise equality */
        bool equal = true;
        if (str->len != cur->len) {
            equal = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != ((uint8_t *)cur->ptr)[i]) {
                    equal = false;
                    break;
                }
            }
        }
        assert(result == equal);
    }

    /* 6. Unchanged fields: aws_string */
    if (str != NULL) {
        assert(str->allocator == old_str_struct.allocator);
        assert(str->len == old_str_struct.len);
        for (size_t i = 0; i < str->len; ++i) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
    }

    /* 7. Unchanged fields: aws_byte_cursor */
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
        for (size_t i = 0; i < cur->len; ++i) {
            assert(((uint8_t *)cur->ptr)[i] == old_cur_bytes[i]);
        }
    }

    /* 8. Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }

    /* 9. Free allocated helper buffers */
    free(str_bytes);
    free(old_str_bytes);
    free(old_cur_bytes);
}
