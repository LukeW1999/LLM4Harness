#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_ITEM_SIZE 256

void aws_array_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list = {0};
    list.alloc = allocator;

    /* nondet initialization of list fields */
    list.item_size = __CPROVER_nondet_uint();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    list.current_size = __CPROVER_nondet_uint();
    __CPROVER_assume(list.current_size > 0);

    list.length = __CPROVER_nondet_uint();
    __CPROVER_assume(list.length <= list.current_size);

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    unsigned char out_buf[MAX_ITEM_SIZE];

    int result = aws_array_list_pop_back(&list, (void *)out_buf);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(0 == memcmp(out_buf,
                           (unsigned char *)old.data + (old.length - 1) * old.item_size,
                           old.item_size));
    } else {
        assert(list.length == old.length);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
