#include "board.h"
#include "move.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

tree game;
hist history[RECORD_SIZE];

unsigned int Occupied0;
unsigned int nOccupiedW;
unsigned int nOccupiedB;

unsigned int Attack_WPawn[32];
unsigned int Attack_WSilver[32];
unsigned int Attack_WGold[32];
unsigned int Attack_BPawn[32];
unsigned int Attack_BSilver[32];
unsigned int Attack_BGold[32];
unsigned int Attack_King[32];
unsigned int Attack_Rook[32][64];
unsigned int Attack_Rook_magic[32];
unsigned int Attack_Rook_mask[32];
unsigned int Attack_Bishop[32][64];
unsigned int Attack_Bishop_magic[32];
unsigned int Attack_Bishop_mask[32];

const unsigned int Attack_WKnight[32] = {
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0x2, 0x5, 0xA, 0x14, 0x8,
  0x40, 0xA0, 0x140, 0x280, 0x100,
  0x800, 0x1400, 0x2800, 0x5000, 0x2000
};

const unsigned int Attack_BKnight[32] = {
  0x800, 0x1400, 0x2800, 0x5000, 0x2000,
  0x10000, 0x28000, 0x50000, 0xA0000, 0x40000,
  0x200000, 0x500000, 0xA00000, 0x1400000, 0x800000,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
};

const unsigned int Attack_WLance[32] = {
  0, 0, 0, 0, 0,
  0x1, 0x2, 0x4, 0x8, 0x10,
  0x21, 0x42, 0x84, 0x108, 0x210,
  0x421, 0x842, 0x1084, 0x2108, 0x4210,
  0x8421, 0x10842, 0x21084, 0x42108, 0x84210
};
const unsigned int Attack_BLance[32] = {
  0x108420, 0x210840, 0x421080, 0x842100, 0x1084200,
  0x108400, 0x210800, 0x421000, 0x842000, 0x1084000,
  0x108000, 0x210000, 0x420000, 0x840000, 0x1080000,
  0x100000, 0x200000, 0x400000, 0x800000, 0x1000000,
  0, 0, 0, 0, 0
};

unsigned int Pin_Rook[32][32][64][2];
unsigned int Pin_Bishop[32][32][64][2];
unsigned int Pin_Lance[32][32][64][2];
unsigned int DoublePawn[32];

const int Attack_Rook_shift[32] =
{
  19, 20, 20, 20, 19,
  20, 21, 21, 21, 20,
  20, 21, 21, 21, 20,
  20, 21, 21, 21, 20,
  19, 20, 20, 20, 19
};

const int Attack_Bishop_shift[32] =
  {
  22, 23, 23, 23, 22,
  23, 23, 23, 23, 23,
  23, 23, 21, 23, 23,
  23, 23, 23, 23, 23,
  22, 23, 23, 23, 22
};

const int Attack_WLance_shift[32] = 
{
  25, 25, 25, 25, 25,
  25, 25, 25, 25, 25,
  24, 24, 24, 24, 24,
  23, 23, 23, 23, 23,
  22, 22, 22, 22, 22
};

const int Attack_BLance_shift[32] =
{
  22, 22, 22, 22, 22,
  23, 23, 23, 23, 23,
  24, 24, 24, 24, 24,
  25, 25, 25, 25, 25,
  25, 25, 25, 25, 25
};

