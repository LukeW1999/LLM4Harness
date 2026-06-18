#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *after = &list.head;
    struct aws_linked_list_node *next_node = list.head.next;
    
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    __CPROVER_assume(after != to_add);
    __CPROVER_assume(next_node != to_add);
    __CPROVER_assume(after != next_node);

    struct aws_linked_list_node *old_after_next = after->next;

    aws_linked_list_insert_after(after, to_add);

    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);
}
