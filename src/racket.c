#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"
#include "score.h"

#include "8x8.til"

// Interrupt handler
racket racket1;
int old_key;
int cnt;
int remain_life;
enum state game_state;

void draw_char(hword color, int code, int x, int y){
    hword   *ptr = (hword*)VRAM + y * LCD_WIDTH + x;
    hword   *p;
    int     i, j;
    unsigned char   *font = char8x8[code];

    for (i = 0; i < FONT_SIZE; i++) {
        p = ptr + LCD_WIDTH * i;
        for (j = FONT_SIZE - 1; j >= 0; j--, p++) {
            if (font[i] & (1 << j))
                *p = color;
        }
    }
}

void erase_and_draw_char(hword color, int code, int x, int y){
    draw_char(COLOR_BLACK, 219, x, y);
    draw_char(color, code, x, y);
}

void draw_string(hword color, char *str, int x, int y){
    int i;
    for(i = 0; str[i] != '\0'; i++){
        draw_char(color, str[i], x + i * FONT_SIZE, y);
    }
}

void draw_clear(int x, int y, int width, int height){
    hword   *base, *d;
    base = (hword*)VRAM + LCD_WIDTH * y + x;

    for (int h = height; h > 0; h--) {
        d = base;
        for (int w = width; w > 0; w--)
            *(d++) = COLOR_BLACK;
        base += LCD_WIDTH;
    }
}

void draw_bound_count(){
    int x = 0;
    int y = 152;
    draw_string(COLOR_WHITE, "COUNT:", x, y);
    erase_and_draw_char(COLOR_WHITE, cnt / 100 + '0', x + FONT_SIZE * 6, y);
    erase_and_draw_char(COLOR_WHITE, (cnt % 100) / 10 + '0', x + FONT_SIZE * 7, y);
    erase_and_draw_char(COLOR_WHITE, cnt % 10 + '0', x + FONT_SIZE * 8, y);
}

void draw_racket(struct racket *r, int x, int y, hword color){
    hword   *base, *d;
    int     w, h;

    /* Base point is at (x, y). */
    base = (hword*)VRAM + LCD_WIDTH * y + x;

    /* Draw box from (x, y). */
    for (h = r->height; h > 0; h--) {
        d = base;
        for (w = r->width; w > 0; w--)
            *(d++) = color;
        base += LCD_WIDTH;
    }

    /* Set the current position. */
    r->x = x;
    r->y = y;
}

void move_racket(struct racket *r, int x, int y, hword color){
    if(x < 0 || x > LCD_WIDTH - r->width)
        return;
    draw_racket(r, r->x, r->y, 0);
    draw_racket(r, x, y, color);        
}



void racket_init(){
    racket1 = (racket){
        .x = 100,
        .y = 135,
        .width = RACKET_WIDTH,
        .height = RACKET_HEIGHT
    };
    hword racket_color = COLOR_WHITE;
    draw_racket(&racket1, racket1.x, racket1.y, racket_color);
}

int cross(box *ball, racket *racket){
    if(ball->x + ball->width >= racket->x && ball->x <= racket->x + racket->width
        && ball->y + ball->height >= racket->y && ball->y <= racket->y + racket->height){
        return ball->x - racket->x + ball->width + 1;
    } else {
        return 0;
    }
}

enum state game_get_state(){
    return game_state;
}

void game_set_state(enum state state){
    game_state = state;
}

void wait_until_vblank(){
    while((gba_register(LCD_STATUS) & 1) == 0);
}

void wait_while_vblank(){
    while((gba_register(LCD_STATUS) & 1));
}

