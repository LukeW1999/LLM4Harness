#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

/* Stub allocator acquire to always succeed */
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    (void)size;
    /* Return a dummy non‑NULL pointer */
    return (void *)0x1;
}

/* Stub allocator release (no‑op) */
void aws_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    (void)ptr;
}

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    size_t item_sz = list.item_size;
    uint8_t *data = (uint8_t *)list.data;

    uint8_t old_item_a[MAX_ITEM_SIZE];
    uint8_t old_item_b[MAX_ITEM_SIZE];

    for (size_t i = 0; i < item_sz; ++i) {
        old_item_a[i] = data[a * item_sz + i];
        old_item_b[i] = data[b * item_sz + i];
    }

    int rc = aws_array_list_swap(&list, a, b);
    assert(rc == AWS_OP_SUCCESS);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);

    if (a == b) {
        for (size_t idx = 0; idx < list.length; ++idx) {
            for (size_t i = 0; i < item_sz; ++i) {
                assert(data[idx * item_sz + i] ==
                       ((uint8_t *)old.data)[idx * item_sz + i]);
            }
        }
    } else {
        for (size_t i = 0; i < item_sz; ++i) {
            assert(data[a * item_sz + i] == old_item_b[i]);
            assert(data[b * item_sz + i] == old_item_a[i]);
        }
        for (size_t idx = 0; idx < list.length; ++idx) {
            if (idx != a && idx != b) {
                for (size_t i = 0; i < item_sz; ++i) {
                    assert(data[idx * item_sz + i] ==
                           ((uint8_t *)old.data)[idx * item_sz + i]);
                }
            }
        }
    }
}
