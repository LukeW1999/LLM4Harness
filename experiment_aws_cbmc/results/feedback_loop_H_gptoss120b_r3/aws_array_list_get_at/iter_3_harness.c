#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t old_len = list.length;

    size_t item_sz = list.item_size;
    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(item_sz == 0 || index <= SIZE_MAX / item_sz);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old_len) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_len);
        }
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert_bytes_match((uint8_t *)list.data + index * item_sz,
                           val,
                           item_sz);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data && old.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               list.current_size);
        }
    }

    assert(aws_array_list_is_valid(&list));

    aws_mem_release(allocator, val);
}