void racket_step(){
    int key = gba_register(KEY_STATUS);
    switch (game_get_state()) {
    case START:
        racket_init();
        break;
    case RUNNING:
        if (! (key & KEY_LEFT)){
            move_racket(&racket1, racket1.x - 10, racket1.y, COLOR_WHITE);
        } else if (! (key & KEY_RIGHT)){
            move_racket(&racket1, racket1.x + 10, racket1.y, COLOR_WHITE);
        } else {
            draw_racket(&racket1, racket1.x, racket1.y, COLOR_WHITE);
        }
        old_key = key;
        box *ball = ball_get_box();
        int cross_result = cross(ball, &racket1);
        if(cross_result){
            if(cross_result <= 10){
                // dx, dy = -4, -1
                fix new_dx = (4 << 8)*-1;
                fix new_dy = (1 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            } else if(11 <= cross_result && cross_result <= 19){
                // dx, dy = -3, -3
                fix new_dx = (3 << 8)*-1;
                fix new_dy = (3 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            } else if(20 <= cross_result && cross_result <= 28){
                // dx, dy = -1, -4
                fix new_dx = (1 << 8)*-1;
                fix new_dy = (4 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            } else if(29 <= cross_result && cross_result <= 37){
                // dx, dy = 1, -4
                fix new_dx = (1 << 8);
                fix new_dy = (4 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            } else if(38 <= cross_result && cross_result <= 46){
                // dx, dy = 3, -3
                fix new_dx = (3 << 8);
                fix new_dy = (3 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            } else if(47 <= cross_result){
                // dx, dy = 4, -1
                fix new_dx = (4 << 8);
                fix new_dy = (1 << 8)*-1;
                ball_set_dx(new_dx);
                ball_set_dy(new_dy);
            }
            cnt++;
        }
        break;
    case DEAD:
        break;
    case RESTART:
        break;
    case CLEAR:
        break;
    case GAMEOVER:
        break;
    }
}

int main(){
    /* Initialize LCD Control Register to use Mode 3. */
    gba_register(LCD_CTRL) = LCD_BG2EN | LCD_MODE3;
    game_set_state(START);
    remain_life = 3;
    cnt = 0;
    while(1){
        wait_until_vblank();
        int key = gba_register(KEY_STATUS);
        switch (game_get_state()) {
        case START:
            draw_string(COLOR_WHITE, "PRESS START BUTTON", 48, 100);
            if(! (key & KEY_START)){
                draw_clear(0, 0, LCD_WIDTH, LCD_HEIGHT);
                racket_step();
                ball_step();
                box_step();
                draw_string(COLOR_WHITE, "LIFE:", 184, 152);
                erase_and_draw_char(COLOR_WHITE, remain_life + '0', 224, 152);
                game_set_state(RUNNING);
            }
            break;
        case RUNNING:
            ball_step();
            racket_step();
            break;
        case DEAD:
            if(remain_life == 0){
                game_set_state(GAMEOVER);
            }
            if(! (key & KEY_START)){
                remain_life--;
                draw_clear(0, 80, LCD_WIDTH, 80);
                draw_string(COLOR_WHITE, "LIFE:", 184, 152);
                erase_and_draw_char(COLOR_WHITE, remain_life + '0', 224, 152);
                ball_init();
                racket_init();
                game_set_state(RUNNING);
            }
            break;
        case RESTART:
            remain_life = 3;
            cnt = 0;
            racket_step();
            ball_step();
            box_step();
            remain_life = LIFE;
            game_set_state(RUNNING);
            break;
        case CLEAR:
            draw_string(COLOR_WHITE, "CLEAR", 100, 80);
            draw_score(remain_life, cnt, num_blocks);
            draw_string(COLOR_WHITE, "PRESS START TO RESTART", 32, 100);
            if(! (key & KEY_START)){
                game_set_state(START);
            }
            break;
        case GAMEOVER:
            draw_string(COLOR_WHITE, "GAME OVER", 86, 80);
            draw_score(remain_life, cnt, num_blocks);
            if(! (key & KEY_START)){
                draw_clear(0, 0, LCD_WIDTH, LCD_HEIGHT);
                game_set_state(START);
            }
            break;
        }
        delay(10000);
        wait_while_vblank();
    }
}