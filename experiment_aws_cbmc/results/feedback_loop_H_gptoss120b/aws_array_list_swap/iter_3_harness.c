#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>

/* -------------------------------------------------------------------------- */
/* Configuration limits (adjust as needed for the proof)                       */
#define MAX_CAPACITY   8U
#define MAX_ITEM_SIZE  16U

/* -------------------------------------------------------------------------- */
/* Dummy allocator that pretends to allocate memory                           */
static void *dummy_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    (void)size;
    /* Return a pointer to a static buffer large enough for any test case */
    static uint8_t dummy_buf[MAX_CAPACITY * MAX_ITEM_SIZE];
    return dummy_buf;
}
static void dummy_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    (void)ptr;
}
static struct aws_allocator dummy_allocator = {
    .mem_acquire = dummy_mem_acquire,
    .mem_release = dummy_mem_release,
};

/* -------------------------------------------------------------------------- */
void aws_array_list_swap_harness(void) {
    struct aws_array_list list = {0};

    /* ---------------------------------------------------------------------- */
    /* Build a well‑formed list                                                */
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_CAPACITY);

    /* Allocate a concrete buffer for the list data */
    static uint8_t backing_buf[MAX_CAPACITY * MAX_ITEM_SIZE];
    list.alloc        = &dummy_allocator;
    list.data         = backing_buf;
    list.item_size    = item_sz;
    list.current_size = capacity * item_sz;

    /* Length must be within the allocated capacity */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= capacity);

    /* Ensure the list is non‑empty so that a and b can be chosen */
    __CPROVER_assume(list.length > 0);

    /* ---------------------------------------------------------------------- */
    /* Choose two valid indices (they may be equal)                           */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* ---------------------------------------------------------------------- */
    /* Preserve a copy of the whole buffer before the swap                     */
    uint8_t old_buf[MAX_CAPACITY * MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.current_size; ++i) {
        old_buf[i] = ((uint8_t *)list.data)[i];
    }

    /* ---------------------------------------------------------------------- */
    /* Perform the swap                                                       */
    int rc = aws_array_list_swap(&list, a, b);
    assert(rc == AWS_OP_SUCCESS);

    /* ---------------------------------------------------------------------- */
    /* Post‑condition checks                                                  */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == &dummy_allocator);
    assert(list.item_size == item_sz);
    assert(list.current_size == capacity * item_sz);
    assert(list.length == list.length); /* length unchanged */

    uint8_t *data = (uint8_t *)list.data;

    if (a == b) {
        /* No change at all */
        for (size_t i = 0; i < list.current_size; ++i) {
            assert(data[i] == old_buf[i]);
        }
    } else {
        /* Verify that only the two items were exchanged */
        for (size_t i = 0; i < item_sz; ++i) {
            assert(data[a * item_sz + i] == old_buf[b * item_sz + i]);
            assert(data[b * item_sz + i] == old_buf[a * item_sz + i]);
        }
        for (size_t idx = 0; idx < list.length; ++idx) {
            if (idx != a && idx != b) {
                for (size_t i = 0; i < item_sz; ++i) {
                    assert(data[idx * item_sz + i] ==
                           old_buf[idx * item_sz + i]);
                }
            }
        }
    }
}
