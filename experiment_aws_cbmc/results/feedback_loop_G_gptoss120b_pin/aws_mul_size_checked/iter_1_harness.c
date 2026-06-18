#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* nondeterministic pointer: either NULL or a malloc'ed buffer */
    void *p = NULL;
    if (nondet_bool()) {
        p = malloc(size);
        __CPROVER_assume(p != NULL);
        /* initialize buffer with nondeterministic data */
        unsigned char *bytes = (unsigned char *)p;
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* The function asserts that if p is NULL then size must be 0 */
    __CPROVER_assume(!(p == NULL && size != 0));

    /* Save old state */
    void *old_p = p;
    size_t old_size = size;

    /* Call the function under test */
    aws_secure_zero(p, size);

    /* Unchanged fields */
    assert(p == old_p);
    assert(size == old_size);

    /* Postcondition: if buffer was non‑NULL and size > 0, all bytes are zero */
    if (p != NULL && size != 0) {
        unsigned char *bytes = (unsigned char *)p;
        for (size_t i = 0; i < size; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* If p is NULL, size must be 0 (already ensured by the assume) */
    if (p == NULL) {
        assert(size == 0);
    }
}
