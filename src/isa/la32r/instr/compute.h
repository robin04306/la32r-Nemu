/***************************************************************************************
* Copyright (c) 2021-2022 Weitong Wang, University of Chinese Academy of Sciences
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

def_EHelper(add_w) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub_w) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(nor) {
  rtl_nor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll_w) {
  rtl_shl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra_w) {
  rtl_sar(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl_w) {
  rtl_shr(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slli_w) {
  rtl_shli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai_w) {
  rtl_sari(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli_w) {
  rtl_shri(s, ddest, dsrc1, id_src2->imm);
}

/********************* mul/div ************************/

def_EHelper(mul_w) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh_w) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh_wu) {
  rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div_w) {
    if(*dsrc2 == 0){
      printf("[NEMU]: divide zero occur on div.w, PC:0x%x , nemu abort to avoid fault, please check!!!\n",s->pc);
      assert(*dsrc2 != 0);
    }else{
      rtl_divs_q(s, ddest, dsrc1, dsrc2);
    }
}

def_EHelper(div_wu) {
    if(*dsrc2 == 0){
      printf("[NEMU]: divide zero occur on div.wu, PC:0x%x , nemu abort to avoid fault, please check!!!\n",s->pc);
      assert(*dsrc2 != 0);
    }else{
      rtl_divu_q(s, ddest, dsrc1, dsrc2);
    }
}

def_EHelper(mod_w) {
    if(*dsrc2 == 0){
      printf("[NEMU]: divide zero occur on mod.w, PC:0x%x , nemu abort to avoid fault, please check!!!\n",s->pc);
      assert(*dsrc2 != 0);
    }else{
      rtl_divs_r(s, ddest, dsrc1, dsrc2);
    }
}

def_EHelper(mod_wu) {
    if(*dsrc2 == 0){
      printf("[NEMU]: divide zero occur on mod.wu, PC:0x%x , nemu abort to avoid fault, please check!!!\n",s->pc);
      assert(*dsrc2 != 0);
    }else{
      rtl_divu_r(s, ddest, dsrc1, dsrc2);
    }
}

/*******************************************************/

def_EHelper(lu12i_w) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(pcaddu12i) {
  rtl_li(s, ddest, id_src1->imm);
}

/*******************************************************/

def_EHelper(addi_w) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti) {
  rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltui) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, (id_src2->imm) & (0x00000fff));
}

def_EHelper(ori) {
  rtl_ori(s, ddest, dsrc1, (id_src2->imm) & (0x00000fff));
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, (id_src2->imm) & (0x00000fff));
}

/* Five additional CNN / AI vector instructions. */

static inline int32_t cnn_signed8(uint32_t x) {
  x &= 0xffu;
  return x < 128u ? (int32_t)x : (int32_t)x - 256;
}

static inline int32_t cnn_signed16(uint32_t x) {
  x &= 0xffffu;
  return x < 32768u ? (int32_t)x : (int32_t)x - 65536;
}

/* Four signed INT8 products accumulated into each INT32 lane. */
def_EHelper(vdot4acc_b) {
  int vd = s->isa.instr.r3.rd;
  int vj = s->isa.instr.r3.rj;
  int vk = s->isa.instr.r3.rk;

  uint32_t a[4], b[4], acc[4];

  for (int i = 0; i < 4; i++) {
    a[i] = cpu.vr[vj]._32[i];
    b[i] = cpu.vr[vk]._32[i];
    acc[i] = cpu.vr[vd]._32[i];
  }

  for (int lane = 0; lane < 4; lane++) {
    uint32_t sum = acc[lane];

    for (int k = 0; k < 4; k++) {
      int index = lane * 4 + k;
      int word = index / 4;
      int shift = (index % 4) * 8;

      int32_t x = cnn_signed8(a[word] >> shift);
      int32_t y = cnn_signed8(b[word] >> shift);

      sum += (uint32_t)(x * y);
    }

    cpu.vr[vd]._32[lane] = sum;
  }
}

