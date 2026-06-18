#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness() {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);
    
    uint8_t *pBuf = malloc(bufsize);
    if (pBuf == NULL) {
        __CPROVER_assume(bufsize == 0);
    }
    
    if (pBuf != NULL) {
        for (size_t i = 0; i < bufsize; i++) {
            pBuf[i] = nondet_uint8_t();
        }
    }

    aws_secure_zero(pBuf, bufsize);

    if (pBuf != NULL) {
        for (size_t i = 0; i < bufsize; i++) {
            assert(pBuf[i] == 0);
        }
    }
    
    free(pBuf);
}
