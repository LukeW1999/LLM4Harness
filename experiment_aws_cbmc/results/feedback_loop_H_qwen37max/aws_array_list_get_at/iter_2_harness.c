#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 100
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 100
#endif

size_t nondet_size_t();
uint8_t nondet_uint8_t();

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    size_t index = nondet_size_t();
    
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; i++) {
        val[i] = nondet_uint8_t();
    }

    struct aws_array_list old = list;
    
    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == (index >= old.length ? index + 1 : old.length));
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        for (size_t i = 0; i < list.item_size; i++) {
            assert(dest[i] == val[i]);
        }
    } else {
        assert(list.length == old.length);
    }
    
    free(val);
}