#define FILE_READ( variable, size ) \
  if( fread( variable, sizeof( unsigned int ), size, fp) != size ){ \
    printf(" fread \"%s\" failed.\n", #variable); \
    fclose( fp ); return -1; }\

int starting_initialize()
{
  /*
    return -1: failed
            0: succeeded
   */
  
  FILE *fp;

  fp = fopen("BB_Attack.bin","rb");

  FILE_READ( Attack_WPawn, 32 );
  FILE_READ( Attack_WSilver, 32 );
  FILE_READ( Attack_WGold, 32 );
  FILE_READ( Attack_BPawn, 32 );
  FILE_READ( Attack_BSilver, 32 );
  FILE_READ( Attack_BGold, 32 );
  FILE_READ( Attack_King, 32 );
  FILE_READ( Attack_Rook, 32 * 64 );
  FILE_READ( Attack_Rook_magic, 32 );
  FILE_READ( Attack_Rook_mask, 32 );
  FILE_READ( Attack_Bishop, 32 * 64 );
  FILE_READ( Attack_Bishop_magic, 32 );
  FILE_READ( Attack_Bishop_mask, 32 );
  FILE_READ( DoublePawn, 32 );
  FILE_READ( Pin_Rook, 32 * 32 * 64 * 2 );
  FILE_READ( Pin_Bishop, 32 * 32 * 64 * 2 );
  
  fclose( fp );

  return 0;
}

void clear_game() {
  //手番と手数の初期化
  TURN = white; N_PLY = 0;
  //持ち駒の初期化
  W_HANDS = 0; B_HANDS = 0;
  //盤の初期化
  memset(BB_ALL, 0, sizeof(BB_ALL));
  BB_N(w_pro_pawn) = Bit(XY2INDEX(5, 5));
  BB_N(w_silver) = Bit(XY2INDEX(4, 5));
  BB_N(w_king) = Bit(XY2INDEX(3, 5));
  BB_N(w_gold) = Bit(XY2INDEX(2, 5));
  BB_N(w_pawn) = Bit(XY2INDEX(1, 5));
  SQ_W_KING = XY2INDEX(3, 5);

  BB_N(b_pro_pawn) = Bit(XY2INDEX(1, 1));
  BB_N(b_silver) = Bit(XY2INDEX(2, 1));
  BB_N(b_king) = Bit(XY2INDEX(3, 1));
  BB_N(b_gold) = Bit(XY2INDEX(4, 1));
  BB_N(b_pawn) = Bit(XY2INDEX(5, 1));
  SQ_B_KING = XY2INDEX(3, 1);
  
  calc_occupied_sq();
}

void clear_game_rand() {
  int worder[5] = {1, 2, 3, 4, 5};
  int border[5] = {5, 4, 3, 2, 1};
  int i, from, to, tmp;
  //手番と手数の初期化
  TURN = white; N_PLY = 0;
  //持ち駒の初期化
  W_HANDS = 0; B_HANDS = 0;

  //盤の初期化
  memset(BB_ALL, 0, sizeof(BB_ALL));

  //順番の入れ替え
  //対称性も無くす
  for(i=0; i<10; ++i) {
    from = rand()%5;

    to = rand()%5;
    tmp = worder[from];
    worder[from] = worder[to];
    worder[to] = tmp;

    to = rand()%5;
    tmp = border[from];
    border[from] = border[to];
    border[to] = tmp;
  }

  //先手
  BB_N(w_pro_pawn) = Bit(XY2INDEX(worder[0], 5));
  BB_N(w_silver) = Bit(XY2INDEX(worder[1], 5));
  BB_N(w_king) = Bit(XY2INDEX(worder[2], 5));
  BB_N(w_gold) = Bit(XY2INDEX(worder[3], 5));
  BB_N(w_pawn) = Bit(XY2INDEX(worder[4], 5));
  SQ_W_KING = XY2INDEX(worder[2], 5);
  //後手
  BB_N(b_pro_pawn) = Bit(XY2INDEX(border[0], 1));
  BB_N(b_silver) = Bit(XY2INDEX(border[1], 1));
  BB_N(b_king) = Bit(XY2INDEX(border[2], 1));
  BB_N(b_gold) = Bit(XY2INDEX(border[3], 1));
  BB_N(b_pawn) = Bit(XY2INDEX(border[4], 1));
  SQ_B_KING = XY2INDEX(border[2], 1);
  
  calc_occupied_sq();  
}


void calc_occupied_sq()
{  
  int i,index, tmp;
  
  nOccupiedW = bb_mask;
  nOccupiedB = bb_mask;
  Occupied0  = 0;

  for( i=0; i < bbs_size; i++ )
    {
      if( i == w_king )
        { nOccupiedW &= ~Bit( SQ_W_KING );
          Occupied0   ^= Bit( SQ_W_KING );
          continue;                       }
      else if(i == b_king )
        { nOccupiedB &= ~Bit( SQ_B_KING );
          Occupied0   ^= Bit( SQ_B_KING );
          continue;                       }
      tmp = BB_N(i);
      while( (index = FirstOne( tmp ) )  >= 0 )
        {
          if( i < (bbs_size / 2) )
            nOccupiedW &= ~Bit(index);
          else
            nOccupiedB &= ~Bit(index);
          Occupied0   ^= Bit( index );
          tmp ^= Bit(index);
        }
    }

  return;
}


void make_move_w(unsigned int move) {
  const int from    = MOVE_FROM( move );
  const int to      = MOVE_TO( move );
  const int type    = MOVE_TYPE( move );
  //  const int promote = move & move_mask_promote;
  const int cap     = move & move_mask_capture;
  int cap_type = 0;

  assert( type < 16 );

  //持ち駒を打つ。
  //どちらの駒でも打てる。持ち駒はなる前としてカウントしている
  if( from == move_drop ) {
    W_HANDS -= HAND_ADD( type & mask_nopro );
    BB_N( type ) |= Bit( to );
    
    nOccupiedW ^= Bit( to );
    Occupied0  ^= Bit( to );

    history[N_PLY].move = move;
    FLIP_TURN;
    N_PLY ++;
    return;
  }

  //反転しない玉の処理
  if(type == w_king) {
    if( cap ) {
      //取った駒は、なる前として持っておく
      cap_type = get_piece_on_sq_b( to );
      assert( cap_type != no_piece && cap_type != b_king );

      BB_N( type ) ^= Bit( from ) | Bit( to );
      BB_N( cap_type ) ^= Bit( to );

      W_HANDS += HAND_ADD( cap_type & mask_nopro );      
      nOccupiedW ^= Bit( from ) | Bit( to );
      nOccupiedB ^= Bit( to );
      Occupied0  ^= Bit( from );      
    }else {
      BB_N( type ) ^= Bit( from ) | Bit( to );
      
      nOccupiedW ^= Bit( from ) | Bit( to );
      Occupied0  ^= Bit( from ) | Bit( to );
    }
    
    SQ_W_KING = FirstOne( BB_N(w_king) );
    assert( SQ_W_KING >= 0 );
    history[N_PLY].move =
    ( move & ~move_mask_captured ) + CAPTURED_2_MOVE( cap_type );

    FLIP_TURN;
    N_PLY ++;
    return;
  }

  if( cap ) { //相手の駒を取る手
    //取った駒は、なる前として持っておく
    cap_type = get_piece_on_sq_b( to );
    assert( cap_type != no_piece && cap_type != b_king );
    
    //移動前の駒を消す
    if( type < m_promote) BB_N( type + m_promote ) ^= Bit( from );
    else BB_N( type - m_promote ) ^= Bit( from );
    //移動後の駒
    BB_N( type ) ^= Bit( to );
    //持ち駒
    BB_N( cap_type ) ^= Bit( to );
    W_HANDS += HAND_ADD( cap_type & mask_nopro );

    nOccupiedW ^= Bit( from ) | Bit( to );
    nOccupiedB ^= Bit( to );
    Occupied0  ^= Bit( from );
  } else {
    //相手の駒を取らない手
    //移動前
    if( type < m_promote ) {
      BB_N( type + m_promote ) ^= Bit( from );
    }
    else {
      BB_N( type - m_promote ) ^= Bit( from );
    }
    //移動後
    BB_N( type ) ^= Bit( to );

    nOccupiedW ^= Bit( from ) | Bit( to );
    Occupied0  ^= Bit( from ) | Bit( to );
  }

  history[N_PLY].move =
    ( move & ~move_mask_captured ) + CAPTURED_2_MOVE( cap_type );

  FLIP_TURN;
  N_PLY ++;
  return;
}

void make_move_b(unsigned int move) {
  const int from    = MOVE_FROM( move );
  const int to      = MOVE_TO( move );
  const int type    = MOVE_TYPE( move );
  const int cap     = move & move_mask_capture;
  int cap_type = 0;
  
  assert( type >= 16 );

  //持ち駒
  if( from == move_drop ) {
    B_HANDS -= HAND_ADD( type & mask_nopro );
    BB_N( type ) |= Bit( to );
    
    nOccupiedB  ^= Bit( to );
    Occupied0   ^= Bit( to );
    
    history[N_PLY].move = move;
    FLIP_TURN;
    N_PLY ++;
    return;
  }

  if(type == b_king) {
    if( cap ) {
      //取った駒は、なる前として持っておく
      cap_type = get_piece_on_sq_w( to );
      assert( cap_type != no_piece && cap_type != w_king );
      
      BB_N( type ) ^= Bit( from ) | Bit( to );
      BB_N( cap_type ) ^= Bit( to );

      B_HANDS += HAND_ADD( cap_type & mask_nopro );
      nOccupiedB ^= Bit( from ) | Bit( to );
      nOccupiedW ^= Bit( to );
      Occupied0  ^= Bit( from );      
    }else {
      BB_N( type ) ^= Bit( from ) | Bit( to );
      
      nOccupiedB ^= Bit( from ) | Bit( to );
      Occupied0  ^= Bit( from ) | Bit( to );
    }
    
    SQ_B_KING = FirstOne( BB_N(b_king) );
    assert( SQ_B_KING >= 0 );
    history[N_PLY].move =
    ( move & ~move_mask_captured ) + CAPTURED_2_MOVE( cap_type );

    FLIP_TURN;
    N_PLY ++;
    return;
  }

  if( cap ) {
    cap_type = get_piece_on_sq_w( to );
    
    assert( cap_type != no_piece && cap_type != w_king );
    
    if( type - m_black < m_promote) { //表に戻る
      BB_N( type + m_promote ) ^= Bit( from );
      BB_N( type ) ^= Bit( to );
    } else { //裏返る
      BB_N( type - m_promote ) ^= Bit( from );
      BB_N( type ) ^= Bit( to );
    }
    //共通処理
    BB_N( cap_type ) ^= Bit( to );
    B_HANDS += HAND_ADD( cap_type & mask_nopro );
    nOccupiedB ^= Bit( from ) | Bit( to );
    nOccupiedW ^= Bit( to );
    Occupied0  ^= Bit( from );
  } else {
    //相手の駒を取らない手
    if( type - m_black < m_promote ) { //表に戻る
      BB_N( type + m_promote ) ^= Bit( from );
      BB_N( type ) ^= Bit( to );
    } else { //裏返る
      BB_N( type - m_promote ) ^= Bit( from );
      BB_N( type ) ^= Bit( to );
    }
    //共通処理
    nOccupiedB ^= Bit( from ) | Bit( to );
    Occupied0  ^= Bit( from ) | Bit( to );
  }

  history[N_PLY].move =
    ( move & ~move_mask_captured ) + CAPTURED_2_MOVE( cap_type );

  FLIP_TURN;
  N_PLY ++;
  return;
}

void unmake_move_w() {
  FLIP_TURN;
  N_PLY --;
  int move = history[N_PLY].move;
  
  const int from     = MOVE_FROM( move );
  const int to       = MOVE_TO( move );
  const int type     = MOVE_TYPE( move );
  const int cap_type = MOVE_CAPTURED_TYPE( move );

  if( from == move_drop ) {
    W_HAND_A += HAND_ADD( type & mask_nopro);
    BB_N( type ) ^= Bit( to );
    
    nOccupiedW ^= Bit( to );
    Occupied0  ^= Bit( to );
    return;
  }

  if(type == w_king) {
    BB_N( type ) ^= Bit( from ) | Bit( to );
    nOccupiedW ^= Bit( from ) | Bit( to );
    if(cap_type) {
      BB_N( cap_type ) ^= Bit( to );
      W_HAND_A -= HAND_ADD(cap_type & mask_nopro);
      nOccupiedB ^= Bit( to );

      Occupied0  ^= Bit( from );      
    }else {
      Occupied0  ^= Bit( from ) | Bit( to );
    }
    SQ_W_KING = FirstOne( BB_N(w_king) );
    return;
  }
  
  if( cap_type ) {
    //移動前の駒を出現
    if(type < m_promote) BB_N( type + m_promote) ^= Bit( from );
    else BB_N( type - m_promote) ^= Bit( from );
    //移動後の駒を消去
    BB_N( type ) ^= Bit( to );
    //持ち駒
    BB_N( cap_type ) ^= Bit( to );
    W_HAND_A -= HAND_ADD( cap_type & mask_nopro);

    nOccupiedW ^= Bit( from ) | Bit( to );
    nOccupiedB ^= Bit( to );
    Occupied0  ^= Bit( from );

  } else {
    //移動前の駒を出現
    if(type < m_promote) BB_N( type + m_promote) ^= Bit( from );
    else BB_N( type - m_promote) ^= Bit( from );
    //移動後の駒を消去
    BB_N( type ) ^= Bit( to );

    nOccupiedW ^= Bit( from ) | Bit( to );
    Occupied0  ^= Bit( from ) | Bit( to );
  }
}

void unmake_move_b() {
  FLIP_TURN;
  N_PLY --;
  int move = history[N_PLY].move;
  
  const int from     = MOVE_FROM( move );
  const int to       = MOVE_TO( move );
  const int type     = MOVE_TYPE( move );
  const int cap_type = MOVE_CAPTURED_TYPE( move );

  if( from == move_drop ) {
    B_HAND_A += HAND_ADD(type & mask_nopro);
    BB_N( type ) ^= Bit( to );
    
    nOccupiedB ^= Bit( to );
    Occupied0  ^= Bit( to );
    return;
  }

  if(type == b_king) {
    BB_N( type ) ^= Bit( from ) | Bit( to );
    nOccupiedB ^= Bit( from ) | Bit( to );
    if(cap_type) {
      BB_N( cap_type ) ^= Bit( to );
      B_HAND_A -= HAND_ADD( (cap_type & mask_nopro) );
      nOccupiedW ^= Bit( to );

      Occupied0  ^= Bit( from );      
    }else {
      Occupied0  ^= Bit( from ) | Bit( to );
    }
    SQ_B_KING = FirstOne( BB_N(b_king) );
    return;
  }
  if( cap_type ) {
    //移動前の駒を出現
    if(type-m_black < m_promote) {
      BB_N( type + m_promote) ^= Bit( from );
    }
    else {
      BB_N( type - m_promote) ^= Bit( from );
    }
    //移動後の駒を消去
    BB_N( type ) ^= Bit( to );
    //持ち駒
    BB_N( cap_type ) ^= Bit( to );
    B_HAND_A -= HAND_ADD( cap_type & mask_nopro );

    nOccupiedB ^= Bit( from ) | Bit( to );
    nOccupiedW ^= Bit( to );
    Occupied0  ^= Bit( from );

  } else {
    //移動前の駒を出現
    if(type-m_black < m_promote) {
      BB_N( (type + m_promote) | 0x10) ^= Bit( from );
    } else {
      BB_N( type - m_promote) ^= Bit( from );
    }
    //移動後の駒を消去
    BB_N( type ) ^= Bit( to );

    nOccupiedB ^= Bit( from ) | Bit( to );
    Occupied0  ^= Bit( from ) | Bit( to );
  }
  return;
}

int gen_legalmoves( unsigned moves[] ) {
  unsigned int pin[32], attack_on_king;
  unsigned int att;
  int nmove = 0;
  
  //後手番の合法手生成
  if( TURN ) {
    pinInfo_b( pin );
    //王手回避
    attack_on_king = attacks_to_w( SQ_B_KING, &att );
    if( attack_on_king > 0 ){
      return gen_evasion_b( moves, 0, attack_on_king, att, pin);
    }
    nmove = gen_cap_b( moves, nmove, pin);
    nmove = gen_drop_b( moves, nmove);
    nmove = gen_nocap_b( moves, nmove, pin);
    return nmove;
  }
  pinInfo_w( pin );
  //王手回避
  attack_on_king = attacks_to_b( SQ_W_KING, &att );
  if( attack_on_king > 0 ) {
    return gen_evasion_w( moves, 0, attack_on_king, att, pin );
  }
  nmove = gen_cap_w( moves, nmove, pin );
  nmove = gen_drop_w( moves, nmove );
  nmove = gen_nocap_w( moves, nmove, pin );
  
  return nmove;
}

int gen_evasion_w( unsigned int moves[], int count, int nAttacks,
                   unsigned int attack_pieces, unsigned int pin[]  )
{
  unsigned int dests, att;
  int to, cap, index;
  unsigned int type;

  dests = Attack_King[ SQ_W_KING ] & nOccupiedW;

  Occupied0   ^= Bit( SQ_W_KING );      /* assume that the king is absent */
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_b( to, &att ) == 0 )
        {
          cap = ( Bit( to ) & (~nOccupiedB) ) ? 1 : 0 ;
	  if(cap == 1) {
	    type = get_piece_on_sq_b(to);
	    moves[ count ] = MOVE_C( w_king, SQ_W_KING, to, 0, 1, type);
	  }else {
	    moves[ count ] = MOVE_C( w_king, SQ_W_KING, to, 0, 0, 0);
	  }
          count ++;
        }
      dests ^= Bit( to );
    }
  Occupied0   ^= Bit( SQ_W_KING );
  
  if( nAttacks >= 2 )
    { return count; }


  int sq = FirstOne( attack_pieces );
  assert( sq >= 0 );
  count = gen_attacks_to_w( moves, count, sq, 1, pin );


