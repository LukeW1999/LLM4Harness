#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_cursor_from_string_harness() {
    /* nondeterministic choice: src can be NULL or a valid string */
    struct aws_string *src = NULL;
    bool src_is_null = nondet_bool();
    if (src_is_null) {
        src = NULL;
    } else {
        /* allocate memory for the string header plus the bytes */
        size_t len;
        __CPROVER_assume(len < MAX_STRING_LEN);
        src = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);
        /* set the fields */
        src->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        src->len = len;
        /* fill bytes with nondet values */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }
        /* ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* call the function */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* postconditions */
    if (src == NULL) {
        /* cursor should be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* cursor should point to the string's bytes and have the same length */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        /* cursor must be valid */
        assert(aws_byte_cursor_is_valid(&cursor));
        /* the input string must remain valid */
        assert(aws_string_is_valid(src));
    }
}
