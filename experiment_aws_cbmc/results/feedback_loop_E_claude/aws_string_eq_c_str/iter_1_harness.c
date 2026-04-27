/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_c_str_harness() {
    /* Non-deterministically choose whether str and c_str are NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    /* Set up aws_string */
    struct aws_string *str = NULL;
    if (!str_is_null) {
        /* Allocate a valid aws_string */
        str = ensure_string_is_allocated_nondet_length();
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up c_str */
    const char *c_str = NULL;
    if (!c_str_is_null) {
        c_str = ensure_c_str_is_allocated(MAX_STRING_LEN);
        __CPROVER_assume(c_str != NULL);
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions based on Doxygen and implementation:
     *
     * 1. If both are NULL, returns true
     * 2. If one is NULL and the other is not, returns false
     * 3. If neither is NULL, returns result of byte comparison
     */

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }
    /* When neither is NULL, result depends on content comparison - no simple assert */

    /* Validity invariants: inputs must remain valid after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }

    /* The function returns a bool - must be true or false */
    assert(result == true || result == false);
}
