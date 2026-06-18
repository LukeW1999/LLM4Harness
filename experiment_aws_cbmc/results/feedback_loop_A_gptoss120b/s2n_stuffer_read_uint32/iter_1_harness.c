#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Definitions from s2n headers (simplified for harness) */
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

/* Return codes used by s2n */
#define S2N_SUCCESS 0

/* External function used by the implementation */
extern int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer,
                                  uint8_t *buf,
                                  size_t len);

/* Function under test */
int s2n_stuffer_read_uint32(struct s2n_stuffer *stuffer, uint32_t *u);

/* Harness */
void s2n_stuffer_read_uint32_harness(void) {
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;
    /* blob fields */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= 1024);
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    stuffer.blob.growable = nondet_bool();
    stuffer.blob.data = NULL;
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    }
    /* cursor fields */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    /* flag fields */
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* 2. Allocate output pointer and initialise */
    uint32_t *u = malloc(sizeof(uint32_t));
    __CPROVER_assume(u != NULL);
    *u = nondet_uint32_t();

    /* Save old state */
    struct s2n_stuffer old = stuffer;
    uint32_t old_u = *u;

    /* Save the 4 bytes that would be read (if they exist) */
    uint8_t saved_bytes[4] = {0};
    if (old.read_cursor + 4 <= old.blob.size && old.blob.data != NULL) {
        for (size_t i = 0; i < 4; ++i) {
            saved_bytes[i] = old.blob.data[old.read_cursor + i];
        }
    }

    /* 3. Call the function under test */
    int result = s2n_stuffer_read_uint32(&stuffer, u);

    /* 4. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* The read must have succeeded, therefore enough bytes were available */
        assert(old.read_cursor + 4 <= old.blob.size);
        /* u must contain the big‑endian composition of the saved bytes */
        uint32_t expected = ((uint32_t)saved_bytes[0] << 24) |
                            ((uint32_t)saved_bytes[1] << 16) |
                            ((uint32_t)saved_bytes[2] << 8)  |
                            ((uint32_t)saved_bytes[3]);
        assert(*u == expected);
        /* read_cursor advances by 4 */
        assert(stuffer.read_cursor == old.read_cursor + 4);
        /* All other fields remain unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
    } else {
        /* On failure the stuffer must be unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        /* u must be unchanged */
        assert(*u == old_u);
    }

    /* 5. Clean up */
    if (stuffer.blob.data) {
        free(stuffer.blob.data);
    }
    free(u);
}
