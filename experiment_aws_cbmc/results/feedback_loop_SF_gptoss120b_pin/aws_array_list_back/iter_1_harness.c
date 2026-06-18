#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
uint32_t nondet_uint32_t(void);
uint64_t nondet_uint64_t(void);

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic fields */
    list.alloc = alloc;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.length = nondet_size_t();
    list.current_size = nondet_size_t();

    /* Enforce validity constraints */
    __CPROVER_assume(list.length * list.item_size <= list.current_size);
    /* current_size must be a multiple of item_size (or zero) */
    __CPROVER_assume(list.current_size == list.item_size * (list.length + nondet_size_t()));

    /* Allocate backing storage if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
        /* Optionally nondet-initialize the buffer */
        uint8_t *buf = (uint8_t *)list.data;
        for (size_t i = 0; i < list.current_size; ++i) {
            buf[i] = nondet_uint8_t();
        }
    } else {
        list.data = NULL;
    }

    /* Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* PRE-CALL SNAPSHOT */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    uint8_t *old_data_copy = NULL;
    if (list.data != NULL) {
        old_data_copy = malloc(list.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, list.data, list.current_size);
    }

    /* CALL */
    int result = aws_array_list_back(&list, val);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* Return value reflects whether the list was non‑empty */
    assert((result == 0) == (old_length > 0));

    /* The list structure must remain unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    /* Backing storage must be unchanged */
    if (old_data != NULL) {
        assert(memcmp(list.data, old_data_copy, old_current_size) == 0);
    }

    /* When successful, the output buffer must contain the last element */
    if (result == 0) {
        assert(memcmp(val,
                      (uint8_t *)old_data + (old_length - 1) * old_item_size,
                      old_item_size) == 0);
    }
}
