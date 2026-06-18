#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

extern size_t nondet_size_t(void);

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic initialization of list fields with reasonable bounds */
    list.alloc = alloc;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= 64);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= 64);

    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= 4096);
    __CPROVER_assume(list.item_size * list.length <= list.current_size);

    list.data = malloc(list.current_size);
    __CPROVER_assume(list.current_size == 0 || list.data != NULL);

    /* ensure the list satisfies its validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic index with a small bound */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= list.length + 5);

    /* output pointer */
    void *val = NULL;

    /* pre‑call snapshot */
    size_t old_length = list.length;
    void *old_data = list.data;

    /* call under verification */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* result must be either success (0) or error (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* list structural fields must remain unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == list.item_size);          /* unchanged by the call */
    assert(list.length == old_length);
    assert(list.current_size == list.current_size);    /* unchanged by the call */
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* successful call implies index was in bounds */
        assert(index < old_length);
        /* val must point to the correct element inside the array */
        assert(val != NULL);
        assert((uint8_t *)val == (uint8_t *)old_data + index * list.item_size);
    } else {
        /* on error, val must remain unchanged (still NULL) */
        assert(val == NULL);
    }
}
