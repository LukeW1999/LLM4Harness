#include <assert.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void harness(void) {
    struct aws_array_list list = {0};

    bool make_valid = __CPROVER_nondet_bool();

    size_t old_current_size = 0;
    size_t old_item_size = 0;
    size_t old_length = 0;
    struct aws_allocator *old_alloc = NULL;
    void *old_data = NULL;

    if (make_valid) {
        list.item_size = __CPROVER_nondet_size_t();
        __CPROVER_assume(list.item_size > 0);

        list.length = __CPROVER_nondet_size_t();

        size_t product;
        __CPROVER_assume(!aws_mul_size_checked(list.length, list.item_size, &product));
        list.current_size = product;

        struct aws_allocator *allocator = aws_default_allocator();
        list.alloc = allocator;

        if (list.current_size > 0) {
            list.data = allocator->mem_acquire(allocator, list.current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }

        old_current_size = list.current_size;
        old_item_size = list.item_size;
        old_length = list.length;
        old_alloc = list.alloc;
        old_data = list.data;
    } else {
        old_current_size = 0;
        old_item_size = 0;
        old_length = 0;
        old_alloc = NULL;
        old_data = NULL;
    }

    aws_array_list_clear(&list);

    assert(list.length == 0);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);

    bool is_zeroed = (list.length == 0) && (list.current_size == 0) &&
                     (list.item_size == 0) && (list.alloc == NULL) && (list.data == NULL);
    assert(is_zeroed || aws_array_list_is_valid(&list));
}
