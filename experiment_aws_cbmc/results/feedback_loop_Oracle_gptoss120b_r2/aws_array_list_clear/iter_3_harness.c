#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    bool make_zeroed = __CPROVER_nondet_bool();

    if (make_zeroed) {
        AWS_ZERO_STRUCT(list);
    } else {
        size_t item_size = __CPROVER_nondet_uint();
        __CPROVER_assume(item_size > 0 && item_size <= 8);

        size_t capacity = __CPROVER_nondet_uint();
        __CPROVER_assume(capacity <= 8);

        size_t current_size = 0;
        __CPROVER_assume(!aws_mul_size_checked(capacity, item_size, &current_size));

        uint8_t *data = NULL;
        if (current_size > 0) {
            data = (uint8_t *)aws_mem_acquire(alloc, current_size);
            __CPROVER_assume(data != NULL);
        }

        size_t length = __CPROVER_nondet_uint();
        __CPROVER_assume(length <= capacity);

        list.alloc = alloc;
        list.item_size = item_size;
        list.current_size = current_size;
        list.length = length;
        list.data = data;

        __CPROVER_assume(aws_array_list_is_valid(&list));
    }

    struct aws_allocator *orig_alloc = list.alloc;
    size_t orig_item_size = list.item_size;

    aws_array_list_clear(&list);

    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));
    assert(list.alloc == orig_alloc);
    assert(list.item_size == orig_item_size);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.data == NULL);
}
