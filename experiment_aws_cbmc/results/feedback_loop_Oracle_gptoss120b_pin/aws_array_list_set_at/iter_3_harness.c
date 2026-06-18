#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_INITIAL_ITEM_ALLOCATION 64U
#define MAX_ITEM_SIZE               32U

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* initialize list */
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_size);

    /* nondeterministic length (number of initialized elements) */
    size_t init_len;
    __CPROVER_assume(init_len <= MAX_INITIAL_ITEM_ALLOCATION);

    /* fill list with dummy data up to init_len */
    uint8_t *dummy = malloc(item_size);
    __CPROVER_assume(dummy);
    for (size_t i = 0; i < init_len; ++i) {
        /* ignore return value for harness */
        aws_array_list_push_back(&list, dummy);
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
        __CPROVER_assume(old_data);
        memcpy(old_data, list.data, old_capacity * old_item_sz);
    }

    /* nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= UINT_MAX);

    /* value to set */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val);

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
        assert(memcmp((uint8_t *)list.data + (index * list.item_size), val, list.item_size) == 0);
        if (old_data) {
            for (size_t i = 0; i < old_capacity; ++i) {
                if (i != index) {
                    assert(memcmp((uint8_t *)list.data + (i * list.item_size),
                                  old_data + (i * list.item_size),
                                  list.item_size) == 0);
                }
            }
        }
    } else {
        assert(list.length == old_length);
        assert(new_capacity == old_capacity);
        if (old_data) {
            assert(memcmp(list.data, old_data, old_capacity * list.item_size) == 0);
        }
    }

    free(val);
    free(old_data);
    aws_array_list_clean_up(&list);
}
