/*@
Preconditions:
- `list` points to a properly allocated `struct aws_linked_list`.
- `aws_linked_list_is_valid(list)` holds (the list satisfies all internal
  consistency constraints).
- The list may be empty or contain an arbitrary (but finite) number of
  nodes.

Postconditions (validity):
- The returned pointer `rval` is exactly `list->tail.prev`.
- If the list is empty, `rval == &list->head`.
- If the list is non‑empty, `rval` points to the last user node in the list
  (its `next` field is `&list->tail`).

Postconditions (frame):
- The `head` and `tail` nodes of `list` are unchanged by the call.
- No memory outside of the function’s local variables is modified.
@*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_rbegin_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* nondet decide whether the list is empty */
    __CPROVER_bool empty = nondet_bool();

    if (!empty) {
        /* create at least one node */
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);

        /* optionally add a few more nodes (bounded to keep the proof tractable) */
        size_t extra = nondet_uint();
        __CPROVER_assume(extra < 5);
        for (size_t i = 0; i < extra; ++i) {
            struct aws_linked_list_node *n = malloc(sizeof(*n));
            __CPROVER_assume(n != NULL);
            aws_linked_list_node_reset(n);
            aws_linked_list_push_back(&list, n);
        }
    }

    /* ensure the list satisfies the library’s internal invariants */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* capture the pre‑state of the list structure for frame checking */
    struct aws_linked_list old = list;

    /* call the function under test */
    struct aws_linked_list_node *rval = aws_linked_list_rbegin(&list);

    /* post‑condition: return value matches the tail's predecessor */
    assert(rval == list.tail.prev);

    if (aws_linked_list_empty(&list)) {
        /* empty list: rbegin should point to the head sentinel */
        assert(rval == &list.head);
    } else {
        /* non‑empty list: rbegin points to a real node */
        assert(rval != &list.head);
        assert(rval->next == &list.tail);
    }

    /* frame condition: the list’s sentinel nodes are unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    return 0;
}
