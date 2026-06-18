#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Insert a nondeterministic number of nodes (0 .. MAX_NODES) into the list */
    #define MAX_NODES 3
    struct aws_linked_list_node nodes[MAX_NODES];
    size_t count;
    __CPROVER_assume(count <= MAX_NODES);
    for (size_t i = 0; i < count; ++i) {
        aws_linked_list_push_back(&list, &nodes[i]);
    }

    bool was_empty = aws_linked_list_empty(&list);
    struct aws_linked_list_node *old_tail = was_empty ? NULL : list.tail.prev;
    struct aws_linked_list_node *old_tail_prev = (count > 1) ? old_tail->prev : &list.head;

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    if (was_empty) {
        assert(popped == NULL);
        assert(aws_linked_list_empty(&list));
    } else {
        assert(aws_linked_list_is_valid(&list));
        assert(popped == old_tail);
        assert(list.tail.prev == old_tail_prev);
        assert(old_tail_prev->next == &list.tail);
    }
}
