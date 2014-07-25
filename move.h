//指し手に関する便利定数
//領域ごとのマスク。
enum {
  move_drop = 25,
  move_mask_type = (b1_1111 << 0),
  move_mask_from = (b1_1111 << 5),
  move_mask_to = (b1_1111 << 10),
  move_mask_promote = (b0001 << 15),
  move_mask_capture = (b0001 << 16),
  move_mask_captured = (b1_1111 << 17),
};

//指し手から情報を取得するマクロ
//上記のマスクを用いて取得している
//駒の種類，移動前・後，成ったか否か，駒を取ったか否か，取った駒の種類
//成ったか否か?成っているか否か？は要調査
#define MOVE_TYPE(move)           ( (move & move_mask_type)         )
#define MOVE_FROM(move)           ( (move & move_mask_from)    >> 5 )
#define MOVE_TO(move)             ( (move & move_mask_to)      >> 10)
#define MOVE_PROMOTE(move)        ( (move & move_mask_promote) >> 15)
#define MOVE_CAPTURE(move)        ( (move & move_mask_capture) >> 16)
#define MOVE_CAPTURED_TYPE(move)  ( (move & move_mask_captured)>> 17)
//NULLの指し手を表現
#define MOVE_NULL                 0

//捕獲した駒の種類を格納する位置に対応させるためのビットシフト
#define CAPTURED_2_MOVE(cap)      ( cap << 17 )

//ビットボード生成時に，それぞれ該当する桁にそろえるようにビットシフトする. 
//駒を取らない場合の指し手を表すビットボードを生成する
#define MOVE(v1, v2, v3, v4, v5) ( (v1) + (v2 << 5) + (v3 << 10) +\
                                   (v4 << 15) + (v5 << 16) + (0 << 17) )
//駒を取る場合の指し手を表すビット列を生成する．
#define MOVE_C(v1, v2, v3, v4, v5, v6)\
                                   ( (v1) + (v2 << 5) + (v3 << 10) +\
                                   (v4 << 15) + (v5 << 16) + (v6 << 17) )

