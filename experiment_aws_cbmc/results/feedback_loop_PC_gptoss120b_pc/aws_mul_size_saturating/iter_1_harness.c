#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* Allocate a buffer that can be used for the pointer argument */
    size_t max_alloc = MAX_BUFFER_SIZE;
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= max_alloc);

    void *alloc_buf = NULL;
    if (alloc_size > 0) {
        alloc_buf = malloc(alloc_size);
        __CPROVER_assume(alloc_buf != NULL);
        /* Fill the allocated buffer with nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)alloc_buf;
        for (size_t i = 0; i < alloc_size; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Save a copy of the original contents for later comparison */
    uint8_t *old = NULL;
    if (alloc_size > 0) {
        old = malloc(alloc_size);
        __CPROVER_assume(old != NULL);
        for (size_t i = 0; i < alloc_size; ++i) {
            old[i] = ((uint8_t *)alloc_buf)[i];
        }
    }

    /* Choose whether the pointer argument is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    size_t bufsize = nondet_size_t();
    size_t offset = 0; /* valid only when pBuf != NULL */

    if (ptr_is_null) {
        pBuf = NULL;
        /* bufsize is nondet; the function will assert that it is zero */
    } else {
        /* Ensure we have an allocated buffer to point into */
        __CPROVER_assume(alloc_buf != NULL);
        /* Choose a nondeterministic offset within the allocated region */
        offset = nondet_size_t();
        __CPROVER_assume(offset <= alloc_size);
        pBuf = (uint8_t *)alloc_buf + offset;
        /* Constrain bufsize so that the range [offset, offset+bufsize) stays inside the allocation */
        __CPROVER_assume(offset + bufsize <= alloc_size);
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑condition checks */

    if (pBuf == NULL) {
        /* When the pointer is NULL the function asserts bufsize == 0 */
        assert(bufsize == 0);
        /* No memory should have been modified */
        if (alloc_size > 0) {
            for (size_t i = 0; i < alloc_size; ++i) {
                assert(((uint8_t *)alloc_buf)[i] == old[i]);
            }
        }
    } else {
        if (bufsize == 0) {
            /* No bytes should be modified */
            if (alloc_size > 0) {
                for (size_t i = 0; i < alloc_size; ++i) {
                    assert(((uint8_t *)alloc_buf)[i] == old[i]);
                }
            }
        } else {
            /* Bytes in the range [offset, offset+bufsize) must be zero */
            uint8_t *p = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                assert(p[i] == 0);
            }
            /* Bytes outside that range must remain unchanged */
            for (size_t i = 0; i < alloc_size; ++i) {
                if (i < offset || i >= offset + bufsize) {
                    assert(((uint8_t *)alloc_buf)[i] == old[i]);
                }
            }
        }
    }

    /* Clean up */
    free(old);
    free(alloc_buf);
}
