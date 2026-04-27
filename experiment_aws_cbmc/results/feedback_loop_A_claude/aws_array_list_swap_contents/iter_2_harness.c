#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_swap_contents_harness() {
    /* 1. Declare and bound list_a */
    struct aws_array_list list_a;
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_a);
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(list_a.alloc != NULL);

    /* 2. Declare and bound list_b */
    struct aws_array_list list_b;
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_b);
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 3. Preconditions from FATAL_PRECONDITION in implementation */
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);

    /* 4. Save old state before calling */
    size_t old_a_current_size = list_a.current_size;
    size_t old_a_length = list_a.length;
    size_t old_a_item_size = list_a.item_size;
    void *old_a_data = list_a.data;
    struct aws_allocator *old_a_alloc = list_a.alloc;

    size_t old_b_current_size = list_b.current_size;
    size_t old_b_length = list_b.length;
    size_t old_b_item_size = list_b.item_size;
    void *old_b_data = list_b.data;
    struct aws_allocator *old_b_alloc = list_b.alloc;

    /* 5. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 6. Assert postconditions: contents are swapped */
    assert(list_a.current_size == old_b_current_size);
    assert(list_a.length == old_b_length);
    assert(list_a.item_size == old_b_item_size);
    assert(list_a.data == old_b_data);
    assert(list_a.alloc == old_b_alloc);

    assert(list_b.current_size == old_a_current_size);
    assert(list_b.length == old_a_length);
    assert(list_b.item_size == old_a_item_size);
    assert(list_b.data == old_a_data);
    assert(list_b.alloc == old_a_alloc);

    assert(list_a.alloc == list_b.alloc);
    assert(list_a.item_size == list_b.item_size);

    /* 7. Assert validity invariants */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
