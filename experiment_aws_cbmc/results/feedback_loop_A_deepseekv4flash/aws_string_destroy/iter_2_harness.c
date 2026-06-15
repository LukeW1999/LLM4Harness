#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Nondeterministically choose length (bounded to avoid large allocations) */
    size_t len;
    __CPROVER_assume(len < 100);

    /* Allocate memory for struct aws_string plus flexible array member bytes */
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + len + 1);
    if (str != NULL) {
        /* Set allocator: either NULL or a valid pointer */
        struct aws_allocator *alloc = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        /* Use memcpy to assign to const fields (allowed because we own the memory) */
        memcpy(&str->allocator, &alloc, sizeof(str->allocator));
        /* Set len using memcpy */
        memcpy(&str->len, &len, sizeof(str->len));
        /* Fill bytes with nondeterministic data and null-terminate */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = '\0';
        /* Assume the string is valid according to aws_string_is_valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Capture whether the string will be freed (non-NULL and non-NULL allocator) */
    bool will_free = (str != NULL) && (str->allocator != NULL);

    /* Call the function under test */
    aws_string_destroy(str);

    /* Postcondition assertions */
    if (str == NULL) {
        /* No memory was allocated, nothing to free; trivially true */
        assert(1 == 1);
    } else {
        if (!will_free) {
            /* allocator was NULL, so string was not freed; it should still be valid */
            assert(aws_string_is_valid(str));
        } else {
            /* allocator was non-NULL, string was freed; accessing str is undefined, so no assertion */
        }
    }
}
