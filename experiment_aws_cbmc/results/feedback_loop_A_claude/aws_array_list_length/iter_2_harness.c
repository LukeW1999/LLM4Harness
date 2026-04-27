#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_length_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;

    /* 3. Call function under test */
    size_t length;
    aws_array_list_length(&list, &length);

    /* 4. Assert postconditions */

    /* The returned length must equal list.length */
    assert(length == old_length);

    /* The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* list.length must not have changed */
    assert(list.length == old_length);
}