#define DROP( piece ) \
  if( IsHand_W( piece & mask_nopro ) ){ \
    moves[ count ] = MOVE( w_ ## piece, move_drop, to, 0, 0 ); \
    count ++;}

  if( attack_pieces & ( Attack_King[ SQ_W_KING ] ) )
    {
      /* nothing to do */
    }
  else if( attack_pieces &  BB_B_ROOK )
    {
      index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
               >> Attack_Rook_shift[ sq ]) & mask_magic;
      dests  = Attack_Rook[ sq ][ index ];

      index  = (( (Occupied0 & Attack_Rook_mask[ SQ_W_KING ])
                * Attack_Rook_magic[ SQ_W_KING ])
               >> Attack_Rook_shift[ SQ_W_KING ]) & mask_magic;
      dests &= Attack_Rook[ SQ_W_KING ][ index ];

      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( W_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }
          count = gen_attacks_to_w( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
  else if( attack_pieces &  BB_B_BISHOP )
    {
      index  = (( (Occupied0 & Attack_Bishop_mask[sq])
                  * Attack_Bishop_magic[sq])
                >> Attack_Bishop_shift[ sq ]) & mask_magic;
      dests  = Attack_Bishop[ sq ][ index ];

      index  = (( (Occupied0 & Attack_Bishop_mask[ SQ_W_KING ])
                * Attack_Bishop_magic[ SQ_W_KING ])
               >> Attack_Bishop_shift[ SQ_W_KING ]) & mask_magic;
      dests &= Attack_Bishop[ SQ_W_KING ][ index ];
      
      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( W_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }          
          count = gen_attacks_to_w( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
  else if( attack_pieces &  BB_B_LANCE )
    {
      index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
               >> Attack_Rook_shift[ sq ]) & mask_magic;
      dests  = Attack_Rook[ sq ][ index ] & Attack_BLance[sq];

      index  = (( (Occupied0 & Attack_Rook_mask[ SQ_W_KING ])
                * Attack_Rook_magic[ SQ_W_KING ])
               >> Attack_Rook_shift[ SQ_W_KING ]) & mask_magic;
      dests &= Attack_Rook[ SQ_W_KING ][ index ] & Attack_WLance[SQ_W_KING];

      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( W_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }
          count = gen_attacks_to_w( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
#undef DROP
  
  return count;
}

int gen_evasion_b( unsigned int moves[], int count, int nAttacks,
                   unsigned int attack_pieces, unsigned int pin[]  )
{
  unsigned int dests, att;
  int to, cap, index;
  unsigned int type;

  dests = Attack_King[ SQ_B_KING ] & nOccupiedB;

  Occupied0   ^= Bit( SQ_B_KING );      /* assume that the king is absent */
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_w( to, &att ) == 0 )
        {
          cap = ( Bit( to ) & (~nOccupiedW) ) ? 1 : 0 ;
	  if(cap == 1) {
	    type = get_piece_on_sq_w(to);
	    moves[ count ] = MOVE_C( b_king, SQ_B_KING, to, 0, 1, type);
	  }else {
	    moves[ count ] = MOVE_C( b_king, SQ_B_KING, to, 0, 0, 0);
	  }
          count ++;
        }
      dests ^= Bit( to );
    }
  Occupied0   ^= Bit( SQ_B_KING );
  
  if( nAttacks >= 2 )
    { return count; }


  int sq = FirstOne( attack_pieces );
  assert( sq >= 0 );
  count = gen_attacks_to_b( moves, count, sq, 1, pin );


#define DROP( piece ) \
  if( IsHand_B( piece & mask_nopro ) ){ \
    moves[ count ] = MOVE( b_ ## piece, move_drop, to, 0, 0 ); \
    count ++;}

  if( attack_pieces & ( Attack_King[ SQ_B_KING ] ) )
    {
      /* nothing to do */
    }
  else if( attack_pieces &  BB_W_ROOK )
    {
      index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
               >> Attack_Rook_shift[ sq ]) & mask_magic;
      dests  = Attack_Rook[ sq ][ index ];

      index  = (( (Occupied0 & Attack_Rook_mask[ SQ_B_KING ])
                * Attack_Rook_magic[ SQ_B_KING ])
               >> Attack_Rook_shift[ SQ_B_KING ]) & mask_magic;
      dests &= Attack_Rook[ SQ_B_KING ][ index ];

      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( B_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }
          count = gen_attacks_to_b( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
  else if( attack_pieces &  BB_W_BISHOP )
    {
      index  = (( (Occupied0 & Attack_Bishop_mask[sq])
                  * Attack_Bishop_magic[sq])
                >> Attack_Bishop_shift[ sq ]) & mask_magic;
      dests  = Attack_Bishop[ sq ][ index ];

      index  = (( (Occupied0 & Attack_Bishop_mask[ SQ_B_KING ])
                * Attack_Bishop_magic[ SQ_B_KING ])
               >> Attack_Bishop_shift[ SQ_B_KING ]) & mask_magic;
      dests &= Attack_Bishop[ SQ_B_KING ][ index ];
      
      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( B_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }          
          count = gen_attacks_to_b( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
  else if( attack_pieces &  BB_W_LANCE )
    {
      index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
               >> Attack_Rook_shift[ sq ]) & mask_magic;
      dests  = Attack_Rook[ sq ][ index ] & Attack_WLance[sq];

      index  = (( (Occupied0 & Attack_Rook_mask[ SQ_B_KING ])
                * Attack_Rook_magic[ SQ_B_KING ])
               >> Attack_Rook_shift[ SQ_B_KING ]) & mask_magic;
      dests &= Attack_Rook[ SQ_B_KING ][ index ] & Attack_BLance[SQ_B_KING];

      assert( dests );

      while( (to = FirstOne( dests )) != -1 )
        {
          if( B_HAND_A )
            {
              DROP( silver );
              DROP( gold );
              DROP( bishop );
              DROP( rook );
	      DROP( pro_pawn );
	      DROP( lance );
	      DROP( knight );
	      DROP( pawn );
            }
          count = gen_attacks_to_b( moves, count, to, 0, pin );

          dests ^= Bit( to );
        }
    }
#undef DROP
  
  return count;
}

#define GEN_ATTACK( piece ) \
  while( ( from = FirstOne( dests ) ) != -1){ \
    if( ~pin[ from ] & Bit( sq ) ){ \
      if(cap == 1) { \
	moves[ count ] = MOVE_C( piece, from, sq, 0, cap, get_piece_on_sq_b(sq) ); \
      } else {						 \
	moves[ count ] = MOVE( piece, from, sq, 0, cap); \
      }							 \
      count ++;					         \
    }							 \
    dests ^= Bit( from ); }

int gen_attacks_to_w( unsigned int moves[], int count, int sq, int cap, unsigned int pin[] )
{
  /* King is not considered. */
  unsigned int dests;
  int from, index;

  dests  = Attack_BPawn[ sq ] & BB_W_PAWN;
  GEN_ATTACK( w_rook);

  dests  = Attack_BSilver[ sq ] & BB_W_SILVER;
  GEN_ATTACK( w_bishop);
  
  dests  = Attack_BGold[ sq ] & BB_W_GOLD;
  GEN_ATTACK( w_knight);

  dests  = Attack_BGold[ sq ] & BB_W_PRO_PAWN;
  GEN_ATTACK( w_lance);

  dests = Attack_BKnight[ sq ] & BB_W_KNIGHT;
  GEN_ATTACK( w_gold);

  //角
  index =(((Occupied0 & Attack_Bishop_mask[sq])
           * Attack_Bishop_magic[sq])
          >> Attack_Bishop_shift[sq]) & mask_magic;
  dests = Attack_Bishop[ sq ][ index ];
  dests &= BB_W_BISHOP;
  GEN_ATTACK( w_silver );
  //飛車
  index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  dests  = Attack_Rook[ sq ][ index ];
  dests &= BB_W_ROOK;
  GEN_ATTACK( w_pawn );
  //香車
  index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  dests  = Attack_Rook[ sq ][ index ] & Attack_BLance[ sq ];
  dests &= BB_W_LANCE;
  GEN_ATTACK( w_pro_pawn );

  return count;
}
#undef GEN_ATTACK

#define GEN_ATTACK( piece ) \
  while( ( from = FirstOne( dests ) ) != -1){ \
    if( ~pin[ from ] & Bit( sq ) ){ \
      if(cap == 1) { \
	moves[ count ] = MOVE_C( piece, from, sq, 0, cap, get_piece_on_sq_w(sq) ); \
      } else {						 \
	moves[ count ] = MOVE( piece, from, sq, 0, cap); \
      }							 \
	count ++;					 \
    }							 \
    dests ^= Bit( from ); }

int gen_attacks_to_b( unsigned int moves[], int count, int sq, int cap, unsigned int pin[] )
{
  /* King is not considered. */
  unsigned int dests;
  int from, index;

  dests  = Attack_WPawn[ sq ] & BB_B_PAWN;
  GEN_ATTACK( b_rook);

  dests  = Attack_WSilver[ sq ] & BB_B_SILVER;
  GEN_ATTACK( b_bishop);
  
  dests  = Attack_WGold[ sq ] & BB_B_GOLD;
  GEN_ATTACK( b_knight);

  dests  = Attack_WGold[ sq ] & BB_B_PRO_PAWN;
  GEN_ATTACK( b_lance);

  dests = Attack_WKnight[ sq ] & BB_B_KNIGHT;
  GEN_ATTACK( b_gold );

  //角
  index =(((Occupied0 & Attack_Bishop_mask[sq])
           * Attack_Bishop_magic[sq])
          >> Attack_Bishop_shift[sq]) & mask_magic;
  dests = Attack_Bishop[ sq ][ index ];
  dests &= BB_B_BISHOP;
  GEN_ATTACK( b_silver );
  //飛車
  index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  dests  = Attack_Rook[ sq ][ index ];
  dests &= BB_B_ROOK;
  GEN_ATTACK( b_pawn );
  //香車
  index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  dests  = Attack_Rook[ sq ][ index ] & Attack_WLance[ sq ];
  dests &= BB_B_LANCE;
  GEN_ATTACK( b_pro_pawn );

  return count;
}
#undef GEN_ATTACK


//利きまわりの便利関数
int attacks_to_w( int sq, unsigned int *attack_pieces ) {
  int count = 0, from, index;
  unsigned int att = 0;
  //sqからある駒の利きの位置にその駒があれば、sqにその駒がある
  att  = Attack_BPawn[ sq ] & BB_N(w_pawn);
  att |= Attack_BSilver[ sq ] & BB_N(w_silver);
  att |= Attack_BGold[ sq ] &
    ( BB_N(w_gold) | BB_N(w_pro_pawn) );
  att |= Attack_King[ sq ] & BB_N(w_king);
  att |= Attack_BKnight[ sq ] & BB_N(w_knight);
  //飛車の飛び駒の利きを加える
  index = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  att |= Attack_Rook[ sq ][ index ] & BB_N(w_rook);

  //香車。飛車の利きにマスクする
  att |= Attack_Rook[sq][index] & Attack_BLance[sq] & BB_N(w_lance);
  
  //角
  index = (( (Occupied0 & Attack_Bishop_mask[sq]) * Attack_Bishop_magic[sq])
            >> Attack_Bishop_shift[ sq ]) & mask_magic;
  att |= Attack_Bishop[ sq ][ index ] & BB_N(w_bishop);

  //利きを当てている全ての駒の位置を記録
  *attack_pieces = att;

  //最後に利きのある駒の数を数える．
  while( (from = FirstOne(att)) != -1 ) {
      count ++;
      att ^= Bit( from );
    }
  
  return count;
}

int attacks_to_b( int sq, unsigned int *attack_pieces ) {
  int count = 0, from;
  unsigned int att, index;
  //sqからある駒の利きの位置にその駒があれば、sqにその駒がある
  att  = Attack_WPawn[ sq ] & BB_N(b_pawn);
  att |= Attack_WSilver[ sq ] & BB_N(b_silver);
  att |= Attack_WGold[ sq ] &
    ( BB_N(b_gold) | BB_N(b_pro_pawn) );
  att |= Attack_King[ sq ] & BB_N(b_king);
  att |= Attack_WKnight[ sq ] & BB_N(b_knight);
  //飛車の飛び駒の利きを加える
  index  = (( (Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
            >> Attack_Rook_shift[ sq ]) & mask_magic;
  att |= Attack_Rook[ sq ][ index ] & BB_N(b_rook);
  //香車。飛車の利きにマスクする
  att |= Attack_Rook[sq][index] & Attack_WLance[sq] & BB_N(b_lance);
  //角
  index = (( (Occupied0 & Attack_Bishop_mask[sq]) * Attack_Bishop_magic[sq])
            >> Attack_Bishop_shift[ sq ]) & mask_magic;
  att |= Attack_Bishop[ sq ][ index ] & BB_N(b_bishop);
  
  //利きを当てている全ての駒の位置を記録  
  *attack_pieces = att;
  
  while( (from = FirstOne(att)) != -1 )
    {
      count ++;
      att ^= Bit( from );
    }
  
  return count;
}

void pinInfo_w( unsigned int pin[] ){
  memset( pin, 0, sizeof(unsigned int)*32 );

  unsigned bb;
  int pIndex, sq = SQ_W_KING;
  int index;
  //飛車
  bb = BB_N(b_rook) & Attack_Rook[sq][0];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Rook_mask[ pIndex ])
		* Attack_Rook_magic[pIndex])
	      >> Attack_Rook_shift[pIndex]) & mask_magic;
    pin[ Pin_Rook[pIndex][sq][index][0] ]
      |= Pin_Rook[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }
  //角
  bb = BB_N(b_bishop) & Attack_Bishop[sq][0];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Bishop_mask[ pIndex ])
		* Attack_Bishop_magic[pIndex])
	      >> Attack_Bishop_shift[pIndex]) & mask_magic;
    pin[ Pin_Bishop[pIndex][sq][index][0] ]
      |= Pin_Bishop[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }
  //香車
  bb = BB_N(b_lance) & Attack_WLance[sq];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Rook_mask[ pIndex ])
		* Attack_Rook_magic[pIndex])
	      >> Attack_Rook_shift[pIndex]) & mask_magic;
    pin[ Pin_Rook[pIndex][sq][index][0] ]
      |= Pin_Rook[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }  
  
  return;
}

