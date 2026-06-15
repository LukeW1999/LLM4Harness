#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list a;
    struct aws_linked_list b;
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION, aws_default_allocator());
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION, aws_default_allocator());
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list_node *old_a_head = a.head;
    struct aws_linked_list_node *old_a_tail = a.tail;
    struct aws_linked_list_node *old_b_head = b.head;
    struct aws_linked_list_node *old_b_tail = b.tail;

    /* 3. Call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));

    /* 5. Assert fields that must NOT change regardless of result */
    assert(a.head == old_b_head);
    assert(a.tail == old_b_tail);
    assert(b.head == old_a_head);
    assert(b.tail == old_a_tail);

    /* 6. Test edge cases */
    bool a_is_empty = aws_linked_list_is_empty(&a);
    bool b_is_empty = aws_linked_list_is_empty(&b);
    assert(a_is_empty == aws_linked_list_is_empty(&b));
    assert(b_is_empty == aws_linked_list_is_empty(&a));

    /* 7. Test size */
    size_t a_size = aws_linked_list_size(&a);
    size_t b_size = aws_linked_list_size(&b);
    assert(a_size == aws_linked_list_size(&b));
    assert(b_size == aws_linked_list_size(&a));

    /* 8. Test swap back */
    aws_linked_list_swap_contents(&a, &b);
    assert(a.head == old_a_head);
    assert(a.tail == old_a_tail);
    assert(b.head == old_b_head);
    assert(b.tail == old_b_tail);
}

int main() {
    aws_linked_list_swap_contents_harness();
    return 0;
}
