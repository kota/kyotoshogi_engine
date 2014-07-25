#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"

#define DELIM " "

static void new_game();
static void new_game_rand();
static void out_board();
static void out_hands_b();
static void out_hands_w();
static void out_bitboard();

static void set_depth(char **last);

static void manual_move(char **last);
static void back();
static int islegalmove(unsigned int moves[], int count, unsigned int move);
static void search_start();
static void ordered_search_start(int);
static void ordered_search_attack_start(int);
static void ordered_zero_search_start(int);

int cmd_prompt(){
  /*
    ret = 0: continue
         -1: exit
    */
  char buf[256];
  const char *token;
  char *last;
  int count_byte, ret;
  ret = 0;

  if( TURN ) out("Black %d> ", N_PLY +1 );
  else out("White %d> ", N_PLY +1 );
  
  memset( buf, 0, sizeof(buf) );
  fgets( buf, sizeof( buf ), stdin );
  count_byte = strlen( buf );
  if( !strcmp( buf, "\n") )
    { return 0; }
  
  buf[count_byte-1] = '\0';
  
  token = strtok_r( buf, DELIM, &last );
  if( token == NULL )
    { return 0; }
  
  if( !strcmp( token, "quit" ) )
    { return -1; }
  else if( !strcmp( token, "board" ) )
    { out_position(); }
  else if( !strcmp( token, "back" ) )
    { back(); }
  else if( !strcmp( token, "move" ) )
    { manual_move( &last ); }
  else if( !strcmp( token, "set-depth") || !strcmp(token, "sd")  )
    { set_depth( &last ); }
  else if( !strcmp( token, "search" ) || !strcmp( token, "s" ) )
    { ordered_search_start(1); }
  else if( !strcmp( token, "asearch" ) || !strcmp( token, "as" ) )
    { ordered_search_attack_start(1); }
  else if( !strcmp( token, "ssearch" ) || !strcmp( token, "ss" ) )
    { ordered_zero_search_start(1); }
  else if( !strcmp( token, "new" )  )
    { new_game(); }
  else if( !strcmp( token, "newr" )  )
    { new_game_rand(); }
  //else if( !strcmp( token, "record" )  )
  //  { out_record( 1 ); }
  else
    { ret = -1; }
  
  if( ret == -1 )
    out(" Invalid command\n");
  
  return 0;
}

static void search_start()
{
  int iret;
  char buf[16];
  iret = search_root();
  if( iret == -2 )
    {
      out( " This game was already concluded.\n");
      return;
    }
  else if( iret == -1 )
    {
      out( " Search() failed.\n" );
      return;
    }
  out_position();
  str_CSA_move( buf, history[get_nply()-1].move );
  out(" move: %s\n",buf);
  return;
}

static void ordered_search_start(int propagation)
{
  int iret;
  iret = ordered_search_root(propagation);
  if( iret == -2 )
    {
      out( " This game was already concluded.\n");
      return;
    }
  else if( iret == -1 )
    {
      out( " Search() failed.\n" );
      return;
    }
  out_position();
  return;
}

static void ordered_search_attack_start(int propagation)
{
  int iret;
  iret = ordered_search_root_attack(propagation);
  if( iret == -2 )
    {
      out( " This game was already concluded.\n");
      return;
    }
  else if( iret == -1 )
    {
      out( " Search() failed.\n" );
      return;
    }
  out_position();
  return;
}

static void ordered_zero_search_start(int propagation)
{
  int iret;
  iret = ordered_zero_search_root(propagation);
  if( iret == -2 )
    {
      out( " This game was already concluded.\n");
      return;
    }
  else if( iret == -1 )
    {
      out( " Search() failed.\n" );
      return;
    }
  out_position();
  return;
}


//可変引数を受け取ってprintfのような出力
void out(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);
  fflush(stdout);
  //そのうち、ログファイルなどに吐き出し
}

