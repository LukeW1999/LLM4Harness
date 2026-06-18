#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/secure_zero.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_secure_zero_harness(void) {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool allocate = nondet_bool();
    uint8_t *pBuf = NULL;

    if (allocate) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
    } else {
        pBuf = NULL;
        __CPROVER_assume(bufsize == 0);
    }

    uint8_t *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    aws_secure_zero(pBuf, bufsize);

    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        if (pBuf == NULL) {
            assert(bufsize == 0);
        }
    }
}
