#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define CHARCOAL {47, 72, 88, 255}
#define LAPIS_LAZULI {51, 101, 138, 255}
#define CAROLINA_BLUE {134, 187, 216, 255}
#define HUNYADI_YELLOW {246, 174, 45, 255}
#define PANTONE {242, 100, 25, 255}
#define PALE_AZURE {99, 210, 255, 255}
#define STEEL_BLUE {32, 129, 195, 255}
#define TIFFANY_BLUE {120, 213, 215, 255}
#define ASH_GRAY {190, 216, 212, 255}
#define SEASALT {247, 249, 249, 255}

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define GAME_NAME "PONG"
#define FPS 60

extern "C" float R(int velocity);
extern "C" float S(int velocity, int time);
extern "C" float C(int positionX, int positionY);
extern "C" int G(int color, float i);
extern "C" float SE(int i);
extern "C" float SA(float rotationAngle, float segment);
extern "C" float EA(float rotationAngle, float segment);
extern "C" float EX(float positionX, float radius, float startAngle);
extern "C" float EY(float positionY, float radius, float startAngle);

enum Path
{
    Regular,
    Sin,
    Curve
};

enum Difficulty
{
    Easy,
    Meduim,
    Hard
};

enum Program
{
    Cpp,
    Assembly
};

typedef struct GameMode
{
    int numberOfPlayer;
    Path path;
    Difficulty difficulty;
    Program program;
} GameMode;

class Shape
{
protected:
    int positionX;
    int positionY;
    Shape(int posX, int posY);

public:
    int getX();
    int getY();
};

class RectangularShape : public Shape
{
protected:
    int width;
    int height;
    RectangularShape(int posX, int posY, int wid, int hei);

public:
    int getWidth();
    int getHeight();
    bool checkCollision(Vector2 mousePoint);
};

class Paddle : public RectangularShape
{
protected:
    int velocityY;
    int accelerationY;
    int padding;
    Color color;

    Paddle(int posX, int posY);
    void limitCheck();
};

class Player
{
private:
    char username[20];
    int score;

public:
    Player(const char *name);
    void updateScore(int delta);
    int getScore();
    char *getName();
    void setName(char name[20]);
};

class Ball : public Shape
{
private:
    int velocityX;
    int velocityY;
    int accelerationX;
    int accelerationY;
    float radius;
    Color color1;
    Color color2;
    Color color3;
    GameMode gameMode;
    int round;
    double *calculationTime;

    void path();

public:
    Ball(GameMode gM, double *cT);
    Ball(double *cT);
    void draw();
    void update(Player *player1, Player *player2);
    void update();
    void collision(Paddle paddle);
    void reset();
    void choose();
    bool conrner();
};

class RightPaddle : public Paddle
{
private:
    bool isAI;

public:
    RightPaddle(int posX, int posY, bool AI);
    void draw();
    void update(Ball ball);
};

class LeftPaddle : public Paddle
{
public:
    LeftPaddle(int posX, int posY);
    void draw();
    void update();
};

class Clickable
{
protected:
    bool isFocus;
    Color focus;
    Color normal;
    Clickable(Color foc, Color nor);

public:
    void toggleFocus();
    void setFocus(bool status);
    bool getFocus();
};

class TextBox : public RectangularShape, public Clickable
{
private:
    char title[20];
    int length;
    char text[100];
    Color fill;
    Color textColor;
    bool isPassword;

public:
    TextBox(int posX, int posY, const char *titleName, bool passwordStatus = false);
    void addChar(int key);
    void deleteChar();
    char *getText();
    int getLength();
    void draw();
};

class Button : public RectangularShape, public Clickable
{
private:
    char title[20];
    Color fill;
    Color textColor;

public:
    Button(int posX, int posY, const char *titleName);
    void draw();
};

class Text : public Shape
{
private:
    char text[100];
    Color color;

public:
    Text(const char *txt, Color col, int posX, int posY);
    void draw();
    void updateText(char *txt);
};

class CheckBox : public RectangularShape, public Clickable
{
private:
    char title[20];
    Color unChecked;
    Color checked;
    Color textColor;
    bool isChecked;

public:
    CheckBox(int posX, int posY, const char *titleName);
    void draw();
    void toggleCheck();
    void setCheck(bool status);
    bool getCheck();
};

