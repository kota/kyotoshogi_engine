#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "header.h"

int sdepth;

void evaluate_next(int nmove, unsigned int legalmoves[], int scores[]);
int search_root_eval(unsigned int legalmoves[], int nmove, int scores[], int depth);

int search_root_eval_attack(unsigned int legalmoves[], int nmove, int scores[], int depth);


int calcHandsSum() {
  return W_HAND(pawn) + B_HAND(pawn) +
    W_HAND(silver) + B_HAND(silver) +
    W_HAND(gold) + B_HAND(gold) +
    W_HAND(bishop) + W_HAND(bishop) +
    W_HAND(rook) + B_HAND(rook);
}

int score[SIZE_LEGALMOVES];

int compareValue(const void *a, const void *b) {
  return score[*(int*)a] - score[*(int*)b];
}

void evaluate_next(int nmove, unsigned int legalmoves[], int scores[]) {
  int imove;
  for(imove = 0; imove<nmove; ++imove) {
    MAKE_MOVE(legalmoves[imove]);
    scores[imove] = -evaluate(); //自分から見た評価値
    UNMAKE_MOVE;
  }
}

//scoresが降順になるようにlegalmovesをソートする
//ソートはバブル
void score_sort(int nmove, unsigned int legalmoves[], int scores[]) {
  int i, j, temp;
  unsigned int utemp;
  for(i=0; i<nmove-1; ++i) { //iまで済み
    for(j=nmove-1; j>i; --j) { //jが交換対象
      //評価値を評価対象として、入れ替え判定
	if(scores[j-1] < scores[j]) {
	//合法手の入れ替え
	utemp = legalmoves[j-1];
	legalmoves[j-1] = legalmoves[j];
	legalmoves[j] = utemp;
	//評価値の入れ替え
	temp = scores[j-1];
	scores[j-1] = scores[j];
	scores[j] = temp;
      }
    }
  }
}

//次の局面の評価値順にソートする
void one_order(unsigned int legalmoves[], int nmove) {
  int scores[SIZE_LEGALMOVES];
  evaluate_next(nmove, legalmoves, scores);
  score_sort(nmove, legalmoves, scores);
}

//指定した手数分探索した結果を用いてオーダリングする
void search_order(unsigned int legalmoves[], int nmove, int depth) {
  int scores[SIZE_LEGALMOVES];
  search_root_eval(legalmoves, nmove, scores, depth);
  score_sort(nmove, legalmoves, scores);
}

void search_order_attack(unsigned int legalmoves[], int nmove, int depth) {
  int scores[SIZE_LEGALMOVES];
  search_root_eval_attack(legalmoves, nmove, scores, depth);
  score_sort(nmove, legalmoves, scores);
}

int beta_cut;

int ordered_search_root(int propagation)
{
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove, best;
  int nmove = 0;
  int depth = sdepth;
  short value;
  short beta, max = 0;
  unsigned int legalmoves[ SIZE_LEGALMOVES ];

  srand((unsigned)time(NULL));

  //depth水増し
  if(calcHandsSum() <= 1) {
    depth += 2;
  }

  //合法手を, 次局面の静的評価順に並べる
  nmove = gen_legalmoves( legalmoves );
  search_order(legalmoves, nmove, 3);

  if( nmove == 0 )
    {
      out( " This game was already concluded.\n" );
      return -2;
    }

  out( " Root move generation -> %d moves.\n", nmove );

  //初期化
  best = 0;
  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  beta_cut = 0;
  if(propagation) {
    for( imove = 0; imove < nmove; imove++ )
      {
	MAKE_MOVE( legalmoves[ imove ] );
	value = -ordered_search( -beta, -max, depth -1+(imove==0?0.5:0), 1 );
	UNMAKE_MOVE;
	
	if( value > max )
	  {
	    max = value;
	    best = imove;
	  }	
      }
  }else { //下の階層でもオーダリングしてから探索を行う。
    for( imove = 0; imove < nmove; imove++ )
      {
	MAKE_MOVE( legalmoves[ imove ] );
	value = -search( -beta, -max, depth -1+(imove==0?0.5:0), 1 );
	UNMAKE_MOVE;
	
	if( value > max )
	  {
	    max = value;
          best = imove;
	  }
	
      }
  }

  MAKE_MOVE( legalmoves[ best ] );
  out("beta_cut: %d\n", beta_cut);
  return 0;
}

