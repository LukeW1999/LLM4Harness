#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* Non-deterministically choose if str is NULL */
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;
    size_t len;

    if (!is_null) {
        /* Bound the length to avoid state explosion */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate memory for the string header + data + null terminator */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = (struct aws_string *)malloc(alloc_size);

        /* If allocation succeeds, initialize the string */
        if (str != NULL) {
            /* Allocator may be NULL (static string) or a valid allocator */
            str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
            str->len = len;
            /* Ensure the null terminator is set (required for validity) */
            str->bytes[len] = 0;
            /* Assume the string is valid according to the library */
            __CPROVER_assume(aws_string_is_valid(str));
        }
    }

    /* Save old state for postcondition checks */
    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str && str->allocator) ? str->allocator : NULL;
    struct store_byte_from_buffer old_byte;

    /* If the string will NOT be freed (allocator is NULL), save a byte for immutability check */
    if (old_str != NULL && old_allocator == NULL) {
        save_byte_from_array(str->bytes, str->len + 1, &old_byte);
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Postconditions */
    if (old_str != NULL && old_allocator != NULL) {
        /* The string was dynamically allocated and should now be freed */
        assert(!__CPROVER_r_ok(old_str));
    } else if (old_str != NULL) {
        /* The string was not freed (allocator was NULL) â it must remain valid and unchanged */
        assert(__CPROVER_r_ok(old_str));
        assert(aws_string_is_valid(old_str));
        assert_byte_from_buffer_matches(old_str->bytes, &old_byte);
    }
    /* If old_str was NULL, the function did nothing â no further checks needed */
}