void pinInfo_b( unsigned int pin[] ){
  memset( pin, 0, sizeof(unsigned int)*32 );

  unsigned bb;
  int pIndex, sq = SQ_B_KING;
  int index;
  //飛車
  bb = BB_N(w_rook) & Attack_Rook[sq][0];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Rook_mask[ pIndex ])
		* Attack_Rook_magic[pIndex])
	      >> Attack_Rook_shift[pIndex]) & mask_magic;
    pin[ Pin_Rook[pIndex][sq][index][0] ]
      |= Pin_Rook[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }
  //角
  bb = BB_N(w_bishop) & Attack_Bishop[sq][0];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Bishop_mask[ pIndex ])
		* Attack_Bishop_magic[pIndex])
	      >> Attack_Bishop_shift[pIndex]) & mask_magic;
    pin[ Pin_Bishop[pIndex][sq][index][0] ]
      |= Pin_Bishop[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }
  //香車
  bb = BB_N(w_lance) & Attack_BLance[sq];
  while( (pIndex = FirstOne( bb )) != -1 ) {
    index  = (( (Occupied0 & Attack_Rook_mask[ pIndex ])
		* Attack_Rook_magic[pIndex])
	      >> Attack_Rook_shift[pIndex]) & mask_magic;
    pin[ Pin_Rook[pIndex][sq][index][0] ]
      |= Pin_Rook[pIndex][sq][index][1];
    bb ^= Bit( pIndex );
  }
  
  return;
}