/* Two signed INT16 products accumulated into each INT32 lane. */
def_EHelper(vmac2acc_h) {
  int vd = s->isa.instr.r3.rd;
  int vj = s->isa.instr.r3.rj;
  int vk = s->isa.instr.r3.rk;

  uint32_t a[4], b[4], acc[4];

  for (int i = 0; i < 4; i++) {
    a[i] = cpu.vr[vj]._32[i];
    b[i] = cpu.vr[vk]._32[i];
    acc[i] = cpu.vr[vd]._32[i];
  }

  for (int lane = 0; lane < 4; lane++) {
    uint32_t sum = acc[lane];

    for (int k = 0; k < 2; k++) {
      int shift = k * 16;

      int32_t x = cnn_signed16(a[lane] >> shift);
      int32_t y = cnn_signed16(b[lane] >> shift);

      int64_t product = (int64_t)x * (int64_t)y;
      sum += (uint32_t)product;
    }

    cpu.vr[vd]._32[lane] = sum;
  }
}

/* Signed INT8 saturating addition followed by ReLU. */
def_EHelper(vreluadd_b) {
  int vd = s->isa.instr.r3.rd;
  int vj = s->isa.instr.r3.rj;
  int vk = s->isa.instr.r3.rk;

  uint32_t a[4], b[4];

  for (int i = 0; i < 4; i++) {
    a[i] = cpu.vr[vj]._32[i];
    b[i] = cpu.vr[vk]._32[i];
  }

  for (int i = 0; i < 4; i++) {
    uint32_t result = 0;

    for (int k = 0; k < 4; k++) {
      int shift = k * 8;

      int32_t x = cnn_signed8(a[i] >> shift);
      int32_t y = cnn_signed8(b[i] >> shift);
      int32_t sum = x + y;

      if (sum < 0) sum = 0;
      if (sum > 127) sum = 127;

      result |= ((uint32_t)sum & 0xffu) << shift;
    }

    cpu.vr[vd]._32[i] = result;
  }
}

/* Clamp four INT32 lanes to INT8 and pack into low 32 bits. */
def_EHelper(vclipack_w) {
  int vd = s->isa.instr.r3.rd;
  int vj = s->isa.instr.r3.rj;

  uint32_t packed = 0;

  for (int i = 0; i < 4; i++) {
    int32_t value = (int32_t)cpu.vr[vj]._32[i];

    if (value < -128) value = -128;
    if (value > 127) value = 127;

    packed |= ((uint32_t)value & 0xffu) << (i * 8);
  }

  cpu.vr[vd]._32[0] = packed;
  cpu.vr[vd]._32[1] = 0;
  cpu.vr[vd]._32[2] = 0;
  cpu.vr[vd]._32[3] = 0;
}

/* Signed INT8 max of each group of four bytes. */
def_EHelper(vmaxpool4_b) {
  int vd = s->isa.instr.r3.rd;
  int vj = s->isa.instr.r3.rj;

  uint32_t packed = 0;

  for (int lane = 0; lane < 4; lane++) {
    uint32_t word = cpu.vr[vj]._32[lane];
    int32_t maximum = -128;

    for (int k = 0; k < 4; k++) {
      int32_t value = cnn_signed8(word >> (k * 8));
      if (value > maximum)
        maximum = value;
    }

    packed |= ((uint32_t)maximum & 0xffu) << (lane * 8);
  }

  cpu.vr[vd]._32[0] = packed;
  cpu.vr[vd]._32[1] = 0;
  cpu.vr[vd]._32[2] = 0;
  cpu.vr[vd]._32[3] = 0;
}

/* Custom CNN Acceleration Instructions */

def_EHelper(vconvacc_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] += (int32_t)cpu.vr[vj]._32[i] * (int32_t)cpu.vr[vk]._32[i];
}

def_EHelper(vrelu_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vj]._32[i];
    cpu.vr[vd]._32[i] = (val > 0) ? val : 0;
  }
}

def_EHelper(vpool_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t a = (int32_t)cpu.vr[vj]._32[i], b = (int32_t)cpu.vr[vk]._32[i];
    cpu.vr[vd]._32[i] = (a > b) ? a : b;
  }
}

def_EHelper(vbias_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] += (int32_t)cpu.vr[vj]._32[i];
}

def_EHelper(vscale_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = (int32_t)cpu.vr[vj]._32[i] * (int32_t)cpu.vr[vk]._32[i];
}

def_EHelper(vavgpool_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t a = (int32_t)cpu.vr[vj]._32[i], b = (int32_t)cpu.vr[vk]._32[i];
    cpu.vr[vd]._32[i] = (a + b) >> 1;
  }
}

def_EHelper(vclip_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vj]._32[i], hi = (int32_t)cpu.vr[vk]._32[i];
    if (val < 0) val = 0;
    if (val > hi) val = hi;
    cpu.vr[vd]._32[i] = val;
  }
}

