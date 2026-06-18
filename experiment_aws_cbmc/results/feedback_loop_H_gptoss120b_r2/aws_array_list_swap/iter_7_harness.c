#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.length > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    size_t data_bytes = list.length * list.item_size;
    unsigned char old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    __CPROVER_assume(data_bytes <= sizeof(old_data));
    if (data_bytes > 0) {
        memcpy(old_data, (const unsigned char *)list.data, data_bytes);
    }

    unsigned char elem_a[MAX_ITEM_SIZE];
    unsigned char elem_b[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= sizeof(elem_a));
    __CPROVER_assume(list.item_size <= sizeof(elem_b));
    if (list.item_size > 0) {
        memcpy(elem_a,
               (const unsigned char *)list.data + a * list.item_size,
               list.item_size);
        memcpy(elem_b,
               (const unsigned char *)list.data + b * list.item_size,
               list.item_size);
    }

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    if (list.item_size > 0) {
        if (a != b) {
            assert(memcmp((const unsigned char *)list.data + a * list.item_size,
                          elem_b,
                          list.item_size) == 0);
            assert(memcmp((const unsigned char *)list.data + b * list.item_size,
                          elem_a,
                          list.item_size) == 0);
        } else {
            assert(memcmp((const unsigned char *)list.data + a * list.item_size,
                          elem_a,
                          list.item_size) == 0);
        }
    }

    if (data_bytes > 0) {
        for (size_t i = 0; i < list.length; ++i) {
            if (i == a || i == b) {
                continue;
            }
            assert(memcmp((const unsigned char *)list.data + i * list.item_size,
                          old_data + i * list.item_size,
                          list.item_size) == 0);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
