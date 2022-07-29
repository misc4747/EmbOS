#define BALL_WIDTH 5
#define BALL_HEIGHT 5

typedef int fix;

extern fix ball_get_dy(void);             // ボールのy方向の速度を返す．
extern fix ball_get_dx(void);             // ボールのx方向の速度を返す．
extern void draw_ball_coord();           // ボールのx, y座標を表示する．
extern void ball_set_dy(fix new_dy);      // ボールのy方向の速度をセットする．
extern void ball_set_dx(fix new_dx);     // ボールのx方向の速度をセットする．
extern struct box *ball_get_box(void);    // ボールの箱の位置を返す．
extern void ball_step(void);              // アニメーションの1ステップを行なう．
extern void ball_init(void);              // ボールの初期化を行なう．
extern void delay(int n);                 // nミリ秒待つ．
extern int ball_get_x(void);              // ボールのx座標を返す．
extern int ball_get_y(void);              // ボールのy座標を返す．