int gen_drop_w( unsigned int moves[], int count )
{
  unsigned long dests;
  int to;

  if( IsHand_W( gold ) )
    {
      dests = ~Occupied0 & bb_mask;
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_gold, move_drop, to, 0, 0 );
          count ++;
          moves[ count ] = MOVE( w_knight, move_drop, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
    }
  
  if( IsHand_W( silver ) )
    {
      dests = ~Occupied0 & bb_mask;
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_silver, move_drop, to, 0, 0 );
          count ++;
          moves[ count ] = MOVE( w_bishop, move_drop, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
    }
  
  if( IsHand_W( pro_pawn ) ) {
    dests = ~Occupied0 & bb_mask;
    while( (to = FirstOne(dests)) != -1 ) {
      moves[ count ] = MOVE( w_pro_pawn, move_drop, to, 0, 0 );
      count ++;
      moves[ count ] = MOVE( w_lance, move_drop, to, 0, 0 );
      count ++;
      dests ^= Bit( to );
    }
  }
  
  if( IsHand_W( pawn ) ){
    dests = ~Occupied0 & bb_mask;
    while( (to = FirstOne(dests)) != -1 ) {
      //飛車として打つ手
      moves[ count ] = MOVE( w_pawn, move_drop, to, 0, 0 );
      count ++;
      moves[ count ] = MOVE( w_rook, move_drop, to, 0, 0 );
      count ++;
      dests ^= Bit( to );
    }
  }

  return count;
}

