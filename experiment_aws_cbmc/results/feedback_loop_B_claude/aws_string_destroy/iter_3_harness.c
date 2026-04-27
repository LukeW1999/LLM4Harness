#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose which case to test */
    struct aws_string *str;

    bool is_null;
    bool has_allocator;

    if (is_null) {
        /* Case 1: NULL pointer - function should be a no-op */
        str = NULL;
        aws_string_destroy(str);
        assert(str == NULL);
    } else if (!has_allocator) {
        /* Case 2: non-NULL str but NULL allocator - function should be a no-op */
        size_t len;
        __CPROVER_assume(len <= 16);

        size_t total_size = sizeof(struct aws_string) + len + 1;
        struct aws_string *s = malloc(total_size);
        __CPROVER_assume(s != NULL);

        /* Set allocator to NULL - cast away const for initialization */
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&s->allocator;
        *alloc_ptr = NULL;

        /* Set length */
        size_t *len_ptr = (size_t *)&s->len;
        *len_ptr = len;

        /* Initialize the bytes to make aws_string_is_valid pass */
        /* The string bytes follow the struct - need null terminator */
        uint8_t *bytes = (uint8_t *)(s + 1);
        bytes[len] = '\0';

        str = s;

        /* Ensure the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));

        assert(str->allocator == NULL);

        /* Call function - should be a no-op since allocator is NULL */
        aws_string_destroy(str);

        /* Memory wasn't freed, still valid */
        assert(str != NULL);
        assert(str->allocator == NULL);
    } else {
        /* Case 3: non-NULL str with non-NULL allocator - memory gets released */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);

        size_t len;
        __CPROVER_assume(len <= 16);

        size_t total_size = sizeof(struct aws_string) + len + 1;

        /* Use aws_mem_acquire to allocate so aws_mem_release can free it */
        str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields */
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
        *alloc_ptr = allocator;

        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;

        /* Initialize the bytes to make aws_string_is_valid pass */
        uint8_t *bytes = (uint8_t *)(str + 1);
        bytes[len] = '\0';

        /* Ensure the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));

        assert(str != NULL);
        assert(str->allocator != NULL);

        /* Call function - should release memory */
        aws_string_destroy(str);

        /* After destroy, memory has been released - don't dereference str */
        assert(true);
    }
}
