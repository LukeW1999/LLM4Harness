#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_swap_contents_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list_a;
    struct aws_array_list list_b;
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old_list_a = list_a;
    struct aws_array_list old_list_b = list_b;

    /* 3. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(list_a.alloc == old_list_b.alloc);
    assert(list_a.current_size == old_list_b.current_size);
    assert(list_a.length == old_list_b.length);
    assert(list_a.item_size == old_list_b.item_size);
    assert(list_a.data == old_list_b.data);

    assert(list_b.alloc == old_list_a.alloc);
    assert(list_b.current_size == old_list_a.current_size);
    assert(list_b.length == old_list_a.length);
    assert(list_b.item_size == old_list_a.item_size);
    assert(list_b.data == old_list_a.data);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list_a.alloc == old_list_a.alloc || list_a.alloc == old_list_b.alloc);
    assert(list_b.alloc == old_list_a.alloc || list_b.alloc == old_list_b.alloc);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}

int main() {
    aws_array_list_swap_contents_harness();
    return 0;
}
