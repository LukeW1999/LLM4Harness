#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Assume the function being verified is declared */
int aws_array_list_back(const struct aws_array_list *list, void *val);

/* Helper macro for CBMC memory writable check (replace with your CBMC version if needed) */
#ifndef AWS_MEM_IS_WRITABLE
#define AWS_MEM_IS_WRITABLE(ptr, size) (ptr != NULL)
#endif

/* Constants for the return values */
#define AWS_OP_SUCCESS 0
#define AWS_OP_ERROR (-1)

void aws_array_list_back_harness() {
    struct aws_array_list list;
    size_t max_length = 10;
    size_t max_item_size = 100;

    /* Nondeterministic but bounded fields */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= max_item_size);
    __CPROVER_assume(list.length <= max_length);
    __CPROVER_assume(list.current_size >= list.item_size * list.length);

    /* Allocate data buffer or set to NULL if empty */
    if (list.current_size == 0) {
        __CPROVER_assume(list.data == NULL);
    } else {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
        /* Mark memory as writable */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    }

    /* Allocator pointer is not used by the function, but assume valid */
    list.alloc = (void*)1; /* any non-NULL value */

    /* Output buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Save state for postcondition checks */
    struct aws_array_list old = list;
    uint8_t *saved_last = NULL;
    if (list.length > 0) {
        saved_last = malloc(list.item_size);
        __CPROVER_assume(saved_last != NULL);
        memcpy(saved_last, (uint8_t*)list.data + list.item_size * (list.length - 1), list.item_size);
    }

    int result = aws_array_list_back(&list, val);

    /* List must be unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Postcondition: success iff length > 0, and then val contains the last element */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert(memcmp(val, saved_last, list.item_size) == 0);
    } else {
        assert(list.length == 0);
    }

    free(val);
    if (saved_last) free(saved_last);
    if (list.data) free(list.data);
}
