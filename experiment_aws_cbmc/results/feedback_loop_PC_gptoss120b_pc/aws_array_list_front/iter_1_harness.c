#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Snapshot old state */
    struct aws_array_list old = list;

    /* 3. Choose nondeterministic indices within bounds */
    size_t len = list.length;
    __CPROVER_assume(len > 0);                     /* need at least one element for a valid swap */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < len);
    __CPROVER_assume(b < len);

    /* 4. Save a byte from each element to compare after the swap */
    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;
    uint8_t *ptr_a = (uint8_t *)list.data + a * list.item_size;
    uint8_t *ptr_b = (uint8_t *)list.data + b * list.item_size;
    __CPROVER_assume(AWS_MEM_IS_READABLE(ptr_a, list.item_size));
    __CPROVER_assume(AWS_MEM_IS_READABLE(ptr_b, list.item_size));
    save_byte_from_array(ptr_a, list.item_size, &storage_a);
    save_byte_from_array(ptr_b, list.item_size, &storage_b);

    /* 5. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Postconditions */

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* Swapped elements: the byte saved from position a must now be at position b and vice‑versa */
    assert_byte_from_buffer_matches(ptr_a, &storage_b);
    assert_byte_from_buffer_matches(ptr_b, &storage_a);
}