int ordered_search_root_attack(int propagation)
{
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove, best;
  int nmove = 0;
  int depth = sdepth;
  short value;
  short beta, max = 0;
  unsigned int legalmoves[ SIZE_LEGALMOVES ];

  srand((unsigned)time(NULL));
  //depth水増し
  if(calcHandsSum() <= 1) {
    depth += 2;
  }

  //合法手を, 次局面の静的評価順に並べる
  nmove = gen_legalmoves( legalmoves );
  search_order_attack(legalmoves, nmove, 3);

  if( nmove == 0 )
    {
      out( " This game was already concluded.\n" );
      return -2;
    }

  out( " Root move generation -> %d moves.\n", nmove );

  //初期化
  best = 0;
  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  beta_cut = 0;
  if(propagation) {
    for( imove = 0; imove < nmove; imove++ )
      {
	MAKE_MOVE( legalmoves[ imove ] );
	value = -ordered_search_attack( -beta, -max, depth -1+(imove==0?0.5:0), 1 );
	UNMAKE_MOVE;
	
	if( value > max )
	  {
	    max = value;
	    best = imove;
	  }	
      }
  }else { //下の階層でもオーダリングしてから探索を行う。
    for( imove = 0; imove < nmove; imove++ )
      {
	MAKE_MOVE( legalmoves[ imove ] );
	value = -search_attack( -beta, -max, depth -1+(imove==0?0.5:0), 1 );
	UNMAKE_MOVE;
	
	if( value > max )
	  {
	    max = value;
	    best = imove;
	  }
	
      }
  }

  MAKE_MOVE( legalmoves[ best ] );
  out("beta_cut: %d\n", beta_cut);
  return 0;
}

int ordered_zero_search_root(int propagation)
{
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove;
  int nmove = 0;
  int depth = sdepth;
  short value;
  short beta, max = 0;
  unsigned int legalmoves[ SIZE_LEGALMOVES ];
  int zero_max, zero_best;

  srand((unsigned)time(NULL));

  //depth水増し
  if(calcHandsSum() <= 1) {
    depth += 2;
  }

  //合法手を, 次局面の静的評価順に並べる
  nmove = gen_legalmoves( legalmoves );
  search_order(legalmoves, nmove, 3);

  if( nmove == 0 )
    {
      out( " This game was already concluded.\n" );
      return -2;
    }

  out( " Root move generation -> %d moves.\n", nmove );

  //初期化
  zero_best = 0;
  zero_max = SCORE_MAX;
  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  beta_cut = 0;
  for( imove = 0; imove < nmove; imove++ ) {
    MAKE_MOVE( legalmoves[ imove ] );
    value = -ordered_search( -beta, -max, depth -1+(imove==0?0.5:0), 1 );
    UNMAKE_MOVE;
    
    if( value > max ) {
      max = value;
    }

    if(value == 0) {
      zero_max = value;
      zero_best = imove;
    }
    else if( value > 0 && value < zero_max) {
      zero_max = value;
      zero_best = imove;
    }else if( -value > 0 && -value < zero_max ) {
      zero_max = -value;
      zero_best = imove;
    }
  }
  //  MAKE_MOVE( legalmoves[ best ] );
  MAKE_MOVE( legalmoves[zero_best] );
  out("beta_cut: %d\n", beta_cut);
  return 0;
}

int ordered_search_attack( short alpha, short beta, float depth, int ply )
{
  int imove;
  short value;
  short max = -SCORE_MAX;  
  int nmove;

  unsigned int legalmoves[ SIZE_LEGALMOVES ];
  
  if( depth <= 0 )
    {
      return evaluate_attack();
    }

  //合法手を, 次局面の静的評価順に並べる
  nmove = gen_legalmoves( legalmoves );
  if(depth > 3) {
     search_order_attack(legalmoves, nmove, 3); }
    
  if( nmove == 0 )
    { return SCORE_MATED + ply; }
  
  max = alpha;

  //最初のノードは0.5手探索
  MAKE_MOVE( legalmoves[ 0 ] );
  value = -search_attack( -beta, -max, depth -0.5, ply +1 );
  UNMAKE_MOVE;
      
  if( value >= beta ) {
      beta_cut++;
      return value;
  }
  if( value > max ) max = value;

  //2番目からのノードの探索
  for( imove = 1; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search_attack( -beta, -max, depth -1, ply +1 );
      UNMAKE_MOVE;
      
      if( value >= beta )
        {
	  beta_cut++;
          return value;
        }
      if( value > max )
        {
          max = value;
        }
      
    }

  return max;
}

