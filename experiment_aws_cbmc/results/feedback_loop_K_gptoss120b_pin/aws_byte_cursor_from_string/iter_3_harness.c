#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

static struct aws_string *make_nondet_aws_string(void) {
    bool is_null = __CPROVER_nondet_bool();
    if (is_null) {
        return NULL;
    }

    size_t len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    uint8_t *buf = (uint8_t *)malloc(len);
    __CPROVER_assume(buf != NULL);
    for (size_t i = 0; i < len; ++i) {
        buf[i] = __CPROVER_nondet_uchar();
    }

    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *s = aws_string_new_from_array(allocator, buf, len);
    free(buf);
    return s;
}

static bool aws_string_equal(const struct aws_string *a, const struct aws_string *b) {
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }
    if (a->len != b->len) {
        return false;
    }
    for (size_t i = 0; i < a->len; ++i) {
        if (a->bytes[i] != b->bytes[i]) {
            return false;
        }
    }
    return true;
}

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src = make_nondet_aws_string();

    struct aws_string *src_copy = NULL;
    if (src != NULL) {
        size_t len = src->len;
        uint8_t *buf = (uint8_t *)malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = src->bytes[i];
        }
        src_copy = aws_string_new_from_array(aws_default_allocator(), buf, len);
        free(buf);
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        __CPROVER_assert(cursor.ptr == NULL, "cursor.ptr should be NULL when src is NULL");
        __CPROVER_assert(cursor.len == 0, "cursor.len should be 0 when src is NULL");
    } else {
        __CPROVER_assert(cursor.ptr == aws_string_bytes(src), "cursor.ptr should point to src bytes");
        __CPROVER_assert(cursor.len == src->len, "cursor.len should equal src length");
    }

    if (src != NULL) {
        __CPROVER_assert(aws_string_equal(src, src_copy), "src should equal src_copy");
    }

    if (src != NULL) {
        aws_string_destroy(src);
    }
    if (src_copy != NULL) {
        aws_string_destroy(src_copy);
    }
}
