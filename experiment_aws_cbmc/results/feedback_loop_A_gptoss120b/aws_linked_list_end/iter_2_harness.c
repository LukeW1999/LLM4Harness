#include <assert.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_end_harness() {
    /* Initialize an empty linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Optionally add a nondeterministic number of nodes (bounded) */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        /* The node does not need to contain payload for this harness */
        aws_linked_list_push_back(&list, node);
    }

    /* Save the old state of the list */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* Postconditions */
    assert(result == &list.tail);
    assert(result == &old.tail);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* The list must still be valid */
    assert(aws_linked_list_is_valid(&list));
}
