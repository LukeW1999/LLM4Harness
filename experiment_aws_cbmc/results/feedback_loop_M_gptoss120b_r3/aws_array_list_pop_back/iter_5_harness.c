#include <assert.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 256

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    uint8_t out_buffer[MAX_ITEM_SIZE];
    int result = aws_array_list_pop_back(&list, out_buffer);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_data);
        }
        assert(aws_array_list_is_valid(&list));
    }
}
