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

struct aws_string_harness_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_byte_cursor_from_string_harness(void) {
    if (nondet_bool()) {
        const struct aws_string *src = NULL;

        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        assert(cursor.len == 0);
        assert(cursor.ptr == NULL);
        assert(aws_byte_cursor_is_valid(&cursor));
    } else {
        struct aws_string_harness_storage *storage = malloc(sizeof(*storage));
        __CPROVER_assume(storage != NULL);

        storage->allocator = NULL;
        storage->len = nondet_size_t();
        __CPROVER_assume(storage->len <= MAX_BUFFER_SIZE);

        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            storage->bytes[i] = nondet_uint8_t();
        }
        storage->bytes[storage->len] = 0;

        const struct aws_string *src = (const struct aws_string *)storage;
        __CPROVER_assume(aws_string_is_valid(src));

        const struct aws_string *old_src = src;
        struct aws_allocator *old_allocator = src->allocator;
        size_t old_len = src->len;
        const uint8_t *old_bytes = src->bytes;

        struct store_byte_from_buffer old_byte;
        save_byte_from_array(src->bytes, src->len + 1, &old_byte);

        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        assert(src == old_src);
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
        assert_byte_from_buffer_matches(src->bytes, &old_byte);

        assert(cursor.len == old_len);
        assert(cursor.ptr == old_bytes);
        assert_bytes_match(cursor.ptr, old_bytes, cursor.len);

        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_valid(&cursor));
    }
}
