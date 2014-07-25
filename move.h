//�w����Ɋւ���֗��萔
//�̈悲�Ƃ̃}�X�N�B
enum {
  move_drop = 25,
  move_mask_type = (b1_1111 << 0),
  move_mask_from = (b1_1111 << 5),
  move_mask_to = (b1_1111 << 10),
  move_mask_promote = (b0001 << 15),
  move_mask_capture = (b0001 << 16),
  move_mask_captured = (b1_1111 << 17),
};

//�w���肩������擾����}�N��
//��L�̃}�X�N��p���Ď擾���Ă���
//��̎�ށC�ړ��O�E��C���������ۂ��C�����������ۂ��C�������̎��
//���������ۂ�?�����Ă��邩�ۂ��H�͗v����
#define MOVE_TYPE(move)           ( (move & move_mask_type)         )
#define MOVE_FROM(move)           ( (move & move_mask_from)    >> 5 )
#define MOVE_TO(move)             ( (move & move_mask_to)      >> 10)
#define MOVE_PROMOTE(move)        ( (move & move_mask_promote) >> 15)
#define MOVE_CAPTURE(move)        ( (move & move_mask_capture) >> 16)
#define MOVE_CAPTURED_TYPE(move)  ( (move & move_mask_captured)>> 17)
//NULL�̎w�����\��
#define MOVE_NULL                 0

//�ߊl������̎�ނ��i�[����ʒu�ɑΉ������邽�߂̃r�b�g�V�t�g
#define CAPTURED_2_MOVE(cap)      ( cap << 17 )

//�r�b�g�{�[�h�������ɁC���ꂼ��Y�����錅�ɂ��낦��悤�Ƀr�b�g�V�t�g����. 
//������Ȃ��ꍇ�̎w�����\���r�b�g�{�[�h�𐶐�����
#define MOVE(v1, v2, v3, v4, v5) ( (v1) + (v2 << 5) + (v3 << 10) +\
                                   (v4 << 15) + (v5 << 16) + (0 << 17) )
//������ꍇ�̎w�����\���r�b�g��𐶐�����D
#define MOVE_C(v1, v2, v3, v4, v5, v6)\
                                   ( (v1) + (v2 << 5) + (v3 << 10) +\
                                   (v4 << 15) + (v5 << 16) + (v6 << 17) )

