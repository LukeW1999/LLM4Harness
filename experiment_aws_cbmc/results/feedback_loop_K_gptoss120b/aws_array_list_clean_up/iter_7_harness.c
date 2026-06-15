/*
Contract for aws_array_list_clean_up

Preconditions:
- __CPROVER_assume(list != NULL);
- __CPROVER_assume(list->alloc == NULL || list->alloc == aws_default_allocator());
- If list->alloc != NULL && list->data != NULL, then list->data
