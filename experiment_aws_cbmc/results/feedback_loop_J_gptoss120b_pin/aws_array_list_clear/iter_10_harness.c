#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 64);
    __CPROVER_assume(list.current_size <= 256);
    __CPROVER_assume(list.length <= (list.current_size / list.item_size));

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_bytes);
    }

    aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (old.data) {
        assert_byte_from_buffer_matches(list.data, &old_bytes);
    }
}
