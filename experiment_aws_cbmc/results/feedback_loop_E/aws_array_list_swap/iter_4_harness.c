#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    /* data structure */
    struct aws_array_list list;

    /* parameters */
    size_t a;
    size_t b;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* save old state */
    struct aws_array_list old_list = list;

    /* assume valid indices */
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* save elements at indices a and b */
    struct store_byte_from_buffer old_byte_a;
    struct store_byte_from_buffer old_byte_b;
    if (list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data + a * list.item_size, list.item_size, &old_byte_a);
        save_byte_from_array((uint8_t *)list.data + b * list.item_size, list.item_size, &old_byte_b);
    }

    /* perform operation under verification */
    aws_array_list_swap(&list, a, b);

    /* assertions */
    assert(aws_array_list_is_valid(&list));

    /* check that elements at indices a and b are swapped */
    if (list.data != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)list.data + a * list.item_size, &old_byte_b);
        assert_byte_from_buffer_matches((uint8_t *)list.data + b * list.item_size, &old_byte_a);
    }

    /* check that other elements remain unchanged */
    for (size_t i = 0; i < list.length; i++) {
        if (i != a && i != b) {
            struct store_byte_from_buffer old_byte;
            save_byte_from_array((uint8_t *)old_list.data + i * old_list.item_size, old_list.item_size, &old_byte);
            assert_byte_from_buffer_matches((uint8_t *)list.data + i * list.item_size, &old_byte);
        }
    }

    /* check that unchanged fields remain unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.data == old_list.data);
}
