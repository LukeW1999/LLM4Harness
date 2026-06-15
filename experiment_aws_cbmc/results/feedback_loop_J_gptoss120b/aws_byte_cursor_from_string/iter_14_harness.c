#include <assert.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    const struct aws_string *src;
    const size_t MAX_STRING_LEN = 256;

    /* Choose between a NULL source or a valid string */
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Stack‑allocated buffer that holds the aws_string struct followed by its bytes */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } buf;

        struct aws_allocator *allocator = aws_default_allocator();
        buf.s.allocator = allocator;
        buf.s.len = len;

        for (size_t i = 0; i < len; ++i) {
            buf.bytes[i] = nondet_uint8_t();
        }

        src = &buf.s;
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Preserve the old state of the source string */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_bytes[256];
    bool have_old_bytes = false;

    if (src) {
        old_allocator = src->allocator;
        old_len = src->len;
        if (src->len > 0) {
            for (size_t i = 0; i < src->len; ++i) {
                old_bytes[i] = aws_string_bytes(src)[i];
            }
            have_old_bytes = true;
        }
    }

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Verify post‑conditions */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        if (have_old_bytes) {
            for (size_t i = 0; i < src->len; ++i) {
                assert(aws_string_bytes(src)[i] == old_bytes[i]);
            }
        }
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, src->len));
    }
}
