#include <assert.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

/* Helper to compute the length of a list (number of data nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        ++len;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

/* Harness */
void harness() {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Stack‑allocated pool of possible nodes */
    struct aws_linked_list_node node_pool[5];
    for (size_t i = 0; i < 5; ++i) {
        aws_linked_list_node_init(&node_pool[i]);
    }

    /* Non‑deterministically create a list with up to 5 elements */
    for (size_t i = 0; i < 5; ++i) {
        if (__CPROVER_nondet_bool()) {
            aws_linked_list_push_back(&list, &node_pool[i]);
        }
    }

    /* Choose a 'before' node: either a node in the list or the sentinel */
    struct aws_linked_list_node *before;
    if (__CPROVER_nondet_bool()) {
        /* Use sentinel (insert at the end) */
        before = &list.head;
    } else {
        /* Pick a node from the pool */
        before = &node_pool[__CPROVER_nondet_uint() % 5];
    }

    /* Ensure 'before' satisfies the required preconditions */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(before->prev->next == before);
    __CPROVER_assume(before->next->prev == before);

    /* Allocate a node to add (stack‑allocated) */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;
    aws_linked_list_node_init(to_add);
    __CPROVER_assume(to_add->next == to_add && to_add->prev == to_add);

    /* Record state before the call */
    size_t old_len = list_length(&list);
    struct aws_linked_list_node *old_prev = before->prev;

    /* Call the function under verification */
    aws_linked_list_insert_before(before, to_add);

    /* Postcondition: list remains a valid doubly‑linked list */
    assert(aws_linked_list_is_valid(&list));

    /* Postcondition: length increased by one */
    size_t new_len = list_length(&list);
    assert(new_len == old_len + 1);

    /* Postcondition: links of the inserted node and its neighbours */
    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(before->prev == to_add);
}
