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
    
    void *pBuf = nondet_bool() ? NULL : malloc(bufsize);
    
    /* Precondition: if pBuf is NULL, bufsize must be 0 to avoid AWS_ASSERT failure */
    if (pBuf == NULL) {
        __CPROVER_assume(bufsize == 0);
    }
    
    aws_secure_zero(pBuf, bufsize);
    
    /* Postcondition: all bytes in the buffer are zeroed */
    if (pBuf != NULL && bufsize > 0) {
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; i++) {
            assert(bytes[i] == 0);
        }
    }
    
    if (pBuf != NULL) {
        free(pBuf);
    }
}
