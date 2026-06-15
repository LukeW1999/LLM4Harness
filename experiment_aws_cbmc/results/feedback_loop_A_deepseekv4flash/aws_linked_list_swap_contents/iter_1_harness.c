#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_swap_contents_harness() {
    struct aws_linked_list a;
    struct aws_linked_list b;

    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));
    __CPROVER_assume(&a != &b);

    bool old_a_empty = aws_linked_list_empty(&a);
    bool old_b_empty = aws_linked_list_empty(&b);

    aws_linked_list_swap_contents(&a, &b);

    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_empty(&a) == old_b_empty);
    assert(aws_linked_list_empty(&b) == old_a_empty);
}
