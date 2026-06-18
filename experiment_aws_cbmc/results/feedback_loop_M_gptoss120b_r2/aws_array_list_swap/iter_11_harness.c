#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

extern uint64_t nondet_uint64_t(void);

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t total_bytes = list.length * list.item_size;
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    if (total_bytes > 0) {
        uint8_t *data_ptr = (uint8_t *)list.data;
        for (size_t i = 0; i < total_bytes; ++i) {
            old_data[i] = data_ptr[i];
        }
    }

    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (total_bytes > 0) {
        uint8_t *data_ptr = (uint8_t *)list.data;
        size_t item_sz = list.item_size;

        for (size_t idx = 0; idx < list.length; ++idx) {
            for (size_t i = 0; i < item_sz; ++i) {
                size_t offset = idx * item_sz + i;
                if (idx == a) {
                    assert(data_ptr[offset] == old_data[b * item_sz + i]);
                } else if (idx == b) {
                    assert(data_ptr[offset] == old_data[a * item_sz + i]);
                } else {
                    assert(data_ptr[offset] == old_data[offset]);
                }
            }
        }
    }

    assert(aws_array_list_is_valid(&list));
}
