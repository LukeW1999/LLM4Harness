#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness() {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);
    
    uint8_t *pBuf = malloc(bufsize);
    
    /* 
     * aws_secure_zero contains an AWS_ASSERT(bufsize == 0) if pBuf is NULL.
     * We must assume that if the allocation failed (or we passed NULL), 
     * the bufsize is 0 to satisfy the function's internal precondition.
     */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);
    
    aws_secure_zero(pBuf, bufsize);
    
    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; i++) {
            assert(pBuf[i] == 0);
        }
    }
    
    free(pBuf);
}
