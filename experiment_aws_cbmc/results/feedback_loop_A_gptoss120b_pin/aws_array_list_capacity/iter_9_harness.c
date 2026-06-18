#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0 && initial_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    aws_array_list_init(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct store_byte_from_buffer val_storage;
    save_byte_from_array(val, list.item_size, &val_storage);

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert_bytes_match(
            (const uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    free(val);
    aws_array_list_clean_up(&list);
}