//盤面を表示する。
//のちのちは残り時間や評価値、合法手なども表示する。
void out_position() {
  out("\n");
  out_board();

  unsigned int moves[256];
  int nmove;
  nmove = gen_legalmoves( moves );
  out_legalmoves( moves, nmove );
  out("例：move 5554KYの入力形式で、５五に居ると金が５四に移動して香車になる指し手を入力できます。\n");
  out("AIに思考させる場合は,sと入力してください。\n");
  //  assert( isCorrect_Occupied() );
}

void out_legalmoves( unsigned int moves[], int count )
{
  int i;
  char buf[8];

  out(" Legal Moves( %d ) =\n ", count);
  
  for( i=0; i < count; i++)
    {
      str_CSA_move( buf, moves[ i ] );
      out(" %s",buf);
      if( i%10 == 9 )
        out("\n ");
    }
  out("\n");
  return;
}

void str_CSA_move( char *str, unsigned int move )
{
  int type_c = MOVE_TYPE( move );
  int type   = type_c & ~mask_piece_color;
  int from   = MOVE_FROM( move );
  int to     = MOVE_TO( move );
  int prom   = MOVE_PROMOTE( move );

  if( from == move_drop )
    {
      snprintf( str, 7, "%d%d%d%d%s",
                0, 0,
                5-( to%5),    ( to/5   +1 ),
                ch_piece_csa[ type ] );
    }
  else if( prom )
    {
      snprintf( str, 7, "%d%d%d%d%s",
                5-( from%5 ), ( from/5 +1 ),
                5-( to%5),    ( to/5   +1 ),
                ch_piece_csa[ type + m_promote ] );
    }
  else
    {
      snprintf( str, 7, "%d%d%d%d%s",
                5-( from%5 ), ( from/5 +1 ),
                5-( to%5),    ( to/5   +1 ),
                ch_piece_csa[ type ] );    
    }
  return;
}

static void out_hands_b() {
  int i;
  for( i=0; i < 8; i++) {
    if( B_HAND(i) > 0 )
      out(" %s%d", ch_piece[i], B_HAND(i) );
  }
}

static void out_hands_w() {
  int i;
  for( i=0; i < 8; i++) {
    if( W_HAND(i) > 0 )
      out(" %s%d", ch_piece[i], W_HAND(i) );  
  }
}

static void out_bitboard() {
  const char *num[5] = {"一", "二", "三", "四", "五"};
  int i, j, index, type;
  out("   5   4   3   2   1\n");
  for( i=0; i<5; i++ ) {
    //頭と左端
    out(" ---------------------\n");
    out(" |");
    for( j=0; j<5; j++ ) {
	//駒の表示
        index = i*5 + j;
        if( Occupied0 & Bit( index ) )
          {
            if( (type = get_piece_on_sq_w( index )) != no_piece ) {
	      out("%s|", ch_piece2[ type ] );
	    }
            if( (type = get_piece_on_sq_b( index )) != no_piece ) {
	      out("%s|", ch_piece2[ type ] );
	    }
          }
        else
          { out("   |"); continue; }
    }
    //段の表示
    out("%s\n", num[i]);    
  }
  out(" ---------------------\n");
}

static void out_board() {
  //手番の表示
  if(TURN == black) out("[%d 手目 後手]\n", N_PLY+1);
  else out("[%d手目 先手]\n", N_PLY+1);
  //後手持ち駒の表示
  out(" 後手持駒= ");  
  out_hands_b();
  out("\n");
  //盤面の表示
  out_bitboard();
  //先手持ち駒の表示
  out(" 先手持駒= ");  
  out_hands_w();
  out("\n");
}



//盤面の状態変更など
static void new_game() {
  game_initialize();
  out_position();
}

static void new_game_rand() {
  game_initialize_rand();
  out_position();
}

static void back()
{
  if( N_PLY > 0 )
    {
      UNMAKE_MOVE;
      out_position();
    }
  else
    {
      out(" This is the starting position.\n");
    }
}

