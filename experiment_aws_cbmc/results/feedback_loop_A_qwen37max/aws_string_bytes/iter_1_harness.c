#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    size_t alloc_size = sizeof(struct aws_string) + len;
    struct aws_string *str = malloc(alloc_size);
    __CPROVER_assume(str != NULL);
    
    /* Bypass const fields for initialization */
    struct aws_string_mut {
        struct aws_allocator *allocator;
        size_t len;
        uint8_t bytes[1];
    };
    struct aws_string_mut *mut_str = (struct aws_string_mut *)str;
    mut_str->allocator = aws_default_allocator();
    mut_str->len = len;
    
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* Save old state to check immutability */
    size_t old_len = str->len;
    struct aws_allocator *old_allocator = str->allocator;
    
    const uint8_t *result = aws_string_bytes(str);
    
    /* Postconditions */
    assert(result == str->bytes);
    
    /* Immutability checks */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);
    
    /* Validity invariants */
    assert(aws_string_is_valid(str));
}
