#include <U8g2lib.h>

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R3, -1, A5, A4);

int fixed_blocks[14][8];
int score = 0;

int active_row_blocks = 0;

void setup() {

  u8g2.begin();

  for(int I = 0 ; I< 8 ; I++){
    for(int J =0; J<16; J++){
      fixed_blocks[I][J]=0;
    }
  }

  fixed_blocks[0][3]=1;
}

void loop() {
  // put your main code here, to run repeatedly:
  u8g2.firstPage();
  do {

    
      for(int I = 0 ; I< 8 ; I++){
        for(int J =0; J<14; J++){
          int value = fixed_blocks[I][J];

          if (value ==1){
            active_row_blocks +=1;
            u8g2.drawBox(J*8, (I*8)+16, 8, 8);
          }
        }

        active_row_blocks=0;
      }

      u8g2.drawHLine(0, 16, 64);
      u8g2.setFont(u8g2_font_5x7_mf);
      u8g2.drawStr(4, 10, "Score: ");
      u8g2.setCursor(39, 10);
      u8g2.print(score);
      u8g2.drawFrame(0, 0, 64, 128);

  }while (u8g2.nextPage());
}

void draw_game(){
  
  // Draw everything

}
