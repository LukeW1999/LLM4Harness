#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 3. Assume preconditions */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert postconditions for BOTH success and failure paths */
    if (a != b) {
        /* 7. Changed fields (from Doxygen) */
        /* Elements at index a and b are swapped */
        void *item1_old = (char *)old.data + a * old.item_size;
        void *item2_old = (char *)old.data + b * old.item_size;
        void *item1_new = (char *)list.data + a * list.item_size;
        void *item2_new = (char *)list.data + b * list.item_size;
        assert_bytes_match(item1_new, item2_old, list.item_size);
        assert_bytes_match(item2_new, item1_old, list.item_size);
    } else {
        /* No change if a == b */
        assert_array_list_equivalence(&list, &old, NULL);
    }

    /* 8. Unchanged fields (implied — Doxygen rarely lists these) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 9. Validity invariants */
    assert(aws_array_list_is_valid(&list));
}
