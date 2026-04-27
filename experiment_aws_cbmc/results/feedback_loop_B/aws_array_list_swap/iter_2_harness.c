#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic indices */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 4. Assume preconditions */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert postconditions for BOTH success and failure paths */
    /* In this case, there is no failure path as the function does not return a status code */
    /* Success path */
    if (a != b) {
        /* Check that elements at index a and b are swapped */
        void *item1 = NULL;
        void *item2 = NULL;
        aws_array_list_get_at_ptr(&list, &item1, a);
        aws_array_list_get_at_ptr(&list, &item2, b);
        assert(AWS_MEM_IS_READABLE(item1, list.item_size));
        assert(AWS_MEM_IS_READABLE(item2, list.item_size));
        assert_bytes_match(item1, (char *)old.data + b * old.item_size, list.item_size);
        assert_bytes_match(item2, (char *)old.data + a * old.item_size, list.item_size);
    }

    /* 7. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 8. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