int ordered_search( short alpha, short beta, float depth, int ply )
{
  int imove;
  short value;
  short max = -SCORE_MAX;  
  int nmove;

  unsigned int legalmoves[ SIZE_LEGALMOVES ];
  
  if( depth <= 0 )
    {
      return evaluate();
    }

  //合法手を, 次局面の静的評価順に並べる
  nmove = gen_legalmoves( legalmoves );
  if(depth > 3) {
     search_order(legalmoves, nmove, 3); }
    
  if( nmove == 0 )
    { return SCORE_MATED + ply; }
  
  max = alpha;

  //最初のノードは0.5手探索
  MAKE_MOVE( legalmoves[ 0 ] );
  value = -search( -beta, -max, depth -0.5, ply +1 );
  UNMAKE_MOVE;
      
  if( value >= beta ) {
      beta_cut++;
      return value;
  }
  if( value > max ) max = value;

  //2番目からのノードの探索
  for( imove = 1; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search( -beta, -max, depth -1, ply +1 );
      UNMAKE_MOVE;
      
      if( value >= beta )
        {
	  beta_cut++;
          return value;
        }
      if( value > max )
        {
          max = value;
        }
      
    }

  return max;
}


int search_root_eval(unsigned int legalmoves[], int nmove, int scores[], int depth) {
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove;
  short value;
  short beta, max = 0;

  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  beta_cut = 0;

  for( imove = 0; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search( -beta, -max, depth -1, 1 );
      UNMAKE_MOVE;
      
      scores[imove] = value;

      if( value > max )
          max = value;      
    }
  return 0;
}

int search_root_eval_attack(unsigned int legalmoves[], int nmove, int scores[], int depth) {
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove;
  short value;
  short beta, max = 0;

  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  beta_cut = 0;

  for( imove = 0; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search_attack( -beta, -max, depth -1, 1 );
      UNMAKE_MOVE;
      
      scores[imove] = value;

      if( value > max )
          max = value;      
    }
  return 0;
}

int search_root()
{
  /*
    ret =  0: succeeded
        = -1: failed
        = -2: the game is already finished
   */
  int imove, best;
  int nmove = 0;
  int depth = sdepth;
  short value;
  short beta, max = 0;
  unsigned int legalmoves[ SIZE_LEGALMOVES ];

  nmove = gen_legalmoves( legalmoves );

  if( nmove == 0 )
    {
      out( " This game was already concluded.\n" );
      return -2;
    }

  out( " Root move generation -> %d moves.\n", nmove );
  
  if( nmove <= 1 )
    {
      best = 0;
      max = 0;
    }

  best = 0;
  max   = -SCORE_MAX;
  beta  = SCORE_MAX;
  
  for( imove = 0; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search( -beta, -max, depth -1, 1 );
      UNMAKE_MOVE;
      
      if( value > max )
        {
          max = value;
          best = imove;
        }
      
    }

  MAKE_MOVE( legalmoves[ best ] );
  return 0;
}

int search( short alpha, short beta, int depth, int ply )
{
  int imove;
  short value;
  short max = -SCORE_MAX;  
  int nmove;

  unsigned int legalmoves[ SIZE_LEGALMOVES ];

  if( depth <= 0 )
    {
      return evaluate();
    }
  
  nmove = gen_legalmoves( legalmoves );
  
  if( nmove == 0 )
    { return SCORE_MATED + ply; }
  
  max = alpha;
  for( imove = 0; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search( -beta, -max, depth -1, ply +1 );
      UNMAKE_MOVE;
      
      if( value >= beta )
        {
          return value;
        }
      if( value > max )
        {
          max = value;
        }
      
    }

  return max;
}

int search_attack( short alpha, short beta, int depth, int ply )
{
  int imove;
  short value;
  short max = -SCORE_MAX;  
  int nmove;

  unsigned int legalmoves[ SIZE_LEGALMOVES ];

  if( depth <= 0 )
    {
      return evaluate_attack();
    }
  
  nmove = gen_legalmoves( legalmoves );
  
  if( nmove == 0 )
    { return SCORE_MATED + ply; }
  
  max = alpha;
  for( imove = 0; imove < nmove; imove++ )
    {
      MAKE_MOVE( legalmoves[ imove ] );
      value = -search_attack( -beta, -max, depth -1, ply +1 );
      UNMAKE_MOVE;
      
      if( value >= beta )
        {
          return value;
        }
      if( value > max )
        {
          max = value;
        }
      
    }

  return max;
}

