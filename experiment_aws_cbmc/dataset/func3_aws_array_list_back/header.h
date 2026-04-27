/* From: include/aws/common/array_list.h and array_list.inl */

struct aws_array_list {
    struct aws_allocator *alloc;
    size_t current_size;  /* total allocated bytes */
    size_t length;        /* number of elements */
    size_t item_size;     /* size of one element in bytes */
    void *data;           /* pointer to backing storage */
};

/**
 * Retrieves the element at the back (last item) of the array list.
 * Requires: list is a valid aws_array_list (aws_array_list_is_valid(list)).
 * Requires: val points to writable memory of list->item_size bytes.
 * Ensures: list remains valid after the call (aws_array_list_is_valid(list)).
 * Ensures: list contents are unchanged (read-only operation).
 * Returns: AWS_OP_SUCCESS if list is non-empty and item was copied into val.
 *          AWS_OP_ERR (AWS_ERROR_LIST_EMPTY) if list is empty.
 *
 * AWS_PRECONDITION(aws_array_list_is_valid(list));
 * AWS_PRECONDITION(val && AWS_MEM_IS_WRITABLE(val, list->item_size));
 * AWS_POSTCONDITION(aws_array_list_is_valid(list));
 */
int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val);

/* Validity predicate:
 * bool aws_array_list_is_valid(const struct aws_array_list *list):
 *   list != NULL
 *   list->item_size > 0
 *   (list->current_size == 0) || (list->data != NULL)
 *   list->current_size == list->item_size * MAX_ITEM_ALLOCATION_or_0
 *   list->length * list->item_size <= list->current_size
 */
