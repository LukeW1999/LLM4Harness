#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* Minimal s2n type definitions needed for the harness */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Forward declarations for s2n functions */
int s2n_stuffer_init(struct s2n_stuffer *stuffer, struct s2n_blob *in);

/* Non-deterministic value generators */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

/* Validity predicates - simplified for CBMC */
bool s2n_blob_is_valid(const struct s2n_blob *b) {
    if (b == NULL) return false;
    if (b->size > 0 && b->data == NULL) return false;
    if (b->allocated > 0 && b->data == NULL) return false;
    return true;
}

bool s2n_stuffer_is_valid(const struct s2n_stuffer *s) {
    if (s == NULL) return false;
    if (!s2n_blob_is_valid(&s->blob)) return false;
    if (s->read_cursor > s->write_cursor) return false;
    if (s->write_cursor > s->blob.size) return false;
    if (s->high_water_mark > s->blob.size) return false;
    return true;
}

void s2n_stuffer_init_harness(void) {
    /* 1. Declare and set up the stuffer */
    struct s2n_stuffer stuffer;
    
    /* 2. Declare and set up the blob */
    struct s2n_blob blob;
    blob.size = nondet_uint32_t();
    blob.allocated = nondet_uint32_t();
    blob.growable = nondet_bool() ? 1 : 0;
    
    /* Allocate data non-deterministically */
    if (blob.size > 0) {
        blob.data = malloc(blob.size);
        __CPROVER_assume(blob.data != NULL);
    } else {
        blob.data = nondet_bool() ? NULL : malloc(1);
    }
    
    /* Assume blob is valid as a precondition */
    __CPROVER_assume(s2n_blob_is_valid(&blob));
    
    /* 3. Save old blob state */
    struct s2n_blob old_blob = blob;
    
    /* 4. Call function under test */
    int result = s2n_stuffer_init(&stuffer, &blob);
    
    /* 5. Assert postconditions */
    if (result == 0) { /* S2N_SUCCESS = 0 */
        /* Changed fields: stuffer is initialized with blob data */
        assert(stuffer.blob.data == old_blob.data);
        assert(stuffer.blob.size == old_blob.size);
        assert(stuffer.blob.allocated == old_blob.allocated);
        assert(stuffer.blob.growable == old_blob.growable);
        
        /* Cursors reset to 0 */
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        
        /* Flags reset to 0 */
        assert(stuffer.alloced == 0);
        assert(stuffer.growable == 0);
        assert(stuffer.tainted == 0);
        
        /* Validity invariant */
        assert(s2n_stuffer_is_valid(&stuffer));
    } else {
        /* On failure, result should be -1 (S2N_FAILURE) */
        assert(result == -1);
    }
    
    /* Blob should not be modified by s2n_stuffer_init */
    assert(blob.data == old_blob.data);
    assert(blob.size == old_blob.size);
    assert(blob.allocated == old_blob.allocated);
    assert(blob.growable == old_blob.growable);
    
    return 0;
}