short evaluate()
{
  int score = 0;

  score += (popuCount(w_king) - popuCount(b_king)) * 5000;
  score += ( popuCount( w_pawn )       - popuCount( b_pawn ) )       * 100;
  score += ( popuCount( w_pro_pawn )       - popuCount( b_pro_pawn ) )       * 200;
  score += ( popuCount( w_silver )     - popuCount( b_silver ) )     * 200;
  score += ( popuCount( w_gold )       - popuCount( b_gold ) )       * 200;
  score += ( popuCount( w_bishop )     - popuCount( b_bishop ) )     * 200;
  score += ( popuCount( w_rook )       - popuCount( b_rook ) )       * 200;
  score += ( popuCount( w_knight )   - popuCount( b_knight ) )   * 200;
  score += ( popuCount( w_lance )   - popuCount( b_lance ) )   * 200;

  score += ( W_HAND( pawn )   - B_HAND( pawn ) )   * 200;
  score += ( W_HAND( silver ) - B_HAND( silver ) ) * 300;
  score += ( W_HAND( gold )   - B_HAND( gold ) )   * 300;
  score += ( W_HAND( pro_pawn ) - B_HAND( pro_pawn ) ) * 300;

  score += rand()%100 - 50;

  return get_turn() ? -score: score;
}

short evaluate_attack() {
  int score = 0;
  int sq, index;
  unsigned bb;
  int b;
  unsigned attacks;
  int count = 0;
  score += (popuCount(w_king) - popuCount(b_king)) * 5000;
  score += ( popuCount( w_pawn )       - popuCount( b_pawn ) )       * 100;
  score += ( popuCount( w_pro_pawn )       - popuCount( b_pro_pawn ) )       * 200;
  score += ( popuCount( w_silver )     - popuCount( b_silver ) )     * 200;
  score += ( popuCount( w_gold )       - popuCount( b_gold ) )       * 200;
  score += ( popuCount( w_bishop )     - popuCount( b_bishop ) )     * 200;
  score += ( popuCount( w_rook )       - popuCount( b_rook ) )       * 200;
  score += ( popuCount( w_knight )   - popuCount( b_knight ) )   * 200;
  score += ( popuCount( w_lance )   - popuCount( b_lance ) )   * 200;

  score += ( W_HAND( pawn )   - B_HAND( pawn ) )   * 200;
  score += ( W_HAND( silver ) - B_HAND( silver ) ) * 300;
  score += ( W_HAND( gold )   - B_HAND( gold ) )   * 300;
  score += ( W_HAND( pro_pawn ) - B_HAND( pro_pawn ) ) * 300;


  bb = BB_N(w_pro_pawn);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_WGold[sq];
    while( (b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }
  bb = BB_N(b_pro_pawn);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_BGold[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(w_silver);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_WSilver[sq];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_silver);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_BSilver[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(w_gold);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_WGold[sq];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_gold);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_BGold[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(w_pawn);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_WPawn[sq];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_pawn);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_BPawn[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(w_knight);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_WKnight[sq];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_knight);
  while((sq = FirstOne(bb)) != -1) {
    attacks = Attack_BKnight[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }


  bb = BB_N(w_rook);
  while((sq = FirstOne(bb)) != -1) {
    index  = (( (Occupied0 & Attack_Rook_mask[ sq ])
		* Attack_Rook_magic[sq])
	      >> Attack_Rook_shift[sq]) & mask_magic;
    attacks = Attack_Rook[ sq ][ index ];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_rook);
  while((sq = FirstOne(bb)) != -1) {
    index  = (( (Occupied0 & Attack_Rook_mask[ sq ])
		* Attack_Rook_magic[sq])
	      >> Attack_Rook_shift[sq]) & mask_magic;
    attacks = Attack_Rook[ sq ][ index ];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(w_bishop);
  while((sq = FirstOne(bb)) != -1) {
    index  = (( (Occupied0 & Attack_Bishop_mask[ sq ])
		* Attack_Bishop_magic[sq])
	      >> Attack_Bishop_shift[sq]) & mask_magic;
    attacks = Attack_Rook[ sq ][ index ];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_rook);
  while((sq = FirstOne(bb)) != -1) {
    index  = (( (Occupied0 & Attack_Bishop_mask[ sq ])
		* Attack_Bishop_magic[sq])
	      >> Attack_Bishop_shift[sq]) & mask_magic;
    attacks = Attack_Bishop[ sq ][ index ];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }


  bb = BB_N(w_lance);
  while((sq = FirstOne(bb)) != -1) {
    index =(((Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
	    >> Attack_Rook_shift[sq]) & mask_magic;
    attacks = Attack_Rook[ sq ][ index ] & Attack_WLance[sq];
    while((b=FirstOne(attacks)) != -1) {
      count++;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }

  bb = BB_N(b_lance);
  while((sq = FirstOne(bb)) != -1) {
    index =(((Occupied0 & Attack_Rook_mask[sq]) * Attack_Rook_magic[sq])
	    >> Attack_Rook_shift[sq]) & mask_magic;
    attacks = Attack_Rook[ sq ][ index ] & Attack_BLance[sq];
    while((b=FirstOne(attacks)) != -1) {
      count--;
      attacks ^= Bit(b);
    }
    bb ^= Bit(sq);
  }
  score += count * 10;
  return score;
}
