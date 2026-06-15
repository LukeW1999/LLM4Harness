#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether str is NULL or a valid string */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    if (!str_is_null) {
        /* Allocate memory for the string header plus the bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);
        size_t total_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);
        /* Initialize fields */
        *(size_t *)&str->len = len;
        bool allocator_is_null = nondet_bool();
        if (allocator_is_null) {
            *(struct aws_allocator **)&str->allocator = NULL;
        } else {
            *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        }
        /* Fill bytes with non-deterministic data */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    }

    /* Save data before calling destroy */
    uint8_t *old_bytes = NULL;
    size_t old_len = 0;
    if (str != NULL && str->allocator == NULL) {
        old_len = str->len;
        old_bytes = (uint8_t *)malloc(old_len);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, aws_string_bytes(str), old_len);
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions */
    if (str == NULL) {
        /* Nothing happens when str is NULL */
        assert(str == NULL);
    } else if (str->allocator == NULL) {
        /* The string was not freed; bytes must be zeroed */
        assert(old_bytes != NULL);
        assert(str->len == old_len);
        assert(str->allocator == NULL);
        for (size_t i = 0; i < old_len; i++) {
            assert(aws_string_bytes(str)[i] == 0);
        }
    } else {
        /* Memory was freed; cannot access string, but check that function ran without error */
        assert(1);
    }
}
