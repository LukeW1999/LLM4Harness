#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

/* Function under test */
int aws_add_size_saturating(size_t a, size_t b, size_t *result);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t *out = malloc(sizeof(size_t));
    __CPROVER_assume(out != NULL);

    int rc = aws_add_size_saturating(a, b, out);

    if (a > SIZE_MAX - b) {
        assert(rc != 0);
        assert(*out == SIZE_MAX);
    } else {
        assert(rc == 0);
        assert(*out == a + b);
    }

    free(out);
}
