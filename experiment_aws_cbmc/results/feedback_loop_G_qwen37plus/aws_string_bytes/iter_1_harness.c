#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    size_t extra_bytes = nondet_size_t();
    __CPROVER_assume(extra_bytes < MAX_BUFFER_SIZE);
    
    struct aws_string *str = malloc(sizeof(struct aws_string) + extra_bytes);
    __CPROVER_assume(str != NULL);
    
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);
    
    assert(result == str->bytes);
}
