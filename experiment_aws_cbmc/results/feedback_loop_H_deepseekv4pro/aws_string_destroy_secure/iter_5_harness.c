#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

#define MAX_LEN 1024

void aws_string_destroy_secure_harness() {
    /* NULL case */
    aws_string_destroy_secure(NULL);

    /* Non-NULL case: allocate a string with nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = ensure_aws_string_is_allocated(allocator, len);

    /* Call the function under test */
    aws_string_destroy_secure(str);
}
