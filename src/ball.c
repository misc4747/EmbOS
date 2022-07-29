#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"

fix    dx = 3 << 8;
fix    dy = 3 << 8;
int next_x_pos = 0;
int next_y_pos = 0;

box ball;

void draw_ball_coord(){
    int x = 0;
    int y = 152;
    erase_and_draw_char(COLOR_WHITE, 'X', x, y);
    erase_and_draw_char(COLOR_WHITE, ':', x+8, y);
    erase_and_draw_char(COLOR_WHITE, (ball.x / 100) % 10 + '0', x+16, y);
    erase_and_draw_char(COLOR_WHITE, (ball.x / 10) % 10 + '0', x+24, y);
    erase_and_draw_char(COLOR_WHITE, (ball.x % 10) + '0', x+32, y);

    erase_and_draw_char(COLOR_WHITE, 'Y', x+40, y);
    erase_and_draw_char(COLOR_WHITE, ':', x+48, y);
    erase_and_draw_char(COLOR_WHITE, (ball.y / 100) % 10 + '0', x+56, y);
    erase_and_draw_char(COLOR_WHITE, (ball.y / 10) % 10 + '0', x+64, y);
    erase_and_draw_char(COLOR_WHITE, (ball.y % 10) + '0', x+72, y);
}

void delay(int n){
    for(int i = 0; i < n; i++);
}

int round_fix(fix f) {
    return (f + 0x80) >> 8;
}

void draw_ball(box *b, int x, int y, hword color)
{
    hword   *base, *d;
    int     w, h;

    /* Base point is at (x, y). */
    base = (hword*)VRAM + LCD_WIDTH * y + x;

    /* Draw box from (x, y). */
    for (h = b->height; h > 0; h--) {
        d = base;
        for (w = b->width; w > 0; w--)
            *(d++) = color;
        base += LCD_WIDTH;
    }

    /* Set the current position. */
    b->x = x;
    b->y = y;
}

void move_ball(struct box *b, int x, int y, hword color){
    if(x > LCD_WIDTH - ball.width){
        dx = -dx;
        x += round_fix(dx);
    }
    if(y > LCD_HEIGHT - ball.height){
        game_set_state(DEAD);
        return;
    }
    if(x < 0){
        dx = -dx;
        x += round_fix(dx);
    }
    if(y < 0){
        dy = -dy;
        y += round_fix(dy);
    }
    draw_ball(b, b->x, b->y, COLOR_BLACK);
    draw_ball(b, x, y, color);        
}

int ball_init_step(){
    int key = gba_register(KEY_STATUS);
    if (! (key & KEY_LEFT)){
        if(ball.x > 0){
            move_ball(&ball, ball.x - 1, ball.y, COLOR_WHITE);
        } else {
            draw_ball(&ball, ball.x, ball.y, COLOR_WHITE);
        }
    } else if (! (key & KEY_RIGHT)){
        if(ball.x < LCD_WIDTH - ball.width){
            move_ball(&ball, ball.x + 1, ball.y, COLOR_WHITE);
        } else {
            draw_ball(&ball, ball.x, ball.y, COLOR_WHITE);
        }
    } else if (! (key & KEY_L)){
        if(ball.x < LCD_WIDTH - ball.width){
            move_ball(&ball, ball.x + 1, ball.y, COLOR_WHITE);
        } else {
            draw_ball(&ball, ball.x, ball.y, COLOR_WHITE);
        }
    } else if (! (key & KEY_R)){
        if(ball.x < LCD_WIDTH - ball.width){
            move_ball(&ball, ball.x + 1, ball.y, COLOR_WHITE);
        } else {
            draw_ball(&ball, ball.x, ball.y, COLOR_WHITE);
        }
    } else if (! (key & KEY_A)){
        return 1;
    } else {
        draw_ball(&ball, ball.x, ball.y, COLOR_WHITE);
    }
    old_key = key;
    return 0;
}

void ball_init(){
    ball = (box){
        .x = 1,
        .y = 80,
        .width = BALL_WIDTH,
        .height = BALL_HEIGHT
    };
    dx = 3 << 8;
    int flag = 0;
    while(flag == 0){
        flag = ball_init_step();
        for(int i = 0; i < 10000; i++);
    }
}

void ball_step(){
    switch (game_get_state()) {
    case START:
        ball_init();
        break;
    case RUNNING:
        next_x_pos = ball.x + round_fix(dx);
        next_y_pos = ball.y + round_fix(dy);
        move_ball(&ball, next_x_pos, next_y_pos, COLOR_WHITE);

        int box_return = box_step();
        int bound_x = box_return % 2;
        int bound_y = box_return / 2;

        if(bound_x > 0){
            ball_set_dx(-dx);
        } else if (bound_x < 0) {
            ball_set_dx(-dx);
        }
        if(bound_y > 0){
            ball_set_dy(-dy);
        } else if (bound_y < 0) {
            ball_set_dy(-dy);
        }
        break;
    case DEAD:
        break;
    case RESTART:
        draw_ball(&ball, ball.x, ball.y, COLOR_BLACK);
        next_x_pos = 0;
        next_y_pos = 0;
        ball_init();
        break;
    case CLEAR:
        break;
    case GAMEOVER:
        break;
    }
}

fix ball_get_dy(void){
    return dy;
}

fix ball_get_dx(void){
    return dx;
}

void ball_set_dy(fix new_dy){
    dy = new_dy;
}

void ball_set_dx(fix new_dx){
    dx = new_dx;
}

int ball_get_x(void){
    return ball.x;
}

int ball_get_y(void){
    return ball.y;
}

box *ball_get_box(void){
    return &ball;
}