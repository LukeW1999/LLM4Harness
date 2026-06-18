#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    /* 1. Declare and initialize the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save the expected length */
    size_t expected_length = list.length;

    /* 3. Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* 4. Assert postconditions */
    /* The returned length must equal the list's length field */
    assert(result == expected_length);

    /* The list must still be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* The list fields must not have been modified */
    assert(list.length == expected_length);
}
