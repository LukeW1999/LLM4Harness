#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "aws/common/byte_buf.h"
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_eq_harness(void) {
    /* nondeterministic lengths, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* nondeterministic buffers */
    const void *array_a = NULL;
    const void *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }
    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* preconditions from the implementation */
    __CPROVER_assume((len_a == 0) || AWS_MEM_IS_READABLE(array_a, len_a));
    __CPROVER_assume((len_b == 0) || AWS_MEM_IS_READABLE(array_b, len_b));

    /* call the function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* expected result according to the specification */
    bool expected;
    if (len_a != len_b) {
        expected = false;
    } else if (len_a == 0) {
        expected = true;
    } else {
        expected = (memcmp(array_a, array_b, len_a) == 0);
    }

    /* postcondition */
    assert(result == expected);
}
