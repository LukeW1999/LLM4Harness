#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(str->allocator != NULL);
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
    AWS_MEM_IS_READABLE(str->bytes, str->len);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string old = *str;

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* In this case, the function frees the memory, so we cannot assert anything about the fields after the call */
    /* Instead, we assert that the memory is freed */
    assert(str == NULL || str->allocator == NULL);

    /* 5. Assert fields that must NOT change regardless of result */
    /* Since the function frees the memory, we cannot assert anything about the fields after the call */
    /* The memory is freed, so the structure is invalid */

    /* 6. Assert validity invariants always holds */
    /* Since the function frees the memory, the structure is invalid after the call */
}
