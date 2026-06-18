#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_BUFFER_SIZE);
    
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);
    
    str->allocator = aws_default_allocator();
    str->len = len;
    
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
}
