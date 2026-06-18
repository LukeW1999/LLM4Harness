#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 4. Save old element bytes for later comparison */
    uint8_t *ptr_a = (uint8_t *)list.data + a * list.item_size;
    uint8_t *ptr_b = (uint8_t *)list.data + b * list.item_size;
    uint8_t *old_ptr_a = (uint8_t *)old.data + a * old.item_size;
    uint8_t *old_ptr_b = (uint8_t *)old.data + b * old.item_size;

    /* 5. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 6. Assert unchanged fields of the list structure */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 7. Assert element swapping behavior */
    if (a == b) {
        /* No change when indices are equal */
        assert_bytes_match(ptr_a, old_ptr_a, list.item_size);
    } else {
        /* Elements at a and b are swapped */
        assert_bytes_match(ptr_a, old_ptr_b, list.item_size);
        assert_bytes_match(ptr_b, old_ptr_a, list.item_size);
    }

    /* 8. Assert validity invariant */
    assert(aws_array_list_is_valid(&list));
}
