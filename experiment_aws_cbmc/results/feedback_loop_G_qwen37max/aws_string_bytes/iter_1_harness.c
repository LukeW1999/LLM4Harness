#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);
    
    /* Cast away const to initialize the flexible array member struct */
    struct aws_string *mutable_str = (struct aws_string *)str;
    mutable_str->len = len;
    mutable_str->allocator = nondet_bool() ? NULL : aws_default_allocator();
    
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    assert(result == str->bytes);
    assert(result != NULL);
    
    /* Validity invariants */
    assert(aws_string_is_valid(str));
}
