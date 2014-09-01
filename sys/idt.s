#copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.


.text
 
.global _x86_64_asm_lidt
 _x86_64_asm_lidt:

      lidt (%rdi)
      retq
