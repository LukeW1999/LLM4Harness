#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.length > 0);

    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    size_t data_bytes = list.length * list.item_size;
    unsigned char old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    __CPROVER_assume(data_bytes <= sizeof(old_data));
    if (data_bytes > 0) {
        aws_memcpy(old_data, list.data, data_bytes);
    }

    unsigned char elem_a[MAX_ITEM_SIZE];
    unsigned char elem_b[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= sizeof(elem_a));
    __CPROVER_assume(list.item_size <= sizeof(elem_b));
    if (list.item_size > 0) {
        aws_memcpy(elem_a,
                   (const unsigned char *)list.data + a * list.item_size,
                   list.item_size);
        aws_memcpy(elem_b,
                   (const unsigned char *)list.data + b * list.item_size,
                   list.item_size);
    }

    int rv = aws_array_list_swap(&list, a, b);
    __CPROVER_assume(rv == AWS_OP_SUCCESS);

    if (list.item_size > 0) {
        if (a != b) {
            __CPROVER_assert(aws_memcmp((const unsigned char *)list.data + a * list.item_size,
                                        elem_b,
                                        list.item_size) == 0,
                             "element a should be swapped with element b");
            __CPROVER_assert(aws_memcmp((const unsigned char *)list.data + b * list.item_size,
                                        elem_a,
                                        list.item_size) == 0,
                             "element b should be swapped with element a");
        } else {
            __CPROVER_assert(aws_memcmp((const unsigned char *)list.data + a * list.item_size,
                                        elem_a,
                                        list.item_size) == 0,
                             "element a unchanged when a == b");
        }
    }

    if (data_bytes > 0) {
        for (size_t i = 0; i < list.length; ++i) {
            if (i == a || i == b) {
                continue;
            }
            __CPROVER_assert(aws_memcmp((const unsigned char *)list.data + i * list.item_size,
                                        old_data + i * list.item_size,
                                        list.item_size) == 0,
                             "unchanged elements remain the same");
        }
    }

    __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
