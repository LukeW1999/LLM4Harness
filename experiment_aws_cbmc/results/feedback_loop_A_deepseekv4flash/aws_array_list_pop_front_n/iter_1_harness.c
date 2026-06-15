#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_pop_front_n_harness() {
    /* -------- Declare and bound data structures -------- */
    struct aws_array_list list;
    size_t n;

    /* Ensure the list is valid and bounded */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Bounding n to avoid overflow in multiplication: n * item_size <= current_size */
    /* Also cap n to a reasonable bound to keep state space manageable */
    __CPROVER_assume(n <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(n * list.item_size / list.item_size == n);  // no overflow

    /* -------- Save old state -------- */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* -------- Call function under test -------- */
    aws_array_list_pop_front_n(&list, n);

    /* -------- Postconditions -------- */

    /* 1. Fields that must NOT change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 2. Length change */
    if (n >= old.length) {
        /* If n >= old.length, list is cleared */
        assert(list.length == 0);
    } else {
        /* Otherwise, length decreases by n */
        assert(list.length == old.length - n);
    }

    /* 3. Data contents check (if n > 0 and not clearing all) */
    if (n > 0 && n < old.length) {
        size_t remaining_bytes = (old.length - n) * list.item_size;
        /* Check that the first remaining_bytes of data now match the old data starting at offset n */
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data + n * list.item_size, remaining_bytes);
        /* The data beyond remaining_bytes (up to current_size) should be either unchanged or filled with debug bytes.
           In DEBUG_BUILD, it's filled with AWS_ARRAY_LIST_DEBUG_FILL; otherwise it's unchanged.
           We can skip checking this region because it's not specified behavior. */
    }

    /* 4. List validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* 5. For the case where n == 0, nothing changes */
    if (n == 0) {
        assert(list.length == old.length);
        /* Also data should be unchanged: we already checked bytes_match above when n=0, but we can strengthen */
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.current_size);
    }
}
