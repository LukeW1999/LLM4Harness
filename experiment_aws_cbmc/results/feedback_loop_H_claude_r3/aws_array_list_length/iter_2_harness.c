#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old length */
    size_t old_length = list.length;

    /* 3. Call function under test */
    size_t result = aws_array_list_length(&list);

    /* 4. Assert postconditions */
    assert(aws_array_list_is_valid(&list));
    assert(result == old_length);
    assert(result == list.length);
}
