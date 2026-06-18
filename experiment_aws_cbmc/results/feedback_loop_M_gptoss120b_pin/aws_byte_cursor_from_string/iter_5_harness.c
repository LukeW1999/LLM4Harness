#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();

    struct aws_string *src = NULL;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes = {0};

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate a struct with enough space for the flexible array member */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_BUFFER_SIZE];
        } src_container;

        src = &src_container.s;

        src->allocator = aws_default_allocator();
        src->len = len;

        for (size_t i = 0; i < len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(src));

        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
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
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
        assert(aws_string_is_valid(src));
    }
}
