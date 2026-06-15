#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>

/* Bounding constants – adjust as needed for the verification */
#define MAX_STRING_LEN 256
#define MAX_CURSOR_LEN 256

int main(void) {
    /* 1. Declare and nondeterministically allocate the source string */
    struct aws_string *src;

    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate enough space for the struct plus a flexible array of bytes */
        src = malloc(sizeof(struct aws_string) + MAX_STRING_LEN - 1);
        __CPROVER_assume(src != NULL);

        /* allocator may be NULL (static string) or a nondeterministic pointer */
        src->allocator = nondet_bool() ? NULL : malloc(1);
        __CPROVER_assume(src->allocator != NULL || src->allocator == NULL);

        /* length is bounded */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_STRING_LEN);

        /* fill the byte array with nondeterministic data – the memory is already
         * allocated as part of the flexible array member */
        for (size_t i = 0; i < src->len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of the source string (if any) */
    struct aws_string old_src = {0};
    if (src) {
        old_src = *src;
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks for both branches */
    if (src == NULL) {
        /* When the input is NULL the function returns a zero‑initialised cursor */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When the input is a valid string the cursor must point to the string's
         * byte array and have the same length as the string */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. Fields of the source string must remain unchanged */
    if (src) {
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        /* The flexible‑array pointer itself is immutable; its address must stay the same */
        assert(src->bytes == old_src.bytes);
    }

    /* 6. Validity invariants must still hold */
    assert(aws_string_is_valid(src));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_CURSOR_LEN));

    return 0;
}
