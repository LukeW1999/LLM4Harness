#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* Save bytes at indices a and b to verify the swap */
    struct store_byte_from_buffer old_byte_a[MAX_ITEM_SIZE];
    struct store_byte_from_buffer old_byte_b[MAX_ITEM_SIZE];
    
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    
    save_byte_from_array((uint8_t *)list.data + a * list.item_size, list.item_size, old_byte_a);
    save_byte_from_array((uint8_t *)list.data + b * list.item_size, list.item_size, old_byte_b);

    /* 3. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 4. Assert postconditions for changed fields */
    if (a != b) {
        assert_byte_from_buffer_matches((uint8_t *)list.data + a * list.item_size, old_byte_b);
        assert_byte_from_buffer_matches((uint8_t *)list.data + b * list.item_size, old_byte_a);
    } else {
        assert_byte_from_buffer_matches((uint8_t *)list.data + a * list.item_size, old_byte_a);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
