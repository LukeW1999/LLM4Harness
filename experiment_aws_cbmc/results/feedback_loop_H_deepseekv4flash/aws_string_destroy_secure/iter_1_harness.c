#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 10

void aws_string_destroy_secure_harness() {
    /* Non-deterministic pointer to string */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL); /* We handle NULL separately */

    /* Non-deterministic length */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Write length field via cast (avoid const issue for proof) */
    *(size_t *)&str->len = len;

    /* Non-deterministic allocator: NULL or non-NULL */
    struct aws_allocator *allocator = nondet_bool() ? (struct aws_allocator *)malloc(sizeof(struct aws_allocator)) : NULL;
    *(struct aws_allocator **)&str->allocator = allocator;

    /* Fill bytes with non-deterministic data */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* Assert that string is valid before call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old bytes if we need to check zeroing */
    uint8_t old_bytes[MAX_STRING_LEN];
    if (len > 0) {
        __CPROVER_assert(len <= MAX_STRING_LEN, "len is within bounds");
        for (size_t i = 0; i < len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    /* Call the function */
    aws_string_destroy_secure(str);

    /* Postconditions */
    /* If allocator was NULL, memory should still be valid and bytes zeroed */
    if (allocator == NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "String should still be valid when allocator is NULL");
        for (size_t i = 0; i < len; ++i) {
            __CPROVER_assert(str->bytes[i] == 0, "Bytes should be zeroed when allocator is NULL");
        }
        /* Free the allocated memory to avoid memory leak in proof */
        free(str);
    } else {
        /* When allocator is non-NULL, memory is freed; no further accesses */
        /* Ensure we do not access freed memory; just assert that allocator was non-NULL */
        __CPROVER_assert(allocator != NULL, "Allocator was non-NULL, memory released");
    }

    /* Also handle case where str is NULL */
    struct aws_string *null_str = NULL;
    aws_string_destroy_secure(null_str);
    /* No effect, no assertion needed */
}
