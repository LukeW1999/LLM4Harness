#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose a small, non‑zero item size */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 8);

    /* Choose a small capacity */
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 8);

    /* Static buffer to avoid dynamic allocation */
    uint8_t data[64];
    __CPROVER_assume(capacity * item_size <= sizeof(data));

    struct aws_array_list list;
    int init_result = aws_array_list_init_static(
        &list, allocator, data, capacity * item_size, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Choose a length that fits within the allocated buffer */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    void *old_val = NULL;
    void *val = old_val;
    void **val_ptr = &val;

    /* Index may be in‑bounds or out‑of‑bounds */
    size_t index;
    __CPROVER_assume(index <= list.length + 2);

    int ret = aws_array_list_get_at_ptr(&list, val_ptr, index);

    if (index < list.length) {
        assert(ret == AWS_OP_SUCCESS);
        assert(*val_ptr == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        assert(ret == AWS_ERROR_INVALID_INDEX);
        assert(*val_ptr == old_val);
    }

    /* Verify that the list's metadata is unchanged */
    assert(list.alloc == allocator);
    assert(list.item_size == item_size);
    assert(list.current_size == capacity * item_size);
    assert(list.length == length);
    assert(list.data == data);
}
