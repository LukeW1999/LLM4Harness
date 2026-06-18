#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 8

void aws_array_list_length_harness() {
    struct aws_array_list list;
    /* bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    /* allocate the data buffer non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);
    /* assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t length = aws_array_list_length(&list);
    /* length should equal list.length */
    assert(length == list.length);
    /* list should remain valid */
    assert(aws_array_list_is_valid(&list));
}
