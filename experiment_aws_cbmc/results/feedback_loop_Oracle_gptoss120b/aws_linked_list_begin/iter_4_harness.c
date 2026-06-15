#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_begin_harness(void) {
    /* Allocate an allocator (default) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a linked list and populate it with a nondeterministic number of nodes */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    uint32_t max_nodes = nondet_uint();
    __CPROVER_assume(max_nodes <= 5);               /* bound the size for tractability */
    make_linked_list(&list, allocator, max_nodes);  /* builds a structurally valid list */

    /* Save a copy of the list to check frame conditions later */
    struct aws_linked_list old = list;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* --------------------------------------------------------------------
     * Post‑condition checks
     * -------------------------------------------------------------------- */

    /* 1. Return value must be the first element (head.next) */
    assert(result == list.head.next);

    /* 2. Consistency with emptiness of the list */
    if (aws_linked_list_empty(&list)) {
        /* For an empty list begin should point to the tail sentinel */
        assert(result == &list.tail);
    } else {
        /* For a non‑empty list begin must not be the tail sentinel */
        assert(result != &list.tail);
        /* The returned node must be linked correctly with the head */
        assert(result->prev == &list.head);
        /* The node's forward link must be bidirectional (valid) */
        assert(aws_linked_list_node_next_is_valid(result));
    }

    /* 3. Frame condition – the list structure must be unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
}
