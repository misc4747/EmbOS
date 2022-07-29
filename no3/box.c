#include "gba.h"
#include "ball.h"
#include "racket.h"
#include "box.h"

static struct box boxes[BLOCK_COLS][BLOCK_ROWS];
   // ブロックを画面上で表示するboxの二次元配列 
static char flags[BLOCK_COLS][BLOCK_ROWS];
   // ブロックが表示されているか、ボールが当たって消えているかの状態を表す二次元配列
   // (たとえば、表示中なら1, 消えていれば0)
int num_blocks;   // 残りブロック数
static int bound_x, bound_y;

void draw_box(box *b, hword color)
{
    hword   *base, *d;
    int     w, h;

    /* Base point is at (x, y). */
    base = (hword*)VRAM + LCD_WIDTH * b->y + b->x;

    /* Draw box from (x, y). */
    for (h = b->height; h > 0; h--) {
        d = base;
        for (w = b->width; w > 0; w--)
            *(d++) = color;
        base += LCD_WIDTH;
    }
}

void box_init(){
      int i, j;
      for(i = 0; i < BLOCK_COLS; i++){
         for(j = 0; j < BLOCK_ROWS; j++){
               boxes[i][j].x = 1 + (LCD_WIDTH / BLOCK_COLS) * i;
               boxes[i][j].y = 1 + 15 * j;
               boxes[i][j].width = 28;
               boxes[i][j].height = 13;
               flags[i][j] = 1;
               draw_box(&boxes[i][j], COLOR_WHITE);
         }
      }
      num_blocks = BLOCK_COLS * BLOCK_ROWS;
}

int hit(int x, int y){
   int i, j;
   if(x >= LCD_WIDTH || y >= LCD_HEIGHT)
      return -1;
   i = x / 30;
   j = y / 15;
   int block_num = j * BLOCK_COLS + i;
   if(block_num < BLOCK_COLS * BLOCK_ROWS && flags[block_num % BLOCK_COLS][block_num / BLOCK_COLS] == 1){
      return block_num;
   }else{
      return -1;
   }
}

int collision(int n){
   if(n != -1 && flags[n % BLOCK_COLS][n / BLOCK_COLS] == 1){
      return 1;
   }
   return 0;
}

void delete_block(int n){
   if(n != -1 && flags[n % BLOCK_COLS][n / BLOCK_COLS] == 1){
      draw_box(&boxes[n % BLOCK_COLS][n / BLOCK_COLS], COLOR_BLACK);
      flags[n % BLOCK_COLS][n / BLOCK_COLS] = 0;
      num_blocks--;
   }
}

int box_step(){
   bound_x = 0;
   bound_y = 0;
   switch (game_get_state()){
      case START:
         box_init();
         return 0;
         break;
      case RUNNING:
         int x, y;
         x = ball_get_x();
         y = ball_get_y();

         int hit_lu = hit(x, y);
         int hit_ru = hit(x + BALL_WIDTH, y);
         int hit_ld = hit(x, y + BALL_HEIGHT);
         int hit_rd = hit(x + BALL_WIDTH, y + BALL_HEIGHT);
         if(hit_lu != -1 || hit_ru != -1){
            bound_y++;
         }
         if(hit_ld != -1 || hit_rd != -1){
            bound_y--;
         }
         if(hit_lu != -1 || hit_ld != -1){
            bound_x++;
         }
         if(hit_ru != -1 || hit_rd != -1){
            bound_x--;
         }

         delete_block(hit_lu);
         delete_block(hit_ru);
         delete_block(hit_ld);
         delete_block(hit_rd);

         if(num_blocks == 0){
            draw_clear(0, 0, LCD_WIDTH, LCD_HEIGHT);
            game_set_state(CLEAR);
         }
         return bound_x + bound_y * 2;
         break;
      case DEAD:
         return 0;
         break;
      case RESTART:
         box_init();
         return 0;
         break;
      case CLEAR:
         return 0;
         break;
      case GAMEOVER:
         return 0;
         break;
   }
   return 0;
}