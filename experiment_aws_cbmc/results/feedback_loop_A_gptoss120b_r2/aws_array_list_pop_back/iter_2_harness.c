#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.allocator = allocator;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(old.length > 0);
        assert(list.length == old.length - 1);
    } else {
        assert(list.length == old.length);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    assert(aws_array_list_is_valid(&list));
}
