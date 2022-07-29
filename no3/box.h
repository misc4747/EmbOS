#define BLOCK_COLS 8
#define BLOCK_ROWS 4

extern void box_init();
extern int box_step();

extern int num_blocks;

typedef struct box{
    int x, y, width, height;
} box;
