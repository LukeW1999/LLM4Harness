#include <assert.h>
#include <stdlib.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.allocator = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    uint8_t *out_buf = malloc(list.item_size);
    __CPROVER_assume(out_buf != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(out_buf, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    int result = aws_array_list_get_at(&list, out_buf, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((const uint8_t *)list.data + (index * list.item_size),
                           (const uint8_t *)out_buf,
                           list.item_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data) {
            assert_array_list_equivalence(&list, &old, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(out_buf);
}