bool checkLogin(TextBox *username, TextBox *password, Button *login);
bool checkMainMenuSelections(CheckBox *singlePlayer, CheckBox *multiPlayer,
                             CheckBox *regular, CheckBox *sin, CheckBox *curve,
                             CheckBox *easy, CheckBox *medium, CheckBox *hard,
                             CheckBox *cpp, CheckBox *assembly);
bool loginMenu(Player *player, double *calculationTime);
bool mainMenu(GameMode *gameMode);
bool game(Player *player1, Player *player2, GameMode *gameMode, double *calculationTime);
float regularPath(int velocity, GameMode *gameMode);
float sinPath(int velocity, int time, GameMode *gameMode);
float curvePath(int positionX, int positionY, GameMode *gameMode);
void drawLine(GameMode *gameMode, double *calculationTime);

Shape::Shape(int posX, int posY) : positionX(posX), positionY(posY) {}

int Shape::getX()
{
    return positionX;
}

int Shape::getY()
{
    return positionY;
}

RectangularShape::RectangularShape(int posX, int posY, int wid, int hei) : Shape(posX, posY), width(wid), height(hei) {}

int RectangularShape::getWidth()
{
    return width;
}

int RectangularShape::getHeight()
{
    return height;
}

bool RectangularShape::checkCollision(Vector2 mousePoint)
{
    return CheckCollisionPointRec(mousePoint, Rectangle{(float)positionX, (float)positionY, (float)width, (float)height});
}

Ball::Ball(GameMode gM, double *cT)
    : Shape(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), gameMode(gM), calculationTime(cT)
{
    choose();
    round = 0;
    radius = 10;
    color1 = STEEL_BLUE;
    color2 = TIFFANY_BLUE;
    color3 = SEASALT;
    reset();
}

Ball::Ball(double *cT)
    : Shape(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), velocityX(300), velocityY(300), accelerationX(0), accelerationY(0), calculationTime(cT)
{
    int random = GetRandomValue(1, 3);
    gameMode.path = (random == 1 ? Path::Regular : random == 2 ? Path::Sin
                                                               : Path::Curve);
    gameMode.difficulty = Difficulty::Easy;
    round = 0;
    radius = 10;
    color1 = STEEL_BLUE;
    color2 = TIFFANY_BLUE;
    color3 = SEASALT;
    reset();
}

void Ball::draw()
{
    float rotationAngle = round * 0.1f;

    double temporaryTime = time(NULL);

    for (int i = 0; i < 6; i++)
    {
        float segment;
        float startAngle;
        float endAngle;
        float endX;
        float endY;

        if (gameMode.program == Program::Cpp)
        {
            segment = i * PI / 3;
            startAngle = rotationAngle + segment;
            endAngle = rotationAngle + segment + PI / 3;
            endX = (float)positionX + radius * cos(startAngle);
            endY = (float)positionY + radius * sin(startAngle);
        }
        else
        {
            segment = SE(i);
            startAngle = SA(rotationAngle, segment);
            endAngle = EA(rotationAngle, segment);
            endX = EX(positionX, radius, startAngle);
            endY = EY(positionY, radius, startAngle);
        }

        Vector2 start = {(float)positionX, (float)positionY};
        Vector2 end = {endX, endY};

        DrawCircleSector(
            Vector2{(float)positionX, (float)positionY},
            radius,
            startAngle * RAD2DEG,
            endAngle * RAD2DEG,
            32,
            (i % 2 == 0 ? color1 : color2));
        DrawLineEx(start, end, 1.0f, color3);
    }

    *calculationTime += time(NULL) - temporaryTime;
}

void Ball::update(Player *player1, Player *player2)
{
    path();

    if (positionX - radius <= 0)
    {
        player2->updateScore(1);
        reset();
    }
    else if (positionX + radius >= SCREEN_WIDTH)
    {
        player1->updateScore(1);
        reset();
    }
    if (positionY - radius <= 0)
    {
        positionY = radius;
        velocityY *= -1;
    }
    else if (positionY + radius >= SCREEN_HEIGHT)
    {
        positionY = SCREEN_HEIGHT - radius;
        velocityY *= -1;
    }

    if (conrner())
    {
        reset();
        choose();
    }
}

