#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_clean_up_harness() {
    /* nondeterministic list with bounded size and allocated data */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* call the function under test */
    aws_array_list_clean_up(&list);

    /* the entire structure is zeroed */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* no memory errors occur (checked implicitly by CBMC) */
}
