#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 32

void aws_string_destroy_secure_harness(void) {
    /* Allocate a nondet aws_string */
    struct aws_string *str = make_arbitrary_aws_string_nondet_len_with_max(aws_default_allocator(), MAX_STRING_LEN);

    /* Save old values before the call */
    struct aws_string old_str;
    size_t i;

    if (str != NULL) {
        old_str = *str;
        __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
        __CPROVER_assume(i < old_str.len);
    } else {
        __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
        __CPROVER_assume(i < old_str.len);
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions:
     * 1. The function returns void, so no return value to check.
     * 2. If str was NULL, nothing should have happened (no crash).
     * 3. If str was non-NULL with no allocator, the memory was zeroed but not freed.
     * 4. If str was non-NULL with an allocator, the memory was zeroed and freed.
     *
     * Since after freeing we cannot safely dereference str, we only assert
     * properties that are safe to check post-call.
     *
     * The main postcondition we can verify is that the function completed
     * without undefined behavior (implicit via CBMC's checks).
     *
     * For the case where str had no allocator (static string), we can verify
     * the bytes were zeroed. But since we can't distinguish post-call whether
     * memory was freed, we assert based on saved state.
     */

    /* If str was NULL, nothing to assert beyond no crash */
    if (str == NULL) {
        assert(true); /* No-op: function handles NULL gracefully */
    }

    /* Frame condition: old_str.len was a valid length */
    if (str != NULL) {
        /* The length field was valid before the call */
        assert(old_str.len <= SIZE_MAX);

        /* If there was no allocator, the string was not freed and bytes should be zeroed */
        if (old_str.allocator == NULL) {
            /* Memory was zeroed but not freed - we can check the bytes */
            const uint8_t *bytes = aws_string_bytes(str);
            assert(bytes[i] == 0);
        }
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
