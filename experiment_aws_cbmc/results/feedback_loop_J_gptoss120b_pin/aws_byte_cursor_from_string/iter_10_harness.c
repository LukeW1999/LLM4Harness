#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t mem[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        src = (struct aws_string *)mem;

        src->allocator = aws_default_allocator();
        src->len = len;
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (src) {
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

    if (src) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
    }
}
