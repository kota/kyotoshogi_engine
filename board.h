#include "utility.h"

//�f�[�^�\���̂��߂̒萔
#define RECORD_SIZE 1024
#define SIZE_LEGALMOVES 1024

//���ǖʂ̏��
typedef struct tree_t {
  unsigned int BBs[32]; //����育�Ƃ̔Օ\��
  unsigned int whand; //���̎�����
  unsigned int bhand; //���̎�����
  int sq_wking; //���̋ʂ̈ʒu
  int sq_bking; //���̋ʂ̈ʒu
  char turn; //���݂̎��
  short n_ply; //���݂̎萔
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

//�����̐����擾
#define W_HAND(piece)     (int)((W_HANDS >> ((piece)*2)) & b0011)
#define B_HAND(piece)     (int)((B_HANDS >> ((piece)*2)) & b0011)

//������ێ����Ă��邩����
#define IsHand_W(piece) (W_HANDS & (b0011 << ((piece) * 2)))
#define IsHand_B(piece) (B_HANDS & (b0011 << ((piece) * 2)))

//������������ɒǉ�
#define HAND_ADD(piece)   (1 << ((piece)*2))

#define SQ_W_KING (game.sq_wking)
#define SQ_B_KING (game.sq_bking)

//���
#define TURN (game.turn)
//�萔
#define N_PLY (game.n_ply)

//�w����𔽉f������}�N��
//���݂̎�Ԃ̃v���C����make_move���Ăяo���D
#define MAKE_MOVE(move)       TURN ? make_move_b( move ) : make_move_w( move );
#define UNMAKE_MOVE           TURN ? unmake_move_w() : unmake_move_b();

//��Ԃ����ւ���Dblack^black�Ȃ�white�ɁBwhite^black�Ȃ�black��
#define FLIP_TURN             TURN ^= black;

//starting_initialize()�ɂ�FILE_READ����Ă���D
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

//�֐��Q
int starting_initialize(); //�������̏�����
void clear_game();
void clear_game_rand();
void calc_occupied_sq();

void make_move_w(unsigned int move);
void make_move_b(unsigned int move);
void unmake_move_w();
void unmake_move_b();


//�w���萶���Ɋւ��֐�
int gen_legalmoves( unsigned legalmoves[] );

//�����܂��֗̕��֐�
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

//���݂̎��(TURN)��black��white�ŕԂ�
int get_turn();
//N_PLY��Ԃ�
int get_nply();
