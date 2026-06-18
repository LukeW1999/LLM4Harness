#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    /* Choose nondeterministic item size and initial capacity within bounds */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialise the list */
    aws_array_list_init(&list, allocator, initial_capacity, item_size);

    /* Ensure the list satisfies the generic bounds and internal invariants */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Guarantee there is at least one element to set */
    if (list.length == 0) {
        uint8_t dummy[MAX_ITEM_SIZE];
        for (size_t i = 0; i < item_size; ++i) {
            dummy[i] = nondet_uint8_t();
        }
        aws_array_list_push_back(&list, dummy);
    }

    /* Choose a valid index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    /* Prepare a nondeterministic value to set */
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    /* Preserve the old state for later comparison */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.data != NULL);
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(dest, (const uint8_t *)val, list.item_size);
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_bytes);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