//着手の入力による盤面の進行
static void manual_move(char **last) {
  const char *p = strtok_r(NULL, DELIM, last);
  unsigned int move;
  unsigned int moves[256];
  int nmove;

  //着手の解析
  out("move: %s\n", p);
  move = CSA2Internal(p);
  if(move == MOVE_NULL) {
    out("Invalid Move\n");
    return;
  }
  //合法手の確認
  nmove = gen_legalmoves(moves);
  if( islegalmove(moves, nmove, move) == 0) {
    out("Invalid Move\n");
    return;
  }

  //着手の実行
  MAKE_MOVE(move);

  //盤面の更新
  out_position();
}

static void set_depth(char **last) {
  const char *p = strtok_r(NULL, DELIM, last);  
  char *end_ptr;
  sdepth = (int)strtol(p, &end_ptr, 10);
}

static int islegalmove(unsigned int moves[], int count, unsigned int move) {
  int i;
  char buf[32];
  //out("move: %d\n", move );
  for( i=0; i < count; i++) {
    str_CSA_move( buf, moves[ i ] );
    //out("move[%d]: %s[%u, %u]", i, buf, moves[i], moves[i] & ~move_mask_captured);
    if( move == (moves[i])) {
      return 1;
    }
  }
  return 0;
}

unsigned int CSA2Internal(const char *str) {
  /* return = MOVE_NULL : illegal
              others    : a move ( validity is not guaranteed ) */
  int move;
  int from, to, type, type_c, prom, cap, cap_type;
  int fromX, fromY, toX, toY, i;
  const char *buf;

  if( str == NULL || strlen( str ) != 6 ) { 
    out("str: %s\n", str);
    return MOVE_NULL; 
  }

  fromX = str[0] -'0';
  fromY = str[1] -'0';
  toX   = str[2] -'0';
  toY   = str[3] -'0';
  buf   = str + 4;

  from = 5 - fromX + ( fromY -1 )*5;
  to   = 5 - toX   + ( toY   -1 )*5;
  prom = 0; //ダミー

  //持ち駒を打つ時
  if( fromX == 0 && fromY == 0 ){
    from = move_drop;
    for( i=0; i < 16; i++ ){
      if( !strcmp( buf, ch_piece_csa[ i ] ) ){
	type_c = TURN ? i+16 : i;
	break;
      }
    }
    if( i >= 16 ) type_c = 0;
    cap = 0;
    cap_type = 0;
    
    move = MOVE_C( type_c, from, to, prom, cap, cap_type );
    return move;
  }
  //盤外へ移動
  if( from < 0 || from >= 25 || to < 0 || to >= 25 ) { 
    out("invalid range,  from: %d, to: %d", from, to);
    return MOVE_NULL; 
  }

  type_c = TURN ? get_piece_on_sq_b( from ) : get_piece_on_sq_w( from );
  //fromに駒が無かった
  if(type_c == no_piece) return MOVE_NULL;
  //おかしい駒
  if( !strcmp( buf, ch_piece_csa[ 0 ] ) ) return MOVE_NULL;
  //手番を含まない駒の情報
  for(i = 0; i<16; i++) {
    if(!strcmp(buf, ch_piece_csa[i])) {
      type = i;
      break;
    }
  }
  if(i==16) return MOVE_NULL;

  //玉以外で表なら成る
  if(type != king) {
    if(0){
      //想定外の駒
      out("なんか違う, type: %d\n", type);
      return MOVE_NULL; 
    }
  }
  type += TURN ? m_black: m_white;
  //取った駒
  cap = 1;
  cap_type = TURN ? get_piece_on_sq_w( to ) : get_piece_on_sq_b( to );
  if( cap_type == no_piece ) {
    cap = 0;
    cap_type = 0;
  }
    
  move = MOVE_C( type, from, to, prom, cap, cap_type );
  return move;
}