void Ball::update()
{
    path();

    if (positionX - radius <= 0)
    {
        positionX = radius;
        velocityX *= -1;
    }
    else if (positionX + radius >= SCREEN_WIDTH)
    {
        positionX = SCREEN_WIDTH - radius;
        velocityX *= -1;
    }
    if (positionY - radius <= 0)
    {
        positionY = radius;
        velocityY *= -1;
    }
    else if (positionY + radius >= SCREEN_HEIGHT)
    {
        positionY = SCREEN_HEIGHT - radius;
        velocityY *= -1;
    }

    if (conrner())
    {
        reset();
        choose();
    }
}

void Ball::path()
{
    double temporaryTime = time(NULL);
    velocityX += accelerationX / FPS;
    velocityY += accelerationY / FPS;

    float deltaX;
    float deltaY;

    switch (gameMode.path)
    {
    case Path::Regular:
        deltaX = regularPath(velocityX, &gameMode);
        deltaY = regularPath(velocityY, &gameMode);
        break;

    case Path::Sin:
        deltaX = regularPath(velocityX, &gameMode);
        deltaY = sinPath(velocityY, round, &gameMode);
        break;

    case Path::Curve:
        accelerationY += curvePath(positionX, positionY, &gameMode);
        deltaX = regularPath(velocityX, &gameMode);
        deltaY = regularPath(velocityY, &gameMode);
        break;

    default:
        break;
    }

    positionX += deltaX;
    positionY += deltaY;
    round++;

    *calculationTime += time(NULL) - temporaryTime;
}

void Ball::collision(Paddle paddle)
{
    if (CheckCollisionCircleRec(Vector2{(float)positionX, (float)positionY},
                                radius,
                                Rectangle{(float)paddle.getX(), (float)paddle.getY(), (float)paddle.getWidth(), (float)paddle.getHeight()}))
    {
        velocityX *= -1;
    }
}

void Ball::reset()
{
    positionX = SCREEN_WIDTH / 2;
    positionY = SCREEN_HEIGHT / 2;
}

void Ball::choose()
{
    switch (gameMode.difficulty)
    {
    case Difficulty::Easy:
        velocityX = 300;
        velocityY = 300;
        accelerationX = 20;
        accelerationY = 20;
        break;
    case Difficulty::Meduim:
        velocityX = 350;
        velocityY = 350;
        accelerationX = 30;
        accelerationY = 30;
        break;
    case Difficulty::Hard:
        velocityX = 400;
        velocityY = 400;
        accelerationX = 40;
        accelerationY = 40;
        break;
    default:
        break;
    }

    int random[2] = {-1, 1};
    velocityX *= random[GetRandomValue(0, 1)];
    velocityY *= random[GetRandomValue(0, 1)];
}

bool Ball::conrner()
{
    return (positionX - radius <= 0 || positionX + radius >= SCREEN_WIDTH) &&
           (positionY - radius <= 0 || positionY + radius >= SCREEN_HEIGHT);
}

Paddle::Paddle(int posX, int posY)
    : RectangularShape(posX, posY - 50, 20, 100), velocityY(5), accelerationY(0)
{
    padding = 5;
    color = HUNYADI_YELLOW;
}

void Paddle::limitCheck()
{
    if (positionY < padding)
    {
        positionY = padding;
    }
    else if (positionY + height > SCREEN_HEIGHT - padding)
    {
        positionY = SCREEN_HEIGHT - height - padding;
    }
}

RightPaddle::RightPaddle(int posX, int posY, bool AI)
    : Paddle(posX, posY), isAI(AI)
{
    positionX -= padding;
    positionX -= width;
}

void RightPaddle::draw()
{
    DrawRectangleRounded(Rectangle{(float)positionX, (float)positionY, (float)width, (float)height}, 0.8, 0, color);
}

