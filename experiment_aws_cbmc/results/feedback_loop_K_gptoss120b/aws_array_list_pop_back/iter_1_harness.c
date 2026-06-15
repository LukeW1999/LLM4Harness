/*  
 * Contract for aws_array_list_pop_back  
 * Preconditions:  
 *   - __CPROVER_assume(list != NULL);  
 *   - __CPROVER_assume(list->item_size > 0);  
 *   - __CPROVER_assume(list->current_size == list->item_size * capacity);  
 *   - __CPROVER_assume(list->length <= capacity);  
 *   - __CPROVER_assume(list->alloc == NULL || list->alloc != NULL);  // allocator may be NULL for static lists  
 *   - __CPROVER_assume(aws_array_list_is_valid(list));  
 *   - If list->length > 0 then __CPROVER_assume(list->data != NULL);  
 * Postconditions (validity):  
 *   - aws_array_list_is_valid(&list) holds after the call.  
 *   - The return value is AWS_OP_SUCCESS when the original length > 0, otherwise AWS_ERROR_LIST_EMPTY.  
 * Postconditions (length):  
 *   - If original length > 0 then list.length == original_length - 1.  
 *   - If original length == 0 then list.length == 0.  
 * Postconditions (frame):  
 *   - list->item_size, list->current_size, list->alloc, and list->data pointer are unchanged.  
 *   - The memory region of the popped element is zeroed.  
 *   - All other bytes of the underlying buffer remain unchanged.  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* nondeterministic item size (bounded) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 64);

    /* nondeterministic capacity (bounded) */
    size_t capacity;
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 10);

    /* allocate raw buffer for static list */
    uint8_t *raw = malloc(item_size * capacity);
    __CPROVER_assume(raw != NULL);

    /* nondeterministic initial length */
    size_t init_len;
    __CPROVER_assume(init_len <= capacity);

    struct aws_array_list list;
    list.alloc = NULL;                     /* static list */
    list.item_size = item_size;
    list.current_size = item_size * capacity;
    list.length = init_len;
    list.data = raw;

    /* ensure the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* make a copy of the underlying buffer to compare later */
    uint8_t *old_data = malloc(item_size * capacity);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, raw, item_size * capacity);

    size_t old_length = list.length;

    int ret = aws_array_list_pop_back(&list);

    /* postcondition: list validity */
    assert(aws_array_list_is_valid(&list));

    /* postcondition: unchanged fields */
    assert(list.item_size == item_size);
    assert(list.current_size == item_size * capacity);
    assert(list.alloc == NULL);
    assert(list.data == raw);

    if (old_length > 0) {
        /* success case */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);

        /* the popped element must be zeroed */
        uint8_t *popped_elem = (uint8_t *)list.data + item_size * list.length;
        for (size_t i = 0; i < item_size; ++i) {
            assert(popped_elem[i] == 0);
        }

        /* unchanged bytes before the popped element */
        assert(memcmp(old_data, raw, item_size * list.length) == 0);
    } else {
        /* error case */
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(list.length == 0);

        /* buffer must be unchanged */
        assert(memcmp(old_data, raw, item_size * capacity) == 0);
    }

    free(old_data);
    free(raw);
    return 0;
}
