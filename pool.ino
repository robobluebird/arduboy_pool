#include <Arduboy2.h>
 
Arduboy2 arduboy;

#define TABLE_OFFSET 6
#define TOP 1
#define RIGHT 2
#define BOTTOM 3
#define LEFT 4

int TABLE_WIDTH =  WIDTH - 12;
int TABLE_HEIGHT = HEIGHT - 12;

struct Coords {
  float x;
  float y;
};

struct Ball {
  int id;
  int pattern;
  float radius;
  float x;
  float y;
  float dx;
  float dy;
};

struct Rail {
  int x1;
  int y1;
  int x2;
  int y2;
  int x3;
  int y3;
  int x4;
  int y4;
  int orientation;
  int yInt1;
  int yInt2;
};

int cueAngle = 180;

struct Ball cueBall;
struct Rail rail;
struct Rail rails[6];

void drawRail(Rail rail) {
  arduboy.drawLine(rail.x1, rail.y1, rail.x2, rail.y2);
  arduboy.drawLine(rail.x2, rail.y2, rail.x4, rail.y4);
  arduboy.drawLine(rail.x3, rail.y3, rail.x4, rail.y4);
  arduboy.drawLine(rail.x3, rail.y3, rail.x1, rail.y1);
}

void drawRails() {
  for (int i = 0; i < 6; i++) {
    drawRail(rails[i]);
  }
}

struct Coords ball45DegreeRelativeCoords(Ball ball) {
  float rad = M_PI * 2 * 45.0 / 360;
  float y = ball.radius * sin(rad);
  float x = ball.radius * cos(rad);

  return { x, y };
}

bool collideRail(Rail rail, Ball ball) {
  if (rail.orientation == TOP) {
    Coords a = ball45DegreeRelativeCoords(ball);
    Coords c = { ball.x + a.x, ball.y - a.y };
    
    if (c.x > rail.x1 && c.x < rail.x3 && c.y > rail.y1) {
      // y = (1)x + b
      float y = c.x + rail.yInt1;

      if (c.y < y) {
        return true;
      }
    } else if (ball.x >= rail.x3 && ball.x <= rail.x4 && ball.y - ball.radius < rail.y3) {
      return true;
    } else {
      c.x = ball.x - a.x;

      if (c.x > rail.x4 && c.x < rail.x2 && c.y > rail.y2) {
        float y = -c.x + rail.yInt2;

        if (c.y < y) {
          return true;
        }
      }
    }

    return false;
  } else if (rail.orientation == BOTTOM) {
    return false;
  } else if (rail.orientation == LEFT) {
    return false;
  } else if (rail.orientation == RIGHT) {
    return false;
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30);

  // these rail coords are relative to the TABLE and not the SCREEN. does that make sense?
  rails[0] = { 9, 4, 61, 4, 13, 8, 57, 8, TOP, -5, 65 };
  rails[1] = { 67, 4, 119, 4, 71, 8, 115, 8, TOP };
  
  cueBall = { 0, 0, 2.0, TABLE_OFFSET + TABLE_WIDTH / 2 - 3, TABLE_OFFSET + TABLE_HEIGHT / 2, 0.0, 0.0 };
}

void handleInput() {
  if (cueBall.dx == 0 && cueBall.dy == 0) {
    if (arduboy.justPressed(A_BUTTON)) {
      cueBall.dx = 5 * sin(M_PI * 2 * cueAngle / 360);
      cueBall.dy = 5 * cos(M_PI * 2 * cueAngle / 360);
    }
    
    if (arduboy.pressed(LEFT_BUTTON)) {
      cueAngle += 5;
    }
  
    if (arduboy.pressed(RIGHT_BUTTON)) {
      cueAngle -= 5;
    }
  
    if (cueAngle > 359) {
      cueAngle = 0;
    } else if (cueAngle < 0) {
      cueAngle = 359;
    }
  }
}

void draw() {
  arduboy.drawRect(TABLE_OFFSET, TABLE_OFFSET, TABLE_WIDTH, TABLE_HEIGHT);
  arduboy.drawLine(TABLE_OFFSET, TABLE_OFFSET, TABLE_OFFSET + TABLE_WIDTH, TABLE_OFFSET, BLACK);
  arduboy.drawRect(0, 0, 1, HEIGHT);
  arduboy.drawCircle(cueBall.x, cueBall.y, cueBall.radius);

  if (cueBall.dx == 0 && cueBall.dy == 0) {
    arduboy.fillCircle(cueBall.x - 6 * sin(M_PI * 2 * cueAngle / 360), cueBall.y - 6 * cos(M_PI * 2 * cueAngle / 360), 1);
  }
  drawRails();
}

void calculate() {
  cueBall.x += cueBall.dx;
  cueBall.y += cueBall.dy;

  if (cueBall.dx != 0.0) {
    if (fabs(cueBall.dx) <= 0.1) {
      cueBall.dx = 0;
    } else {
      cueBall.dx = cueBall.dx * 0.9;
    }
  }

  if (cueBall.dy != 0.0) {
    if (fabs(cueBall.dy) <= 0.1) {
      cueBall.dy = 0;
    } else {
      cueBall.dy = cueBall.dy * 0.9;
    }
  }

  if (cueBall.y + cueBall.radius > TABLE_OFFSET + TABLE_HEIGHT) {
    cueBall.y = TABLE_OFFSET + TABLE_HEIGHT - cueBall.radius;
    cueBall.dy = -cueBall.dy;
  }

  if (cueBall.x - cueBall.radius < TABLE_OFFSET) {
    cueBall.x = TABLE_OFFSET + cueBall.radius;
    cueBall.dx = -cueBall.dx;
  } else if (cueBall.x + cueBall.radius > TABLE_OFFSET + TABLE_WIDTH) {
    cueBall.x = TABLE_OFFSET + TABLE_WIDTH - cueBall.radius;
    cueBall.dx = -cueBall.dx;
  }

  if (collideRail(rails[0], cueBall)) {
    float temp = cueBall.dx;
    cueBall.dx = -cueBall.dy;
    cueBall.dy = -temp;
  }
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();
  arduboy.pollButtons();

  handleInput();
  calculate();
  draw();
  
  arduboy.display();
}
