#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <assert.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    /* nondet item size */
    list.item_size = nondet_uint64_t();
    __CPROVER_assume(list.item_size > 0);
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_bytes);
    }

    int result = aws_array_list_clear(&list);

    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (old.data != NULL && old.current_size > 0) {
        assert_byte_from_buffer_matches(old.data, &old_bytes);
    }
}
