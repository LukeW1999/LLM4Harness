#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

/* Bounding constants – adjust as needed for the verification */
#define MAX_STRING_LEN 256
#define MAX_CURSOR_LEN 256

extern _Bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

int main(void) {
    struct aws_string *src;

    /* Choose between a NULL source and a valid source */
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* Allocate a concrete aws_string with a flexible array on the stack */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } src_container;

        src = &src_container.s;

        /* Initialise the string fields */
        src->allocator = NULL;                     /* allocator is irrelevant for the test */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_STRING_LEN);

        for (size_t i = 0; i < src->len; ++i) {
            src_container.bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Preserve the original state of the source string */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    const uint8_t *old_bytes = NULL;
    if (src) {
        old_allocator = src->allocator;
        old_len = src->len;
        old_bytes = src->bytes;
    }

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Post‑condition checks */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* Ensure the source string was not modified */
    if (src) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
    }

    /* Verify invariants still hold */
    assert(aws_string_is_valid(src));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_CURSOR_LEN));

    return 0;
}
