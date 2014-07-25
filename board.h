#include "utility.h"

//データ構造のための定数
#define RECORD_SIZE 1024
#define SIZE_LEGALMOVES 1024

//現局面の状態
typedef struct tree_t {
  unsigned int BBs[32]; //先手後手ごとの盤表現
  unsigned int whand; //先手の持ち駒
  unsigned int bhand; //後手の持ち駒
  int sq_wking; //先手の玉の位置
  int sq_bking; //後手の玉の位置
  char turn; //現在の手番
  short n_ply; //現在の手数
} tree;


typedef struct hist_t {
  unsigned int move;
} hist;


extern tree game;
extern hist history[RECORD_SIZE];
extern unsigned int Occupied0;
extern unsigned int nOccupiedW;
extern unsigned int nOccupiedB;

#define BB_ALL (game.BBs)
#define BB_N(n) (game.BBs[n])

#define BB_W_PAWN        (game.BBs[w_pawn])
#define BB_W_SILVER      (game.BBs[w_silver])
#define BB_W_GOLD        (game.BBs[w_gold])
#define BB_W_BISHOP      (game.BBs[w_bishop])
#define BB_W_ROOK        (game.BBs[w_rook])
#define BB_W_PRO_PAWN    (game.BBs[w_pro_pawn])
#define BB_W_LANCE       (game.BBs[w_lance])
#define BB_W_KNIGHT      (game.BBs[w_knight])
#define BB_W_KING        (game.BBs[w_king])

#define BB_B_PAWN        (game.BBs[b_pawn])
#define BB_B_SILVER      (game.BBs[b_silver])
#define BB_B_GOLD        (game.BBs[b_gold])
#define BB_B_BISHOP      (game.BBs[b_bishop])
#define BB_B_ROOK        (game.BBs[b_rook])
#define BB_B_PRO_PAWN    (game.BBs[b_pro_pawn])
#define BB_B_LANCE       (game.BBs[b_lance])
#define BB_B_KNIGHT      (game.BBs[b_knight])
#define BB_B_KING        (game.BBs[b_king])

#define W_HANDS (game.whand)
#define W_HAND_A (game.whand)
#define B_HANDS (game.bhand)
#define B_HAND_A (game.bhand)

//ある駒の数を取得
#define W_HAND(piece)     (int)((W_HANDS >> ((piece)*2)) & b0011)
#define B_HAND(piece)     (int)((B_HANDS >> ((piece)*2)) & b0011)

//ある駒を保持しているか判定
#define IsHand_W(piece) (W_HANDS & (b0011 << ((piece) * 2)))
#define IsHand_B(piece) (B_HANDS & (b0011 << ((piece) * 2)))

//ある駒を持ち駒に追加
#define HAND_ADD(piece)   (1 << ((piece)*2))

#define SQ_W_KING (game.sq_wking)
#define SQ_B_KING (game.sq_bking)

//手番
#define TURN (game.turn)
//手数
#define N_PLY (game.n_ply)

//指し手を反映させるマクロ
//現在の手番のプレイヤのmake_moveを呼び出す．
#define MAKE_MOVE(move)       TURN ? make_move_b( move ) : make_move_w( move );
#define UNMAKE_MOVE           TURN ? unmake_move_w() : unmake_move_b();

//手番を入れ替える．black^blackならwhiteに。white^blackならblackに
#define FLIP_TURN             TURN ^= black;

//starting_initialize()にてFILE_READされている．
extern unsigned int Attack_WPawn[32];
extern unsigned int Attack_WSilver[32];
extern unsigned int Attack_WGold[32];
extern const unsigned int Attack_WKnight[32];
extern unsigned int Attack_BPawn[32];
extern unsigned int Attack_BSilver[32];
extern unsigned int Attack_BGold[32];
extern const unsigned int Attack_BKnight[32];
extern unsigned int Attack_King[32];
extern unsigned int Attack_Rook[32][64];
extern unsigned int Attack_Rook_magic[32];
extern unsigned int Attack_Rook_mask[32];
extern const    int Attack_Rook_shift[32];
extern unsigned int Attack_Bishop[32][64];
extern unsigned int Attack_Bishop_magic[32];
extern unsigned int Attack_Bishop_mask[32];
extern const    int Attack_Bishop_shift[32];
extern const unsigned int Attack_WLance[32];
extern const unsigned int Attack_BLance[32];

extern unsigned int Pin_Rook[32][32][64][2];
extern unsigned int Pin_Bishop[32][32][64][2];
extern unsigned int Pin_WLance[32][32][64][2];
extern unsigned int Pin_BLance[32][32][64][2];

extern unsigned int DoublePawn[32];

//関数群
int starting_initialize(); //利き情報の初期化
void clear_game();
void clear_game_rand();
void calc_occupied_sq();

void make_move_w(unsigned int move);
void make_move_b(unsigned int move);
void unmake_move_w();
void unmake_move_b();


//指し手生成に関わる関数
int gen_legalmoves( unsigned legalmoves[] );

//利きまわりの便利関数
int attacks_to_w( int sq, unsigned int *attack_pieces );
int attacks_to_b( int sq, unsigned int *attack_pieces );
void pinInfo_w( unsigned int pin[32] );
void pinInfo_b( unsigned int pin[32] );

int gen_evasion_w( unsigned int moves[], int count, int nAttacks,
                   unsigned int attack_pieces, unsigned int pin[] );
int gen_evasion_b( unsigned int moves[], int count, int nAttacks,
                   unsigned int attack_pieces, unsigned int pin[] );
int gen_cap_w( unsigned int moves[], int count, unsigned int pin[] );
int gen_cap_b( unsigned int moves[], int count, unsigned int pin[] );
int gen_nocap_w( unsigned int moves[], int count, unsigned int pin[] );
int gen_nocap_b( unsigned int moves[], int count, unsigned int pin[] );
int gen_drop_w( unsigned int moves[], int count );
int gen_drop_b( unsigned int moves[], int count );
int gen_attacks_to_w( unsigned int moves[], int count, int sq, int cap, unsigned int pin[] );
int gen_attacks_to_b( unsigned int moves[], int count, int sq, int cap, unsigned int pin[] );

int get_piece_on_sq_w(int index);
int get_piece_on_sq_b(int index);

int FirstOne(int bb);

int popuCount( int piece );

//現在の手番(TURN)をblackかwhiteで返す
int get_turn();
//N_PLYを返す
int get_nply();
