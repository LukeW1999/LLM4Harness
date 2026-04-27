#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_destroy_harness() {
    /* parameters */
    struct aws_string *str;
    struct aws_allocator *allocator;

    /* assumptions */
    allocator = can_fail_allocator();
    str = bounded_malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    str->allocator = allocator;
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_STRING_LEN);
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1));
    str->bytes[str->len] = 0; // Ensure null termination

    /* operation under verification */
    aws_string_destroy(str);

    /* assertions */
    // Check that the memory was released if allocator is not null
    if (allocator) {
        assert(!AWS_MEM_IS_READABLE(str, sizeof(struct aws_string) + MAX_STRING_LEN));
    } else {
        // If allocator is null, the memory should not be released
        assert(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string) + MAX_STRING_LEN));
    }

    // Check that the string pointer is now invalid
    assert(str == NULL);
}
