extern void racket_step(void);
extern void racket_init(void);

extern void draw_char(hword color, int code, int x, int y);
extern void erase_and_draw_char(hword color, int code, int x, int y);
extern void draw_clear(int x, int y, int width, int height);
extern void draw_string(hword color, char *str, int x, int y);

extern enum state game_get_state(void);
extern void game_set_state(enum state);

typedef struct racket{
    int x, y, width, height;
} racket;

extern int old_key;
extern int* waveptr;
extern int wavecnt;			// Wave data counter

#define FONT_SIZE       8
#define COLOR_WHITE     BGR(31, 31, 31)
#define COLOR_BLACK     0
#define RACKET_WIDTH    50
#define RACKET_HEIGHT   3
#define LIFE            3

enum state {START, RUNNING, DEAD, GAMEOVER, RESTART, CLEAR};