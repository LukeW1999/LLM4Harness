#include <aws/common/array_list.h>
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

    /* 2. Save old length */
    size_t old_length = list.length;

    /* 3. Call the function under test */
    size_t result = aws_array_list_length(&list);

    /* 4. Assert postconditions */
    /* The returned length must equal the list's length field */
    assert(result == old_length);

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* The length field must not have changed */
    assert(list.length == old_length);

    /* The result must be within valid bounds */
    assert(result <= list.current_size / (list.item_size == 0 ? 1 : list.item_size) || list.item_size == 0);
}
