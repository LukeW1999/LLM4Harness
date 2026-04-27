/* From: include/aws/common/array_list.h and array_list.inl */

struct aws_array_list {
    struct aws_allocator *alloc;
    size_t current_size;  /* total allocated bytes */
    size_t length;        /* number of elements */
    size_t item_size;     /* size of one element in bytes */
    void *data;           /* pointer to backing storage */
};


int aws_array_list_back(const struct aws_array_list *AWS_RESTRICT list, void *val);

/* Validity predicate:
 * bool aws_array_list_is_valid(const struct aws_array_list *list):
 *   list != NULL
 *   list->item_size > 0
 *   (list->current_size == 0) || (list->data != NULL)
 *   list->current_size == list->item_size * MAX_ITEM_ALLOCATION_or_0
 *   list->length * list->item_size <= list->current_size
 */
