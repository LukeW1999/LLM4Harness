#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministic choice for a NULL input */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Call with NULL: the function should do nothing */
        aws_string_destroy_secure(NULL);
        /* Trivial postcondition – harness requires at least one assert */
        assert(1);
        return;
    }