int gen_drop_b( unsigned int moves[], int count )
{
  unsigned long dests;
  int to;

  if( IsHand_B( gold ) )
    {
      dests = ~Occupied0 & bb_mask;
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( b_gold, move_drop, to, 0, 0 );
          count ++;
          moves[ count ] = MOVE( b_knight, move_drop, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
    }
  
  if( IsHand_B( silver ) )
    {
      dests = ~Occupied0 & bb_mask;
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( b_silver, move_drop, to, 0, 0 );
          count ++;
          moves[ count ] = MOVE( b_bishop, move_drop, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
    }
  
  if( IsHand_B( pro_pawn ) ) {
    dests = ~Occupied0 & bb_mask;
    while( (to = FirstOne(dests)) != -1 ) {
      moves[ count ] = MOVE( b_pro_pawn, move_drop, to, 0, 0 );
      count ++;
      moves[ count ] = MOVE( b_lance, move_drop, to, 0, 0 );
      count ++;
      dests ^= Bit( to );
    }
  }
  
  if( IsHand_B( pawn ) ){
    dests = ~Occupied0 & bb_mask;
    while( (to = FirstOne(dests)) != -1 ) {
      //飛車として打つ手
      moves[ count ] = MOVE( b_pawn, move_drop, to, 0, 0 );
      count ++;
      moves[ count ] = MOVE( b_rook, move_drop, to, 0, 0 );
      count ++;
      dests ^= Bit( to );
    }
  }
  
  return count;
}

int gen_cap_w( unsigned int moves[], int count, unsigned int pin[] )
{
  /* ret: number of total moves */
  unsigned int dests, bb, att;
  int from, to, index;
  int cap;
  
  bb = BB_W_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WPawn[ from ] & ~nOccupiedB;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
	  moves[ count ] = MOVE_C( w_rook, from, to, 0, 1, cap);
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  bb = BB_W_PRO_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WGold[ from ] & ~nOccupiedB;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
          moves[ count ] = MOVE_C( w_lance, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_SILVER;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WSilver[ from ] & ~nOccupiedB;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ) {
	cap = get_piece_on_sq_b(to);
	moves[ count ] = MOVE_C( w_bishop, from, to, 0, 1, cap);
	count ++;
	dests ^= Bit( to );
      }
      bb ^= Bit( from );
    }

  bb = BB_W_GOLD;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WGold[ from ] & ~nOccupiedB;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
          moves[ count ] = MOVE_C( w_knight, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_BISHOP;
  while( (from = FirstOne(bb)) != -1 )
    {
      index =(((Occupied0 & Attack_Bishop_mask[from])
               * Attack_Bishop_magic[from])
              >> Attack_Bishop_shift[from]) & mask_magic;
      dests = Attack_Bishop[ from ][ index ];
      dests &= ~nOccupiedB & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
	  moves[ count ] = MOVE_C( w_silver, from, to, 0, 1, cap);
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_ROOK;
  while( (from = FirstOne(bb)) != -1 ) {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ];
    dests &= ~nOccupiedB & ~(pin[ from ]);
    while( (to = FirstOne(dests)) != -1 ){
      cap = get_piece_on_sq_b(to);
      moves[ count ] = MOVE_C( w_pawn, from, to, 0, 1, cap);
      count ++;
      dests ^= Bit( to );
    }
    bb ^= Bit( from );
  }

  bb = BB_W_LANCE;
  while( (from = FirstOne(bb)) != -1 )
    {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ] & Attack_WLance[from];
    dests &= ~nOccupiedB & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
          moves[ count ] = MOVE_C( w_pro_pawn, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_KNIGHT;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WKnight[ from ] & ~nOccupiedB;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_b(to);
          moves[ count ] = MOVE_C( w_gold, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  dests = Attack_King[ SQ_W_KING ] & ~nOccupiedB;
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_b( to, &att ) == 0 )
        {
	  cap = get_piece_on_sq_b(to);
          moves[ count ] = MOVE_C( w_king, SQ_W_KING, to, 0, 1, cap);
          count ++;
        }
      dests ^= Bit( to );
    }

  return count;
}

int gen_cap_b( unsigned int moves[], int count, unsigned int pin[] )
{
  /* ret: number of total moves */
  unsigned int dests, bb, att;
  int from, to, index, cap;

  
  bb = BB_B_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BPawn[ from ] & ~nOccupiedW;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
	  moves[ count ] = MOVE_C( b_rook, from, to, 0, 1, cap);
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  bb = BB_B_PRO_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BGold[ from ] & ~nOccupiedW;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
          moves[ count ] = MOVE_C( b_lance, from, to, 0, 1, cap );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_SILVER;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BSilver[ from ] & ~nOccupiedW;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ) {
	cap = get_piece_on_sq_w(to);
	moves[ count ] = MOVE_C( b_bishop, from, to, 0, 1, cap);
	count ++;
	dests ^= Bit( to );
      }
      bb ^= Bit( from );
    }

  bb = BB_B_GOLD;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BGold[ from ] & ~nOccupiedW;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
          moves[ count ] = MOVE_C( b_knight, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_BISHOP;
  while( (from = FirstOne(bb)) != -1 )
    {
      index =(((Occupied0 & Attack_Bishop_mask[from])
               * Attack_Bishop_magic[from])
              >> Attack_Bishop_shift[from]) & mask_magic;
      dests = Attack_Bishop[ from ][ index ];
      dests &= ~nOccupiedW & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
	  moves[ count ] = MOVE_C( b_silver, from, to, 0, 1, cap);
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_ROOK;
  while( (from = FirstOne(bb)) != -1 ) {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ];
    dests &= ~nOccupiedW & ~(pin[ from ]);
    while( (to = FirstOne(dests)) != -1 ){
      cap = get_piece_on_sq_w(to);
      moves[ count ] = MOVE_C( b_pawn, from, to, 0, 1, cap);
      count ++;
      dests ^= Bit( to );
    }
    bb ^= Bit( from );
  }

  bb = BB_B_LANCE;
  while( (from = FirstOne(bb)) != -1 )
    {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ] & Attack_BLance[from];
    dests &= ~nOccupiedW & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
          moves[ count ] = MOVE_C( b_pro_pawn, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_KNIGHT;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BKnight[ from ] & ~nOccupiedW;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  cap = get_piece_on_sq_w(to);
          moves[ count ] = MOVE_C( b_gold, from, to, 0, 1, cap);
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  dests = Attack_King[ SQ_B_KING ] & ~nOccupiedW;
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_w( to, &att ) == 0 )
        {
	  cap = get_piece_on_sq_w(to);
          moves[ count ] = MOVE_C( b_king, SQ_B_KING, to, 0, 1, cap);
          count ++;
        }
      dests ^= Bit( to );
    }

  return count;
}

