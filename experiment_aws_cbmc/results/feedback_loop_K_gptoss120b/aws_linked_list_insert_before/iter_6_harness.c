/*
Preconditions:
- __CPROVER_assume(before != NULL);
- __CPROVER_assume(before->prev != NULL);
- __CPROVER_assume(before->prev->next == before);  // bidirectional link holds
- __CPROVER_assume(to_add->next == to_add && to_add->prev == to_add); // node not already in a list

Postconditions (validity):
- assert(aws_linked_list_is_valid(&list)); // the whole list remains a valid doubly‑linked list

Postconditions (length):
- let old_len be the number of data nodes in the list before the call;
- let new_len be the number after the call;
- assert(new_len == old_len + 1);

Postconditions (node links):
- assert(to_add->next == before);
- assert(to_add->prev == old_prev); // where old_prev = before->prev before insertion
- assert(before->prev == to_add);

Postconditions (frame):
- For every node n in the list other than to_add and before, the values of n->next and n->prev are unchanged.
*/

#include <proof_helpers/make_common_data_structures.h>

/* Helper to compute the length of a list (number of data nodes) */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        ++len;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

/* Helper to capture the prev pointer of a node before insertion */
static struct aws_linked_list_node *capture_prev(struct aws_linked_list_node *node) {
    return node->prev;
}

/* Harness */
void harness() {
    /* Allocate and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Stack‑allocated pool of possible nodes */
    struct aws_linked_list_node node_pool[5];
    size_t i;
    for (i = 0; i < 5; ++i) {
        aws_linked_list_node_init(&node_pool[i]);
    }

    /* Non‑deterministically create a list with up to 5 elements */
    for (i = 0; i < 5; ++i) {
        if (__CPROVER_nondet_bool()) {
            aws_linked_list_push_back(&list, &node_pool[i]);
        }
    }

    /* Choose a 'before' node from the list (including the sentinel) */
    struct aws_linked_list_node *before;
    if (__CPROVER_nondet_bool()) {
        /* Choose an existing data node */
        before = aws_linked_list_begin(&list);
        const struct aws_linked_list_node *end = aws_linked_list_end(&list);
        while (before != end && __CPROVER_nondet_bool()) {
            before = aws_linked_list_next(before);
        }
        /* If we walked off the end, fall back to sentinel */
        if (before == end) {
            before = (struct aws_linked_list_node *)&list.head;
        }
    } else {
        /* Directly use the sentinel (insert at the end) */
        before = (struct aws_linked_list_node *)&list.head;
    }

    /* Ensure 'before' satisfies the required preconditions */
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    __CPROVER_assume(before->prev->next == before);

    /* Allocate a node to add (stack‑allocated) */
    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;
    aws_linked_list_node_init(to_add);
    __CPROVER_assume(to_add->next == to_add && to_add->prev == to_add);

    /* Record state before the call */
    size_t old_len = list_length(&list);
    struct aws_linked_list_node *old_prev = capture_prev(before);

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
