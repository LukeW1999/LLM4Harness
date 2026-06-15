#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    struct aws_linked_list_node *back = aws_linked_list_pop_back(&list);

    /* Postconditions from implementation */
    assert(back != NULL);
    assert(back->next == NULL);
    assert(back->prev == NULL);
    assert(aws_linked_list_is_valid(&list));
}
