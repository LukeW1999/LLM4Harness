#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    struct aws_linked_list_node *before = &list.tail;
    struct aws_linked_list_node *curr = list.head.next;
    if (curr != &list.tail) {
        if (nondet_bool()) {
            before = curr;
        }
    }

    struct aws_linked_list_node *old_before_prev = before->prev;

    aws_linked_list_insert_before(before, to_add);

    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(old_before_prev->next == to_add);
    assert(before->prev == to_add);
}