void RightPaddle::update(Ball ball)
{
    if (isAI && ball.getX() > SCREEN_WIDTH / 2)
    {
        if (positionY + height / 2 > ball.getY())
        {
            positionY -= velocityY;
        }
        else if (positionY + height / 2 < ball.getY())
        {
            positionY += velocityY;
        }
    }
    else
    {
        if (IsKeyDown(KEY_UP))
        {
            positionY -= velocityY;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            positionY += velocityY;
        }
    }
    limitCheck();
}

LeftPaddle::LeftPaddle(int posX, int posY)
    : Paddle(posX, posY)
{
    positionX += padding;
}

void LeftPaddle::draw()
{
    DrawRectangleRounded(Rectangle{(float)positionX, (float)positionY, (float)width, (float)height}, 0.8, 0, color);
}

void LeftPaddle::update()
{
    if (IsKeyDown(KEY_W))
    {
        positionY -= velocityY;
    }
    else if (IsKeyDown(KEY_S))
    {
        positionY += velocityY;
    }
    limitCheck();
}

Player::Player(const char *name) : score(0)
{
    strncpy(username, name, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';
}

void Player::updateScore(int delta)
{
    score += delta;
}

int Player::getScore()
{
    return score;
}

char *Player::getName()
{
    return username;
}

void Player::setName(char name[20])
{
    strcpy(username, name);
}

Clickable::Clickable(Color foc, Color nor) : isFocus(false), focus(foc), normal(nor) {}

void Clickable::toggleFocus()
{
    isFocus = !isFocus;
}

void Clickable::setFocus(bool status)
{
    isFocus = status;
}

bool Clickable::getFocus()
{
    return isFocus;
}

TextBox::TextBox(int posX, int posY, const char *titleName, bool passwordStatus)
    : RectangularShape(posX, posY, 200, 30),
      Clickable(STEEL_BLUE, PALE_AZURE),
      fill(SEASALT), textColor(TIFFANY_BLUE), length(0), isPassword(passwordStatus)
{
    strcpy(title, titleName);
    text[0] = '\0';
}

void TextBox::addChar(int key)
{
    if (isFocus && length < 99)
    {
        text[length] = (char)key;
        length++;
        text[length] = '\0';
    }
}

void TextBox::deleteChar()
{
    if (isFocus && length > 0)
    {
        length--;
        text[length] = '\0';
    }
}

char *TextBox::getText()
{
    char *textCopy = (char *)malloc(100 * sizeof(char));
    strcpy(textCopy, text);
    return textCopy;
}

int TextBox::getLength()
{
    return length;
}

void TextBox::draw()
{
    DrawText(title, positionX - MeasureText(title, 20) / 2, positionY - 25, 20, textColor);
    DrawRectangleRec(Rectangle{(float)positionX - width / 2, (float)positionY, (float)width, (float)height}, fill);
    DrawRectangleLines(positionX - width / 2, positionY, width, height, isFocus ? focus : normal);
    if (isPassword)
    {
        char hiddenPassword[100] = "";
        for (int i = 0; i < length; i++)
            hiddenPassword[i] = '*';
        hiddenPassword[length] = '\0';
        DrawText(hiddenPassword, positionX - MeasureText(hiddenPassword, 20) / 2, positionY + 5, 20, textColor);
    }
    else
    {
        DrawText(text, positionX - MeasureText(text, 20) / 2, positionY + 5, 20, textColor);
    }
}

Button::Button(int posX, int posY, const char *titleName)
    : RectangularShape(posX, posY, 100, 30),
      Clickable(STEEL_BLUE, PALE_AZURE),
      fill(HUNYADI_YELLOW), textColor(TIFFANY_BLUE)
{
    strcpy(title, titleName);
}

void Button::draw()
{
    DrawRectangleRec(Rectangle{(float)positionX - width / 2, (float)positionY, (float)width, (float)height}, fill);
    DrawRectangleLines(positionX - width / 2, positionY, width, height, isFocus ? focus : normal);
    DrawText(title, positionX - MeasureText(title, 20) / 2, positionY + 5, 20, textColor);
}

Text::Text(const char *txt, Color col, int posX, int posY) : Shape(posX, posY), color(col)
{
    strncpy(text, txt, sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';
}

void Text::draw()
{
    DrawText(text, positionX - MeasureText(text, 20) / 2, positionY, 20, color);
}

void Text::updateText(char *txt)
{
    strncpy(text, txt, sizeof(text) - 1);
    text[sizeof(text) - 1] = '\0';
}

CheckBox::CheckBox(int posX, int posY, const char *titleName)
    : RectangularShape(posX, posY, 30, 30),
      Clickable(STEEL_BLUE, PALE_AZURE),
      unChecked(ASH_GRAY),
      checked(PANTONE),
      textColor(TIFFANY_BLUE),
      isChecked(false)
{
    strcpy(title, titleName);
}

void CheckBox::draw()
{
    DrawRectangleRec(Rectangle{(float)positionX - width / 2, (float)positionY, (float)width, (float)height}, isChecked ? checked : unChecked);
    DrawRectangleLines(positionX - width / 2, positionY, width, height, isFocus ? focus : normal);
    DrawText(title, positionX + width + 5, positionY + 5, 20, textColor);
}

void CheckBox::toggleCheck()
{
    isChecked = !isChecked;
}

void CheckBox::setCheck(bool status)
{
    isChecked = status;
}

bool CheckBox::getCheck()
{
    return isChecked;
}
bool checkLogin(TextBox *username, TextBox *password, Button *login)
{
    if (username->getLength() != 0 && password->getLength() != 0)
    {
        return true;
    }
    else
    {
        username->setFocus(true);
        password->setFocus(false);
        login->setFocus(false);
        return false;
    }
}

bool loginMenu(Player *player, double *calculationTime)
{
    Text title("LOGIN MENU", PANTONE, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 200);
    TextBox username(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, "USERNAME");
    TextBox password(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "PASSWORD", true);
    Button login(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 100, "Login");
    username.setFocus(true);
    password.setFocus(false);
    login.setFocus(false);

    bool loginStatus = false;

    Ball ball1(calculationTime);
    Ball ball2(calculationTime);

    while (!WindowShouldClose() && !loginStatus)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            username.addChar(key);
            password.addChar(key);
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE))
        {
            username.deleteChar();
            password.deleteChar();
        }

        if (IsKeyPressed(KEY_TAB))
        {
            if (username.getFocus())
            {
                username.setFocus(false);
                password.setFocus(true);
            }
            else if (password.getFocus())
            {
                password.setFocus(false);
                login.setFocus(true);
            }
            else if (login.getFocus())
            {
                login.setFocus(false);
                username.setFocus(true);
            }
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (username.getFocus())
            {
                username.toggleFocus();
                password.toggleFocus();
            }
            else
            {
                loginStatus = checkLogin(&username, &password, &login);
            }
        }

        Vector2 mousePoint = GetMousePosition();
        if (login.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            loginStatus = checkLogin(&username, &password, &login);
        }
        else if (username.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            username.setFocus(true);
            password.setFocus(false);
        }
        else if (password.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            username.setFocus(false);
            password.setFocus(true);
        }

        BeginDrawing();
        ClearBackground(ASH_GRAY);
        ball1.update();
        ball1.draw();
        ball2.update();
        ball2.draw();

        title.draw();
        username.draw();
        password.draw();
        login.draw();

        EndDrawing();
    }

    player->setName(username.getText());
    return loginStatus;
}

