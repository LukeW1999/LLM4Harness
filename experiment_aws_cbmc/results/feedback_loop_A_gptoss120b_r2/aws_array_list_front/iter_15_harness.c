#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/allocator.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_sz;
    __CPROVER_assume(item_sz > 0 && item_sz <= 1024);
    size_t initial_capacity;
    __CPROVER_assume(initial_capacity <= 64);
    aws_array_list_init(&list, allocator, initial_capacity, item_sz);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    unsigned char val_buf[1024];
    unsigned char *val = val_buf;
    __CPROVER_assume(val != NULL);
    bool do_push;
    __CPROVER_assume(do_push == true || do_push == false);
    if (do_push) {
        aws_array_list_push_back(&list, val);
    }

    struct aws_array_list old = list;

    const void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        assert(front != NULL);
        assert(front == list.data);
    } else {
        assert(front == NULL);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    assert(list.length == old.length);

    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
