#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    struct aws_array_list list;
    /* Use the default allocator to satisfy any pointer requirements */
    list.alloc = aws_default_allocator();

    /* Bound the list dimensions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Allocate the data member and assume it is valid */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Choose a non-deterministic index and force it to be valid to reach the success path */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    /* Save the old state */
    struct aws_array_list old = list;
    size_t data_size = old.current_size * old.item_size;
    struct store_byte_from_buffer storage;
    save_byte_from_array((uint8_t *)old.data, data_size, &storage);

    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    assert(result == AWS_OP_SUCCESS);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length - 1);

    /* Data before the erased index must remain unchanged */
    if (index > 0) {
        assert_bytes_match((uint8_t *)old.data, (uint8_t *)list.data, index * list.item_size);
    }

    /* Elements after the erased index shift one position to the left */
    for (size_t i = index; i < list.length; i++) {
        assert_bytes_match(
            (uint8_t *)old.data + (i + 1) * list.item_size,
            (uint8_t *)list.data + i * list.item_size,
            list.item_size);
    }

    /* The list must still satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
