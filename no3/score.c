void draw_score(int score){
    int digits;
    for(int i = 0; i < SCORE_DIGITS; i++){
        digits = score % 10;
        score /= 10;
        draw_digit(i, digits);
    }
}

int calc_score(int remain_life, int bound_count, int num_block){
    int score = 0;
    score += remain_life * 1000;
    score += (32 - num_block) * 100;
    score += (30 - bound_count) * 100;
    if(score < 0){
        return 0
    } else {
        return score;
    }
}
