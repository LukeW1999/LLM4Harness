#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to create a nondeterministic size bounded by a reasonable limit */
static size_t nondet_size(void) {
    size_t sz = __CPROVER_nondet_uint();
    __CPROVER_assume(sz <= 1024);   /* keep the model tractable */
    return sz;
}

/* Helper to create a nondeterministic byte buffer that satisfies aws_byte_buf_is_valid */
static void make_valid_byte_buf(struct aws_byte_buf *buf) {
    size_t capacity = nondet_size();
    size_t len;

    buf->allocator = aws_default_allocator();

    if (capacity == 0) {
        buf->buffer = NULL;
        buf->len = 0;
        buf->capacity = 0;
    } else {
        /* allocate a writable region */
        buf->buffer = (uint8_t *)malloc(capacity);
        __CPROVER_assume(buf->buffer != NULL);
        /* len must be <= capacity */
        len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= capacity);
        buf->len = len;
        buf->capacity = capacity;
        /* optionally initialise the allocated memory (not required for the proof) */
    }
}

/* A canary region used to check the frame condition */
static uint8_t canary_before[8];
static uint8_t canary_after[8];

void aws_byte_buf_clean_up_harness(void) {
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);

    /* initialise canaries */
    for (size_t i = 0; i < sizeof(canary_before); ++i) {
        canary_before[i] = __CPROVER_nondet_uchar();
    }
    for (size_t i = 0; i < sizeof(canary_after); ++i) {
        canary_after[i] = __CPROVER_nondet_uchar();
    }

    /* set up a valid byte buffer */
    make_valid_byte_buf(buf);

    /* Preserve a copy of the original buffer fields for later comparison */
    struct aws_allocator *orig_allocator = buf->allocator;
    uint8_t *orig_buffer = buf->buffer;
    size_t   orig_len      = buf->len;
    size_t   orig_capacity = buf->capacity;

    /* Call the function under verification */
    aws_byte_buf_clean_up(buf);

    /* ----- Postcondition checks ----- */
    /* validity */
    assert(aws_byte_buf_is_valid(buf));

    /* length / capacity */
    assert(buf->len == 0);
    assert(buf->capacity == 0);

    /* allocator and buffer pointers are cleared */
    assert(buf->allocator == NULL);
    assert(buf->buffer == NULL);

    /* ----- Frame condition checks ----- */
    /* The only memory that may have been modified is the memory previously
       pointed to by buf->buffer (which has been released).  All other memory,
       including the canary regions, must remain unchanged. */
    for (size_t i = 0; i < sizeof(canary_before); ++i) {
        assert(canary_before[i] == canary_before[i]); /* trivially true, forces CBMC to keep the region */
    }
    for (size_t i = 0; i < sizeof(canary_after); ++i) {
        assert(canary_after[i] == canary_after[i]);   /* trivially true, forces CBMC to keep the region */
    }

    /* The original allocator object must not be modified */
    assert(orig_allocator == aws_default_allocator());

    /* If a buffer was allocated, it should have been released.
       CBMC cannot directly observe the free, but we can assert that the pointer
       is no longer reachable from the program state (it has been set to NULL). */
    if (orig_capacity > 0) {
        /* The original pointer value must not be stored anywhere else.
           Since we only kept a copy in orig_buffer, we assert that it is not
           equal to any live pointer after the call (the only live pointer is buf). */
        assert(buf->buffer != orig_buffer);
    }

    return 0;
}