int gen_nocap_w( unsigned int moves[], int count, unsigned int pin[] )
{
  /* ret: number of generated moves */
  unsigned int dests, bb, att;
  int from, to, index;

  
  bb = BB_W_SILVER;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WSilver[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ) {
          moves[ count ] = MOVE( w_bishop, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_GOLD;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WGold[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_knight, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_BISHOP;
  while( (from = FirstOne(bb)) != -1 )
    {
      index =(((Occupied0 & Attack_Bishop_mask[from])
               * Attack_Bishop_magic[from])
              >> Attack_Bishop_shift[from]) & mask_magic;
      dests = Attack_Bishop[ from ][ index ];
      dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  moves[ count ] = MOVE( w_silver, from, to, 0, 0 );
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_ROOK;
  while( (from = FirstOne(bb)) != -1 ) {
      index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
             >> Attack_Rook_shift[from]) & mask_magic;
      dests = Attack_Rook[ from ][ index ];
      dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ){
	  moves[ count ] = MOVE( w_pawn, from, to, 0, 0 );
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_LANCE;
  while( (from = FirstOne(bb)) != -1 )
    {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ] & Attack_WLance[from];
    dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_pro_pawn, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_W_KNIGHT;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WKnight[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_gold, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }


  bb = BB_W_PRO_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WGold[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE( w_lance, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  bb = BB_W_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_WPawn[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  moves[ count ] = MOVE( w_rook, from, to, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  
  
  dests = Attack_King[ SQ_W_KING ] & ~Occupied0;
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_b( to, &att ) == 0 )
        {
          moves[ count ] = MOVE( w_king, SQ_W_KING, to, 0, 0 );
          count ++;
        }
      dests ^= Bit( to );
    }
  
  return count;
}

int gen_nocap_b( unsigned int moves[], int count, unsigned int pin[]  )
{
  /* ret: number of generated moves */
  unsigned int dests, bb, att;
  int from, to, index;

  bb = BB_B_SILVER;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BSilver[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ) {
	moves[ count ] = MOVE_C( b_bishop, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_GOLD;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BGold[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE_C( b_knight, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_BISHOP;
  while( (from = FirstOne(bb)) != -1 )
    {
      index =(((Occupied0 & Attack_Bishop_mask[from])
               * Attack_Bishop_magic[from])
              >> Attack_Bishop_shift[from]) & mask_magic;
      dests = Attack_Bishop[ from ][ index ];
      dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  moves[ count ] = MOVE_C( b_silver, from, to, 0, 0, 0 );
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_ROOK;
  while( (from = FirstOne(bb)) != -1 ) {
      index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
             >> Attack_Rook_shift[from]) & mask_magic;
      dests = Attack_Rook[ from ][ index ];
      dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 ){
	moves[ count ] = MOVE_C( b_pawn, from, to, 0, 0, 0 );
	  count ++;
	  dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_LANCE;
  while( (from = FirstOne(bb)) != -1 )
    {
    index =(((Occupied0 & Attack_Rook_mask[from]) * Attack_Rook_magic[from])
	    >> Attack_Rook_shift[from]) & mask_magic;
    dests = Attack_Rook[ from ][ index ] & Attack_BLance[from];
    dests &= ~Occupied0 & ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE_C( b_pro_pawn, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  bb = BB_B_KNIGHT;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BKnight[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE_C( b_gold, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }


  bb = BB_B_PRO_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BGold[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
          moves[ count ] = MOVE_C( b_lance, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }
  
  bb = BB_B_PAWN;
  while( (from = FirstOne(bb)) != -1 )
    {
      dests = Attack_BPawn[ from ] & ~Occupied0;
      dests &= ~(pin[ from ]);
      while( (to = FirstOne(dests)) != -1 )
        {
	  moves[ count ] = MOVE_C( b_rook, from, to, 0, 0, 0 );
          count ++;
          dests ^= Bit( to );
        }
      bb ^= Bit( from );
    }

  
  
  dests = Attack_King[ SQ_B_KING ] & ~Occupied0;
  while( (to = FirstOne( dests )) != -1 )
    {
      if( attacks_to_w( to, &att ) == 0 )
        {
          moves[ count ] = MOVE_C( b_king, SQ_B_KING, to, 0, 0, 0 );
          count ++;
        }
      dests ^= Bit( to );
    }
  
  return count;

}


int get_piece_on_sq_w(int sq) {
#define TEST( piece ) \
  if( BB_N( piece ) & Bit( sq ) ) \
    return piece; \

  TEST( w_pro_pawn );
  TEST( w_silver );
  TEST( w_gold );
  TEST( w_pawn );
  TEST( w_rook );
  TEST( w_bishop );
  TEST( w_knight );
  TEST( w_lance );
  if( SQ_W_KING == sq )
    return w_king;  
  return no_piece;
}

int get_piece_on_sq_b(int sq) {

  TEST( b_pro_pawn );
  TEST( b_silver );
  TEST( b_gold );
  TEST( b_pawn );
  TEST( b_rook );
  TEST( b_bishop );
  TEST( b_knight );
  TEST( b_lance );
  if( SQ_B_KING == sq )
    return b_king;
  return no_piece;
#undef TEST
}


int FirstOne(int bb) {
  return bb ? __builtin_ctz(bb) : -1;
}

int popuCount( int piece )
{
  int count = 0;
  unsigned bb = BB_N( piece );
  int b;
  
  while( ( b = FirstOne( bb ) ) != -1 )
    {
      count ++;
      bb &= ~Bit( b );
    }

  return count;
}

int get_turn()
{
  return TURN ? black : white ; 
}

int get_nply()
{
  return N_PLY;
}

