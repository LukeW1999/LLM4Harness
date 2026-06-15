#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    /* 1. Allocate and nondeterministically initialize an aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    bool has_allocator = nondet_bool();
    str->allocator = has_allocator ? aws_default_allocator() : NULL;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* Assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_string old = *str;
    uint8_t *old_bytes = malloc(len);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = str->bytes[i];
    }

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Assert unchanged fields (the function does not modify any fields) */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    if (len > 0) {
        assert_bytes_match(str->bytes, old_bytes, len);
    }

    /* 5. No validity invariant is required after destroy because the memory may be freed */

    free(old_bytes);
}
