//4bit�֗��萔
enum {b0000, b0001, b0010, b0011,
      b0100, b0101, b0110, b0111,
      b1000, b1001, b1010, b1011,
      b1100, b1101, b1110, b1111,
      b1_1111 = 31};

enum {
  m_promote = b1000,
  //��̎��
  pro_pawn = 1,
  silver = 4,
  gold = 5,
  pawn = 6,
  king = 8,
  lance = pro_pawn + m_promote,
  bishop = silver + m_promote,
  knight = gold + m_promote,
  rook = pawn + m_promote,
  //���Ԃ̋���
  m_white = 0,
  w_pro_pawn = pro_pawn + m_white,
  w_silver = silver + m_white,
  w_gold = gold + m_white,
  w_pawn = pawn + m_white,
  w_king = king + m_white,
  w_lance = lance + m_white,
  w_bishop = bishop + m_white,
  w_knight = knight + m_white,
  w_rook = rook + m_white,
  //���Ԃ̋���
  m_black = 16,
  b_pro_pawn = pro_pawn + m_black,
  b_silver = silver + m_black,
  b_gold = gold + m_black,
  b_pawn = pawn + m_black,
  b_king = king + m_black,
  b_lance = lance + m_black,
  b_bishop = bishop + m_black,
  b_knight = knight + m_black,
  b_rook = rook + m_black,

  bbs_size = 32
};

enum {
  bb_mask = 0x01ffffff, //25bit���̃}�X�N
  mask_magic = 0x0000003f, //�}�W�b�N�r�b�g�{�[�h���p���̃}�X�N
  mask_type = b1111, //��̎�ނ̃}�X�N
  mask_type_c = b1_1111, //��Ԃ��܂߂���̎�ނ̃}�X�N
  mask_nopro = b0111, //�\�̃}�X�N
  mask_piece_color = (b0001 << 4), //��Ԃ��擾����}�X�N

  white = 0, //���
  black = 1, //���
  myturn_not_set = -1, //�Ȃɂ���H
  no_piece = -1, //��
};

//�֗��}�N��
//x�r�b�g�ڂ�1�̃r�b�g���Ԃ�
#define Bit(x) (1 << x)
//�Ֆʂ̃C���f�b�N�X��x, y����v�Z����
#define XY2INDEX(x, y) ( (5-x) + (y-1)*5 )
//0-24 => 24-0 �Ֆʂ̃C���f�b�N�X�𔽓]����H
#define INVERSE(x) (24 - x)
//bb��0�̂Ƃ�-1��Ԃ��B�ŏ��ɗ����Ă���r�b�g��Ԃ�
#define FIRSTONE(bb) (31 - __builtin_clz(bb))

