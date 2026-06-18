#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

void aws_secure_zero_harness() {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *pBuf = nondet_bool() ? NULL : malloc(bufsize == 0 ? 1 : bufsize);
    
    /* Precondition derived from AWS_ASSERT in the implementation */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    aws_secure_zero(pBuf, bufsize);

    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; i++) {
            assert(pBuf[i] == 0);
        }
    }
}
