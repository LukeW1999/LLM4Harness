#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src;

    if (nondet_bool()) {
        src = NULL;
    } else {
        uint64_t len64 = nondet_uint64_t();
        __CPROVER_assume(len64 < MAX_STRING_LEN);
        size_t len = (size_t)len64;

        _Alignas(struct aws_string) uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
        src = (struct aws_string *)storage;

        struct aws_allocator *allocator = aws_default_allocator();
        src->allocator = allocator;
        src->len = len;

        /* Null‑terminate the string bytes */
        ((uint8_t *)src)[sizeof(struct aws_string) + len] = 0;

        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;

    if (src != NULL) {
        old_allocator = src->allocator;
        old_len = src->len;
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    if (src != NULL) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert(aws_string_is_valid(src));
    }
}