bool checkMainMenuSelections(CheckBox *singlePlayer, CheckBox *multiPlayer,
                             CheckBox *regular, CheckBox *sin, CheckBox *curve,
                             CheckBox *easy, CheckBox *medium, CheckBox *hard,
                             CheckBox *cpp, CheckBox *assembly)
{
    bool hasGameMode = singlePlayer->getCheck() || multiPlayer->getCheck();
    bool hasPath = regular->getCheck() || sin->getCheck() || curve->getCheck();
    bool hasDifficulty = easy->getCheck() || medium->getCheck() || hard->getCheck();
    bool hasLanguage = cpp->getCheck() || assembly->getCheck();
    return hasGameMode && hasPath && hasDifficulty && hasLanguage;
}

bool mainMenu(GameMode *gameMode)
{
    Text title("MAIN MENU", PANTONE, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 375);
    Text guide("Choose the game mode:", LAPIS_LAZULI, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 325);
    Text ballPath("Choose the ball's path:", LAPIS_LAZULI, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 175);
    Text difficulty("Choose game's difficulty:", LAPIS_LAZULI, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 25);
    Text hotPart("Choose the language used in hot patrs:", LAPIS_LAZULI, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 125);

    CheckBox singlePlayer(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 250, "SINGLEPLAYER");
    CheckBox multiPlayer(SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2 - 250, "MULTIPLAYER");
    singlePlayer.setCheck(true);
    singlePlayer.setFocus(true);

    CheckBox regular(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 100, "REGULAR");
    CheckBox sin(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100, "SIN");
    CheckBox curve(SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT / 2 - 100, "CURVE");
    regular.setCheck(true);

    CheckBox easy(SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 + 50, "EASY");
    CheckBox medium(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, "MEDIUM");
    CheckBox hard(SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT / 2 + 50, "HARD");
    easy.setCheck(true);

    CheckBox cpp(SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 200, "C++");
    CheckBox assembly(SCREEN_WIDTH / 2 + 200, SCREEN_HEIGHT / 2 + 200, "ASSEMBLY");
    cpp.setCheck(true);

    Button startGame(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 275, "Start");

    bool start = false;

    while (!WindowShouldClose() && !start)
    {
        if (IsKeyPressed(KEY_TAB))
        {
            if (singlePlayer.getFocus())
            {
                singlePlayer.setFocus(false);
                multiPlayer.setFocus(true);
            }
            else if (multiPlayer.getFocus())
            {
                multiPlayer.setFocus(false);
                regular.setFocus(true);
            }
            else if (regular.getFocus())
            {
                regular.setFocus(false);
                sin.setFocus(true);
            }
            else if (sin.getFocus())
            {
                sin.setFocus(false);
                curve.setFocus(true);
            }
            else if (curve.getFocus())
            {
                curve.setFocus(false);
                easy.setFocus(true);
            }
            else if (easy.getFocus())
            {
                easy.setFocus(false);
                medium.setFocus(true);
            }
            else if (medium.getFocus())
            {
                medium.setFocus(false);
                hard.setFocus(true);
            }
            else if (hard.getFocus())
            {
                hard.setFocus(false);
                cpp.setFocus(true);
            }
            else if (cpp.getFocus())
            {
                cpp.setFocus(false);
                assembly.setFocus(true);
            }
            else if (assembly.getFocus())
            {
                assembly.setFocus(false);
                startGame.setFocus(true);
            }
            else if (startGame.getFocus())
            {
                startGame.setFocus(false);
                singlePlayer.setFocus(true);
            }
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (singlePlayer.getFocus())
            {
                singlePlayer.toggleCheck();
                if (singlePlayer.getCheck())
                {
                    multiPlayer.setCheck(false);
                }
            }
            else if (multiPlayer.getFocus())
            {
                multiPlayer.toggleCheck();
                if (multiPlayer.getCheck())
                {
                    singlePlayer.setCheck(false);
                }
            }
            else if (regular.getFocus())
            {
                regular.toggleCheck();
                if (regular.getCheck())
                {
                    sin.setCheck(false);
                    curve.setCheck(false);
                }
            }
            else if (sin.getFocus())
            {
                sin.toggleCheck();
                if (sin.getCheck())
                {
                    regular.setCheck(false);
                    curve.setCheck(false);
                }
            }
            else if (curve.getFocus())
            {
                curve.toggleCheck();
                if (curve.getCheck())
                {
                    regular.setCheck(false);
                    sin.setCheck(false);
                }
            }
            else if (easy.getFocus())
            {
                easy.toggleCheck();
                if (easy.getCheck())
                {
                    medium.setCheck(false);
                    hard.setCheck(false);
                }
            }
            else if (medium.getFocus())
            {
                medium.toggleCheck();
                if (medium.getCheck())
                {
                    easy.setCheck(false);
                    hard.setCheck(false);
                }
            }
            else if (hard.getFocus())
            {
                hard.toggleCheck();
                if (hard.getCheck())
                {
                    easy.setCheck(false);
                    medium.setCheck(false);
                }
            }
            else if (cpp.getFocus())
            {
                cpp.toggleCheck();
                if (cpp.getCheck())
                {
                    assembly.setCheck(false);
                }
            }
            else if (assembly.getFocus())
            {
                assembly.toggleCheck();
                if (assembly.getCheck())
                {
                    cpp.setCheck(false);
                }
            }
            else if (startGame.getFocus())
            {
                start = checkMainMenuSelections(&singlePlayer, &multiPlayer,
                                                &regular, &sin, &curve,
                                                &easy, &medium, &hard,
                                                &cpp, &assembly);
            }
        }

        Vector2 mousePoint = GetMousePosition();
        if (startGame.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            start = checkMainMenuSelections(&singlePlayer, &multiPlayer,
                                            &regular, &sin, &curve,
                                            &easy, &medium, &hard,
                                            &cpp, &assembly);
        }
        else if (singlePlayer.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(true);
            singlePlayer.setCheck(true);
            multiPlayer.setFocus(false);
            multiPlayer.setCheck(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
        }
        else if (multiPlayer.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            singlePlayer.setCheck(false);
            multiPlayer.setFocus(true);
            multiPlayer.setCheck(true);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
        }
        else if (regular.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(true);
            regular.setCheck(true);
            sin.setFocus(false);
            sin.setCheck(false);
            curve.setFocus(false);
            curve.setCheck(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
        }
        else if (sin.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            regular.setCheck(false);
            sin.setFocus(true);
            sin.setCheck(true);
            curve.setFocus(false);
            curve.setCheck(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
        }
        else if (curve.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            regular.setCheck(false);
            sin.setFocus(false);
            sin.setCheck(false);
            curve.setFocus(true);
            curve.setCheck(true);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
        }
        else if (easy.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(true);
            easy.setCheck(true);
            medium.setFocus(false);
            medium.setCheck(false);
            hard.setFocus(false);
            hard.setCheck(false);
        }
        else if (medium.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            easy.setCheck(false);
            medium.setFocus(true);
            medium.setCheck(true);
            hard.setFocus(false);
            hard.setCheck(false);
        }
        else if (hard.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            easy.setCheck(false);
            medium.setFocus(false);
            medium.setCheck(false);
            hard.setFocus(true);
            hard.setCheck(true);
        }
        else if (cpp.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
            cpp.setFocus(true);
            cpp.setCheck(true);
            assembly.setFocus(false);
            assembly.setCheck(false);
        }
        else if (assembly.checkCollision(mousePoint) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            singlePlayer.setFocus(false);
            multiPlayer.setFocus(false);
            regular.setFocus(false);
            sin.setFocus(false);
            curve.setFocus(false);
            easy.setFocus(false);
            medium.setFocus(false);
            hard.setFocus(false);
            cpp.setFocus(false);
            cpp.setCheck(false);
            assembly.setFocus(true);
            assembly.setCheck(true);
        }

        BeginDrawing();
        ClearBackground(CAROLINA_BLUE);

        title.draw();
        guide.draw();
        ballPath.draw();
        difficulty.draw();
        hotPart.draw();
        singlePlayer.draw();
        multiPlayer.draw();
        regular.draw();
        sin.draw();
        curve.draw();
        easy.draw();
        medium.draw();
        hard.draw();
        cpp.draw();
        assembly.draw();
        startGame.draw();

        EndDrawing();
    }

    if (multiPlayer.getCheck())
    {
        gameMode->numberOfPlayer = 2;
    }

    if (sin.getCheck())
    {
        gameMode->path = Path::Sin;
    }
    else if (curve.getCheck())
    {
        gameMode->path = Path::Curve;
    }

    if (medium.getCheck())
    {
        gameMode->difficulty = Difficulty::Meduim;
    }
    else
    {
        gameMode->difficulty = Difficulty::Hard;
    }

    if (assembly.getCheck())
    {
        gameMode->program = Program::Assembly;
    }

    return singlePlayer.getCheck();
}

bool game(Player *player1, Player *player2, GameMode *gameMode, double *calculationTime)
{
    Ball ball(*gameMode, calculationTime);
    LeftPaddle leftPaddle(0, SCREEN_HEIGHT / 2);
    RightPaddle rightPaddle(SCREEN_WIDTH, SCREEN_HEIGHT / 2, gameMode->numberOfPlayer == 1);

    while (!WindowShouldClose())
    {
        ball.update(player1, player2);
        leftPaddle.update();
        rightPaddle.update(ball);
        ball.collision(leftPaddle);
        ball.collision(rightPaddle);

        BeginDrawing();
        ClearBackground(CAROLINA_BLUE);

        drawLine(gameMode, calculationTime);

        ball.draw();
        leftPaddle.draw();
        rightPaddle.draw();
        DrawText(player1->getName(), 10, 10, 20, LAPIS_LAZULI);
        DrawText(player2->getName(), SCREEN_WIDTH - 100, 10, 20, LAPIS_LAZULI);
        DrawText(TextFormat("%i", player1->getScore()), 10, 40, 20, LAPIS_LAZULI);
        DrawText(TextFormat("%i", player2->getScore()), SCREEN_WIDTH - 100, 40, 20, LAPIS_LAZULI);
        EndDrawing();
    }

    return true;
}

float regularPath(int velocity, GameMode *gameMode)
{
    if (gameMode->program == Program::Cpp)
    {
        return velocity / (float)FPS;
    }
    else
    {
        return R(velocity);
    }
}

float sinPath(int velocity, int time, GameMode *gameMode)
{
    if (gameMode->program == Program::Cpp)
    {
        const float frequency = 0.05f;
        float baseMovement = velocity / FPS;
        float sineComponent = sin(frequency * time);
        return baseMovement * sineComponent;
    }
    else
    {
        return S(velocity, time);
    }
}

float curvePath(int positionX, int positionY, GameMode *gameMode)
{
    if (gameMode->program == Program::Cpp)
    {
        const float constant = 1000;
        positionX -= SCREEN_WIDTH / 2;
        positionY -= SCREEN_HEIGHT / 2;
        float norm = positionX * positionX + positionY * positionY;
        if (norm < 25)
        {
            return 0;
        }
        else
        {
            return constant * positionY / norm;
        }
    }
    else
    {
        return C(positionX, positionY);
    }
}

void drawLine(GameMode *gameMode, double *calculationTime)
{
    DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, PANTONE);
    Color color = CAROLINA_BLUE;
    int radius = 128;
    float delta = 0.1f;
    
    double temporaryTime = time(NULL);

    for (float i = radius; i > 0; i -= delta)
    {
        Color gradientColor;
        if (gameMode->program == Program::Cpp)
        {
            gradientColor = {
                (unsigned char)fmin(color.r + (radius - i) * 0.5, 255),
                (unsigned char)fmin(color.g + (radius - i) * 0.5, 255),
                (unsigned char)fmin(color.b + (radius - i) * 0.5, 255),
                color.a};
        }
        else
        {
            gradientColor = {
                (unsigned char)G(color.r, i),
                (unsigned char)G(color.g, i),
                (unsigned char)G(color.b, i),
                color.a};
        }

        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, i, gradientColor);
    }

    *calculationTime += time(NULL) - temporaryTime;

    DrawCircleLines(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius, PANTONE);
}


double executionTime = 0;
double calculationTime = 0;

int main()
{
    double temporaryTime = time(NULL);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_NAME);
    SetTargetFPS(FPS);

    GameMode gameMode = {
        .numberOfPlayer = 1,
        .path = Path::Regular,
        .difficulty = Difficulty::Easy,
        .program = Program::Assembly};

    Player player1("Player1");
    Player player2("AI");

    bool start = false;
    start = mainMenu(&gameMode);

    game(&player1, &player2, &gameMode, &calculationTime);

    CloseWindow();

    double executionTime = time(NULL) - temporaryTime;

    FILE *logFile = fopen("log.txt", "a");
    fprintf(logFile,
            "Execution time is %.0f seconds.\nCalculation time while using %s is %.9f nano seconds.\n",
            executionTime,
            (gameMode.program == Program::Cpp ? "C++" : "ASSEMBLY"),
            calculationTime * 1000000000);
    fclose(logFile);

    return 0;
}

