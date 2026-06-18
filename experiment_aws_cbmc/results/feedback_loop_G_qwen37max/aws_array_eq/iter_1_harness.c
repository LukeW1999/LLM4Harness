#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
    
    uint8_t *buf_a = malloc(MAX_BUFFER_SIZE > 0 ? MAX_BUFFER_SIZE : 1);
    uint8_t *buf_b = malloc(MAX_BUFFER_SIZE > 0 ? MAX_BUFFER_SIZE : 1);
    __CPROVER_assume(buf_a != NULL);
    __CPROVER_assume(buf_b != NULL);
    
    void *ptr_a = (len_a == 0 && nondet_bool()) ? NULL : (void *)buf_a;
    void *ptr_b = (len_b == 0 && nondet_bool()) ? NULL : (void *)buf_b;
    
    bool result = aws_array_eq(ptr_a, len_a, ptr_b, len_b);
    
    if (len_a != len_b) {
        assert(result == false);
    } else if (len_a == 0) {
        assert(result == true);
    }
}
