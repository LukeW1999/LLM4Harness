#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int main(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    (void)allocator; /* allocator not used directly in this harness */

    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* optionally add a node to make the list non‑empty */
    if (nondet_bool()) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        node->next = NULL;
        node->prev = NULL;
        aws_linked_list_push_back(&list, node);
    }

    struct aws_linked_list_node *begin = aws_linked_list_begin(&list);

    /* post‑conditions */
    assert(begin == list.head.next);

    if (list.head.next != &list.head) {
        /* non‑empty list */
        assert(begin != &list.head);
        assert(begin->next != NULL);
        assert(begin->prev != NULL);
    } else {
        /* empty list returns the sentinel node */
        assert(begin == &list.head);
    }

    /* clean up */
    if (list.head.next != &list.head) {
        struct aws_linked_list_node *node = list.head.next;
        aws_linked_list_remove(node);
        free(node);
    }

    return 0;
}
