#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = __CPROVER_nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    size_t item_sz = list.item_size;
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < item_sz; ++i) {
        val_buf[i] = __CPROVER_nondet_unsigned_char();
    }

    int result = aws_array_list_set_at(&list, val_buf, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
}
