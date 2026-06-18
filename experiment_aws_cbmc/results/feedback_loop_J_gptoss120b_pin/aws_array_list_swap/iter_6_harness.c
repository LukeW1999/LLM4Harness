#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    __CPROVER_assume(list.current_size >= list.length * list.item_size);
    __CPROVER_assume(list.length <= list.current_size /
                     (list.item_size == 0 ? 1 : list.item_size));

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    size_t item_sz = list.item_size;
    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    if (item_sz > 0) {
        memcpy(old_a,
               (uint8_t *)list.data + a * item_sz,
               item_sz);
        memcpy(old_b,
               (uint8_t *)list.data + b * item_sz,
               item_sz);
    }

    int rc = aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (rc == AWS_OP_SUCCESS) {
        if (a == b || item_sz == 0) {
            if (item_sz > 0) {
                assert(memcmp((uint8_t *)list.data + a * item_sz,
                              old_a,
                              item_sz) == 0);
                assert(memcmp((uint8_t *)list.data + b * item_sz,
                              old_b,
                              item_sz) == 0);
            }
        } else {
            assert(memcmp((uint8_t *)list.data + a * item_sz,
                          old_b,
                          item_sz) == 0);
            assert(memcmp((uint8_t *)list.data + b * item_sz,
                          old_a,
                          item_sz) == 0);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
