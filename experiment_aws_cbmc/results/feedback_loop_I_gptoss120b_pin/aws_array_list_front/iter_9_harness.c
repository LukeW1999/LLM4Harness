#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *out = NULL;
    if (list.item_size > 0) {
        out = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(out != NULL);
    }

    struct aws_array_list old = list;

    int result = aws_array_list_front(&list, out);

    if (result == AWS_OP_SUCCESS) {
        if (list.item_size > 0 && list.length > 0) {
            assert(memcmp((const uint8_t *)list.data,
                          (const uint8_t *)out,
                          list.item_size) == 0);
        }
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    } else {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));

    if (out) {
        aws_mem_release(allocator, out);
    }
}
