#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static int byte_memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    for (size_t i = 0; i < n; ++i) {
        if (pa[i] != pb[i]) {
            return 1;
        }
    }
    return 0;
}

void aws_array_list_set_at_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 32U);

    /* initialize list */
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, 64U, item_size);

    /* nondeterministic initial length */
    size_t init_len = nondet_size_t();
    __CPROVER_assume(init_len <= 64U);

    /* fill list with dummy elements */
    uint8_t *dummy = malloc(item_size);
    __CPROVER_assume(dummy != NULL);
    for (size_t i = 0; i < init_len; ++i) {
        (void)aws_array_list_push_back(&list, dummy);
    }
    free(dummy);

    /* snapshot pre‑state */
    size_t old_length   = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_item_sz  = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    uint8_t *old_data = NULL;
    if (list.data) {
        old_data = malloc(old_capacity * old_item_sz);
        __CPROVER_assume(old_data != NULL);
        for (size_t i = 0; i < old_capacity * old_item_sz; ++i) {
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    /* nondeterministic index and whether it is in‑range */
    size_t index = nondet_size_t();
    bool idx_in_range = nondet_bool();
    if (idx_in_range) {
        __CPROVER_assume(index < old_length);
    } else {
        __CPROVER_assume(index >= old_length);
    }

    /* value to set */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* call under verification */
    int ret = aws_array_list_set_at(&list, val, index);

    /* post‑conditions */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_sz);

    size_t new_capacity = aws_array_list_capacity(&list);
    assert(list.length <= new_capacity);
    assert(new_capacity >= old_capacity);

    if (ret == AWS_OP_SUCCESS) {
        assert(index < old_length);
        assert(byte_memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);
        if (old_data) {
            for (size_t i = 0; i < old_capacity; ++i) {
                if (i != index) {
                    assert(byte_memcmp((uint8_t *)list.data + (i * list.item_size),
                                       old_data + (i * list.item_size),
                                       list.item_size) == 0);
                }
            }
        }
    } else {
        assert(list.length == old_length);
        assert(new_capacity == old_capacity);
        if (old_data) {
            assert(byte_memcmp(list.data, old_data, old_capacity * list.item_size) == 0);
        }
    }

    free(val);
    free(old_data);
    aws_array_list_clean_up(&list);
}
