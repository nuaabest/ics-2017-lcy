#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t2);
	operand_write(id_dest,&t2);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
				printf("3");
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
				printf("4");
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
			//printf("5");
  //TODO();
	cpu.esp=cpu.ebp;
	rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
				//printf("6");
	int m=cpu.eax;
  if (decoding.is_operand_size_16) {
    //TODO();
		m=m<<8;
		m=m>>8;
  }
  else {
					//printf("6");
    m=m<<16;
    m=m>>16;		
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
      TODO();
  }
  else {
    //TODO();
		if(cpu.eax<0){
				cpu.edx=0xFFFFFFFF;
		}
		else cpu.edx=0x0;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
	//printf("eax1:0x%08x\n",cpu.eax);
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
	//printf("eax2:0x%08x\n",cpu.eax);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
	//printf("eax2:0x%08x\n",cpu.eax);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
