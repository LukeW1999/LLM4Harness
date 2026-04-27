#ifndef AWS_COMMON_ARRAY_LIST_H
#define AWS_COMMON_ARRAY_LIST_H


AWS_COMMON_API
int aws_array_list_ensure_capacity(struct aws_array_list *AWS_RESTRICT list, size_t index);

/**
 * Copies the the memory pointed to by val into the array at index. If in dynamic mode, the size will grow by a factor
 * of two when the array is full. In static mode, AWS_ERROR_INVALID_INDEX will be raised if the index is past the bounds
 * of the array.
 */
AWS_STATIC_IMPL
int aws_array_list_set_at(struct aws_array_list *AWS_RESTRICT list, const void *val, size_t index);

/**
 * Swap elements at the specified indices, which must be within the bounds of the array.
 */
AWS_COMMON_API
void aws_array_list_swap(struct aws_array_list *AWS_RESTRICT list, size_t a, size_t b);

/**
 * Sort elements in the list in-place according to the comparator function.
 */
AWS_COMMON_API
void aws_array_list_sort(struct aws_array_list *AWS_RESTRICT list, aws_array_list_comparator_fn *compare_fn);

AWS_EXTERN_C_END
#ifndef AWS_NO_STATIC_IMPL
#    include <aws/common/array_list.inl>
#endif /* AWS_NO_STATIC_IMPL */

AWS_POP_SANE_WARNING_LEVEL

#endif /* AWS_COMMON_ARRAY_LIST_H */
