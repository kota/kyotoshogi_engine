#include "board.h"
#include "move.h"

#define SEARCH_DEPTH         5

#define SCORE_MAX            8100
#define SCORE_MATED          (-SCORE_MAX)

extern const char *ch_piece[16];
extern const char *ch_piece2[32];
extern const char *ch_piece_csa[16];

extern int sdepth;

//main.c
void close_program();

//ini.c
void game_initialize();
void game_initialize_rand();
void game_finalize();

//io.c
void out( const char *format, ... );
int cmd_prompt();
void out_position();
void str_CSA_move( char *buf, unsigned int move );
unsigned int CSA2Internal(const char *str);
void out_legalmoves( unsigned int moves[], int count );

//search.c

int search_root();
int search( short alpha, short beta, int depth, int ply);
int search_attack( short alpha, short beta, int depth, int ply);


int ordered_search_root(int propagation);
int ordered_search_root_attack(int propagation);
int ordered_zero_search_root(int propagation);
int ordered_search( short alpha, short beta, float depth, int ply);
int ordered_search_attack( short alpha, short beta, float depth, int ply);

short evaluate();
short evaluate_attack();
