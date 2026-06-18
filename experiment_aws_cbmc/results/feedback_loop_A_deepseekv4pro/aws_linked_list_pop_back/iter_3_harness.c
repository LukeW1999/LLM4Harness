#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Insert a nondeterministic number of nodes (1 .. MAX_NODES) into the list */
    #define MAX_NODES 3
    struct aws_linked_list_node nodes[MAX_NODES];
    size_t count;
    __CPROVER_assume(count > 0 && count <= MAX_NODES);
    for (size_t i = 0; i < count; ++i) {
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    struct aws_linked_list_node *old_tail = list.tail.prev;
    struct aws_linked_list_node *old_tail_prev = (count > 1) ? old_tail->prev : &list.head;

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));
    assert(popped == old_tail);
    assert(list.tail.prev == old_tail_prev);
    assert(old_tail_prev->next == &list.tail);
}
