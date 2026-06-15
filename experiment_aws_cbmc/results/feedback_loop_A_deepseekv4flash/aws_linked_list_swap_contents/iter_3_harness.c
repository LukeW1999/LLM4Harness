#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;
    struct aws_linked_list_node n1, n2;

    // Nondeterministically set up list a (empty or with one node)
    bool a_has_node;
    __CPROVER_assume(a_has_node == nondet_bool());
    if (a_has_node) {
        // Initialize node n1 as the only element in list a
        n1.next = &a.head;
        n1.prev = &a.head;
        a.head.next = &n1;
        a.head.prev = &n1;
    } else {
        a.head.next = &a.head;
        a.head.prev = &a.head;
    }

    // Nondeterministically set up list b (empty or with one node)
    bool b_has_node;
    __CPROVER_assume(b_has_node == nondet_bool());
    if (b_has_node) {
        n2.next = &b.head;
        n2.prev = &b.head;
        b.head.next = &n2;
        b.head.prev = &n2;
    } else {
        b.head.next = &b.head;
        b.head.prev = &b.head;
    }

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a != &b);

    // Save original first nodes (or sentinel if empty)
    struct aws_linked_list_node *a_orig = a.head.next;
    struct aws_linked_list_node *b_orig = b.head.next;

    aws_linked_list_swap_contents(&a, &b);

    // Postconditions
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(a.head.next == b_orig);
    assert(b.head.next == a_orig);
    assert(aws_linked_list_empty(&a) == !b_has_node);
    assert(aws_linked_list_empty(&b) == !a_has_node);
}
