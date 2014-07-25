#include <stdlib.h>
#include "header.h"

int main( int argc, char *argv[] ) {

  if( starting_initialize()) exit(1);
  //初期化処理
  game_initialize();
  out_position();

  //本処理
  while(1) {
    if( cmd_prompt() == -1)
      close_program();
  }
}

void close_program() {
  game_finalize();
  exit(0);
}