def_EHelper(vquant_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vj]._32[i] >> (int32_t)cpu.vr[vk]._32[i];
    if (val < -128) val = -128;
    if (val > 127)  val = 127;
    cpu.vr[vd]._32[i] = val;
  }
}

def_EHelper(vreplgr2vr_w) {
  int vd = s->isa.instr.r2.rd, rj = s->isa.instr.r2.rj;
  int32_t val = (int32_t)reg_l(rj);
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = val;
}

def_EHelper(vadd_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = cpu.vr[vj]._32[i] + cpu.vr[vk]._32[i];
}

def_EHelper(vmax_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t a = (int32_t)cpu.vr[vj]._32[i], b = (int32_t)cpu.vr[vk]._32[i];
    cpu.vr[vd]._32[i] = (a > b) ? a : b;
  }
}

def_EHelper(vsra_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = (int32_t)cpu.vr[vj]._32[i] >> (int32_t)cpu.vr[vk]._32[i];
}

def_EHelper(vsrai_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  int imm = s->isa.instr.r3.rk;   // immediate shift amount, not a register index
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = (int32_t)cpu.vr[vj]._32[i] >> imm;
}

def_EHelper(vmin_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t a = (int32_t)cpu.vr[vj]._32[i], b = (int32_t)cpu.vr[vk]._32[i];
    cpu.vr[vd]._32[i] = (a < b) ? a : b;
  }
}

def_EHelper(vmul_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = (int32_t)cpu.vr[vj]._32[i] * (int32_t)cpu.vr[vk]._32[i];
}

def_EHelper(vxor_v) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = cpu.vr[vj]._32[i] ^ cpu.vr[vk]._32[i];
}

def_EHelper(vqrnd_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vj]._32[i];
    int32_t shift = (int32_t)cpu.vr[vk]._32[i];
    int32_t rounded = (shift > 0) ? (val + (1 << (shift - 1))) >> shift : val;
    if (rounded < -128) rounded = -128;
    if (rounded > 127)  rounded = 127;
    cpu.vr[vd]._32[i] = rounded;
  }
}

def_EHelper(vbiasrelu_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vd]._32[i] + (int32_t)cpu.vr[vj]._32[i];
    cpu.vr[vd]._32[i] = (val > 0) ? val : 0;
  }
}

def_EHelper(vleakyrelu_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj, vk = s->isa.instr.r3.rk;
  for (int i = 0; i < 4; i++) {
    int32_t val = (int32_t)cpu.vr[vj]._32[i];
    int32_t shift = (int32_t)cpu.vr[vk]._32[i];
    cpu.vr[vd]._32[i] = (val >= 0) ? val : (val >> shift);
  }
}

def_EHelper(vredmax_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  int32_t m = (int32_t)cpu.vr[vj]._32[0];
  for (int i = 1; i < 4; i++) {
    int32_t v = (int32_t)cpu.vr[vj]._32[i];
    if (v > m) m = v;
  }
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = m;
}

def_EHelper(vredsum_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  int32_t sum = 0;
  for (int i = 0; i < 4; i++)
    sum += (int32_t)cpu.vr[vj]._32[i];
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = sum;
}

def_EHelper(vbsrl_v) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  int imm = s->isa.instr.r3.rk;  // byte shift amount, 0-31
  uint8_t bytes[16], result[16] = {0};
  for (int i = 0; i < 4; i++) {
    uint32_t w = cpu.vr[vj]._32[i];
    bytes[i*4+0] = w & 0xff; bytes[i*4+1] = (w>>8) & 0xff;
    bytes[i*4+2] = (w>>16) & 0xff; bytes[i*4+3] = (w>>24) & 0xff;
  }
  for (int i = 0; i < 16; i++) {
    int src = i + imm;
    result[i] = (src < 16) ? bytes[src] : 0;
  }
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = result[i*4+0] | (result[i*4+1]<<8) | (result[i*4+2]<<16) | (result[i*4+3]<<24);
}

def_EHelper(vreplvei_w) {
  int vd = s->isa.instr.r3.rd, vj = s->isa.instr.r3.rj;
  int idx = (s->isa.instr.val >> 10) & 0x3;   // bits 11:10, the real immediate position
  int32_t val = (int32_t)cpu.vr[vj]._32[idx];
  for (int i = 0; i < 4; i++)
    cpu.vr[vd]._32[i] = val;
}