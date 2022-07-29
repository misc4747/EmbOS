#include "gba.h"
#include "racket.h"

void draw_score(int remain_life, int bound_count, int num_block){
    int score = calc_score(remain_life, bound_count, num_block);
    int digits;
    draw_string(COLOR_WHITE,"SCORE:", 80, 88);

    for(int i = 0; i < 4; i++){
        digits = score % 10;
        draw_char(COLOR_WHITE, digits + '0', 152 - i * 8, 88);
        score /= 10;
    }
}

int calc_score(int remain_life, int bound_count, int num_block){
    int score = 0;
    score += remain_life * 1000;
    score += (32 - num_block) * 100;
    score += (30 - bound_count) * 100;
    if(score < 0){
        return 0;
    } else {
        return score;
    }
}
