#include <aws/common/array_list.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 2
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_size = list.item_size;

    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;
    struct store_byte_from_buffer val_storage;
    save_byte_from_array((const uint8_t *)val, item_size, &val_storage);

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(index * item_size + item_size <= list.current_size);
        assert_bytes_match((const uint8_t *)list.data + (index * item_size),
                           (const uint8_t *)val, item_size);
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
    }

    free(val);
}
