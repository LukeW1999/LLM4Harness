#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    /* data structures */
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* initialize linked lists */
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Add elements to the lists to ensure they are not empty */
    struct aws_linked_list_node node1;
    struct aws_linked_list_node node2;
    __CPROVER_assume(&node1 != NULL);
    __CPROVER_assume(&node2 != NULL);
    aws_linked_list_push_back(&a, &node1);
    aws_linked_list_push_back(&b, &node2);

    /* save old states */
    struct aws_linked_list old_a = a;
    struct aws_linked_list old_b = b;

    /* perform operation under verification */
    aws_linked_list_swap_contents(&a, &b);

    /* assertions */
    if (!aws_linked_list_empty(&old_b)) {
        assert(a.head.next == old_b.head.next);
        assert(a.tail.prev == old_b.tail.prev);
    } else {
        assert(aws_linked_list_empty(&a));
    }

    if (!aws_linked_list_empty(&old_a)) {
        assert(b.head.next == old_a.head.next);
        assert(b.tail.prev == old_a.tail.prev);
    } else {
        assert(aws_linked_list_empty(&b));
    }

    /* unchanged fields */
    assert(a.head.prev == &a.head);
    assert(a.tail.next == &a.tail);
    assert(b.head.prev == &b.head);
    assert(b.tail.next == &b.tail);

    /* validity invariants */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
