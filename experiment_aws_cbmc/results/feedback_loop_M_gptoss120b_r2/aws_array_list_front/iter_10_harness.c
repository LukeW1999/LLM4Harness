#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 256

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.allocator = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t out_buf[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= sizeof(out_buf));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data && old.length > 0) {
        save_byte_from_array(old.data, old.item_size, &old_byte);
    }

    int result = aws_array_list_front(&list, out_buf);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert_bytes_match((uint8_t *)list.data,
                           out_buf,
                           list.item_size);
        assert(list.allocator == old.allocator);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        assert(old.length == 0);
        assert(list.allocator == old.allocator);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
