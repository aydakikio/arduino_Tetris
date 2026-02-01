#include <U8g2lib.h>

#define T_WIDTH 4    // Tetromino width 
#define T_HEIGHT 4   // Tetromino height 
#define A_WIDTH 8    // Arena width
#define A_HEIGHT 14  // Arena height

// ===== HARDWARE CONFIGURATION =====
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R3, -1, A5, A4);

int arena[A_HEIGHT][A_WIDTH] = {0}; 

const int tetrominoes[7][16] = {
  { 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },  //I
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 },  //O
  { 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0 },  //S
  { 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },  //Z
  { 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0 },  //T
  { 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0 },  //L
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0 }   //J
};

bool game_over = false;
int currTetrominoIdx;
int currRotation = 0;
int score = 0;
int currX = A_WIDTH / 2 - 2;  
int currY = 0;

unsigned long lastDropTime = 0;
const int dropInterval = 1000; 

void new_piece();
bool moveDown();
void merge_piece();
void check_lines();

void setup() {
  u8g2.begin();
  randomSeed(analogRead(0));  
  new_piece();  
}

void loop() {
  unsigned long currentTime = millis();
  
  if (!game_over) {

    if (currentTime - lastDropTime > dropInterval) {
      if (!moveDown()) {

        merge_piece();
        check_lines();
        new_piece();
      }
      lastDropTime = currentTime;
    }
  }
  
  draw_game();
  delay(50); 
}

int rotate(int x, int y, int rotation) {
  switch (rotation % 4) {
    case 0: return y * 4 + x;          //0 Degree rotation
    case 1: return 12 + y - (x * 4);   //90 Degree rotation
    case 2: return 15 - (y * 4) - x;   //180 Degree rotation
    case 3: return 3 - y + (x * 4);    //270 Degree rotation
    default: return 0;
  }
}

bool check_valid_position(int tetromino, int rotation, int x_position, int y_position) {
  for (int x = 0; x < 4; x++) {  
    for (int y = 0; y < 4; y++) {  
      int index = rotate(x, y, rotation);

      if (tetrominoes[tetromino][index] != 1) { 
        continue;
      }

      int area_x = x + x_position;
      int area_y = y + y_position;

      // check collision with borders
      if (area_x < 0 || area_x >= A_WIDTH || area_y >= A_HEIGHT) {
        return false;
      }

      // checking collisition with another blocks
      if (area_y >= 0 && arena[area_y][area_x] == 1) {
        return false;
      }
    }
  }
  return true;
}

bool moveDown() {
    if (check_valid_position(currTetrominoIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void new_piece() {
  currTetrominoIdx = random(7); //choosing a random tetrominy
  currRotation = 0;
  currX = A_WIDTH / 2 - 2;  //Starting ftom center
  currY = 0;
  
  if (!check_valid_position(currTetrominoIdx, currRotation, currX, currY)) {
    game_over = true;
  }
}

void merge_piece() {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      int index = rotate(x, y, currRotation);
      if (tetrominoes[currTetrominoIdx][index] == 1) {
        int arena_x = currX + x;
        int arena_y = currY + y;
        if (arena_y >= 0 && arena_x >= 0 && arena_x < A_WIDTH) {
          arena[arena_y][arena_x] = 1;
        }
      }
    }
  }
}

void check_lines() {
  for (int y = A_HEIGHT - 1; y >= 0; y--) {
    bool lineComplete = true;
    for (int x = 0; x < A_WIDTH; x++) {
      if (arena[y][x] == 0) {
        lineComplete = false;
        break;
      }
    }
    
    if (lineComplete) {
      //Deleting the line and moving down the upper lines
      for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < A_WIDTH; x++) {
          arena[yy][x] = arena[yy-1][x];
        }
      }

      //empty the upper line
      for (int x = 0; x < A_WIDTH; x++) {
        arena[0][x] = 0;
      }
      score += 100; 
      y++; 
    }
  }
}

void draw_blocks() {
  // drawing fixed 
  for(int y = 0; y < A_HEIGHT; y++){
    for (int x = 0; x < A_WIDTH; x++) {
      if (arena[y][x] == 1) {
        u8g2.drawBox(x * 8 + 1, y * 8 + 17, 7, 7);
      }
    }
  }

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      int index = rotate(x, y, currRotation);
      if (tetrominoes[currTetrominoIdx][index] == 1) {
        int drawX = (currX + x) * 8 + 1;
        int drawY = (currY + y) * 8 + 17;
        if (drawY >= 17) { 
          u8g2.drawBox(drawX, drawY, 7, 7);
        }
      }
    }
  }
}

void draw_game() {

  // Draw everything
  u8g2.firstPage();
  do {
  
    u8g2.drawHLine(0, 16, 64);
    u8g2.setFont(u8g2_font_5x7_mf);
    u8g2.drawStr(4, 10, "Score: ");
    u8g2.setCursor(39, 10);
    u8g2.print(score);
    u8g2.drawFrame(0, 0, 64, 128);

    draw_blocks();


  } while (u8g2.nextPage());
}
