#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list = {0};
    list.allocator = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= old.length + 5);
    __CPROVER_assume(old.item_size != 0);

    struct store_byte_from_buffer old_byte_at_index;
    if (index < old.length) {
        save_byte_from_array((uint8_t *)old.data + index * old.item_size,
                             old.item_size,
                             &old_byte_at_index);
    }

    size_t other_index = nondet_size_t();
    __CPROVER_assume(old.length > 0);
    __CPROVER_assume(other_index < old.length);
    __CPROVER_assume(other_index != index);
    struct store_byte_from_buffer old_byte_other;
    save_byte_from_array((uint8_t *)old.data + other_index * old.item_size,
                         old.item_size,
                         &old_byte_other);

    uint8_t *val = (uint8_t *)allocator->mem_acquire(allocator, old.item_size);
    __CPROVER_assume(val != NULL);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);

        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data +
                                            other_index * list.item_size,
                                            &old_byte_other);
        }

        assert(list.allocator == old.allocator);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.allocator == old.allocator);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        if (index < old.length) {
            assert_byte_from_buffer_matches((uint8_t *)list.data +
                                            index * list.item_size,
                                            &old_byte_at_index);
        }
    }

    assert(aws_array_list_is_valid(&list));

    allocator->mem_release(allocator, val);
}
