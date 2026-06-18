#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 64U
#define MAX_ITEM_SIZE               32U

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic fields */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.current_size >= list.length * list.item_size);
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * list.item_size);
    list.alloc = allocator;

    /* allocate backing buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data);
    } else {
        list.data = NULL;
    }

    /* preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic index */
    size_t index;
    __CPROVER_assume(index <= (size_t)UINT_MAX);

    /* value to set */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val);

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
        assert(list.length >= old_length);
        assert(list.length >= index + 1);
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

    return;
}
