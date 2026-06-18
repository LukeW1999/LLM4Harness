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

    /* Use the default allocator – always succeeds */
    list.alloc = aws_default_allocator();

    /* Bound item_size and capacity (current_size) */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size > 0 && list.current_size <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Allocate exactly current_size * item_size bytes; guaranteed to succeed */
    list.data = aws_mem_acquire(list.alloc, list.current_size * list.item_size);
    __CPROVER_assume(list.data != NULL);

    /* Set length so that list is valid and we have at least one element to erase */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length > 0 && list.length <= list.current_size);

    /* Explicitly assume the list is valid (satisfies all invariants) */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Choose a valid index to erase */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    /* Save the state before the call */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage;
    save_byte_from_array((uint8_t *)old.data, old.current_size * old.item_size, &storage);

    /* Call the function under test */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    assert(result == AWS_OP_SUCCESS);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length - 1);

    /* Data before the erased index stays unchanged */
    if (index > 0) {
        assert_bytes_match((uint8_t *)old.data, (uint8_t *)list.data, index * list.item_size);
    }

    /* Elements after the erased index shift one position left */
    for (size_t i = index; i < list.length; i++) {
        assert_bytes_match(
            (uint8_t *)old.data + (i + 1) * list.item_size,
            (uint8_t *)list.data + i * list.item_size,
            list.item_size);
    }

    /* The list remains valid */
    assert(aws_array_list_is_valid(&list));
}
