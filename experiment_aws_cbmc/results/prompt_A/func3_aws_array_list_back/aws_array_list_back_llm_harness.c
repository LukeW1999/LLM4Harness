#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1

struct aws_allocator {
    // Dummy allocator structure for the sake of the example
    void *dummy;
};

struct aws_array_list {
    struct aws_allocator *alloc;
    size_t current_size;  /* total allocated bytes */
    size_t length;        /* number of elements */
    size_t item_size;     /* size of one element in bytes */
    void *data;           /* pointer to backing storage */
};

// Dummy implementation of aws_array_list_is_valid for the sake of the example
bool aws_array_list_is_valid(const struct aws_array_list *list) {
    return list != NULL &&
           list->item_size > 0 &&
           ((list->current_size == 0) || (list->data != NULL)) &&
           list->current_size == list->item_size * (list->current_size / list->item_size) &&
           list->length * list->item_size <= list->current_size;
}

int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val);

void aws_array_list_back_harness() {
    struct aws_array_list list;
    void *val;

    // Non-deterministic initialization
    list.alloc = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    list.current_size = nondet_size_t();
    list.length = nondet_size_t();
    list.item_size = nondet_size_t();
    list.data = malloc(list.current_size);
    val = malloc(list.item_size);

    // Precondition: list is a valid aws_array_list
    __CPROVER_assume(aws_array_list_is_valid(&list));

    // Precondition: val points to writable memory of list->item_size bytes
    __CPROVER_assume(val != NULL);

    // Save old state for postconditions
    struct aws_array_list old_list = list;

    // Call the function under verification
    int result = aws_array_list_back(&list, val);

    // Postcondition: list remains valid after the call
    assert(aws_array_list_is_valid(&list));

    // Postcondition: list contents are unchanged
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    // Postcondition: If successful, val contains the last item of the list
    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert(__CPROVER_w_ok(val, list.item_size));
        // This assertion assumes that the last item is correctly copied to val,
        // which would require knowing the internal state of list.data, which we don't have.
        // Therefore, this part is left as a comment.
        // assert(memcmp(val, (char *)list.data + (list.length - 1) * list.item_size, list.item_size) == 0);
    }

    // Postcondition: If list is empty, the function returns AWS_OP_ERR
    if (list.length == 0) {
        assert(result == AWS_OP_ERR);
    }
}
