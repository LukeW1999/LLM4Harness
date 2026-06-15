#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

_Bool nondet_bool(void);

/* Failing allocator that always returns NULL on allocation attempts */
static void *failing_mem_acquire(struct aws_allocator *allocator, size_t size) {
    (void)allocator;
    (void)size;
    return NULL;
}
static void failing_mem_release(struct aws_allocator *allocator, void *ptr) {
    (void)allocator;
    (void)ptr;
}
static void *failing_mem_realloc(struct aws_allocator *allocator, void *ptr, size_t size) {
    (void)allocator;
    (void)ptr;
    (void)size;
    return NULL;
}
static void *failing_mem_calloc(struct aws_allocator *allocator, size_t num, size_t size) {
    (void)allocator;
    (void)num;
    (void)size;
    return NULL;
}
static struct aws_allocator failing_allocator = {
    .mem_acquire = failing_mem_acquire,
    .mem_release = failing_mem_release,
    .mem_realloc = failing_mem_realloc,
    .mem_calloc  = failing_mem_calloc
};

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Choose allocator */
    if (nondet_bool()) {
        list.alloc = &failing_allocator;
    } else {
        list.alloc = aws_default_allocator();
    }

    /* Choose whether the list is full */
    if (nondet_bool()) {
        __CPROVER_assume(list.length == list.current_size);
    } else {
        __CPROVER_assume(list.length < list.current_size);
    }

    /* Allocate nondeterministic input value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (old.data != NULL && old.length > 0) {
        old_data = malloc(old.length * old.item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old.length * old.item_size);
    }

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Unconditional sanity check */
    assert(aws_array_list_is_valid(&list));

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        for (size_t i = 0; i < list.item_size; ++i) {
            assert(((uint8_t *)list.data)[i] == ((uint8_t *)val)[i]);
        }
        if (old.length > 0) {
            for (size_t i = 0; i < old.length * old.item_size; ++i) {
                assert(((uint8_t *)list.data)[list.item_size + i] == old_data[i]);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            for (size_t i = 0; i < old.length * old.item_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    free(val);
    free(old_data);
}
