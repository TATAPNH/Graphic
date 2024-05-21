#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#include "../stb-master/stb_easy_font.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"
#define bool int
#define true 1
#define false 0
#define Y 18
#define X 40
#define TILE_SIZE 32.0f
#include <float.h>


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


char TileMap[Y][X] = {
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B        AAAAAAAAAAAA            AAAAAAB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "BAAAAA                  AAAAAAA        B",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "B        AAAA                     AAAAAB",
    "B                                      B",
    "B                                      B",
    "B                                      B",
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
};

void DrawCollision() {
    const float blockSize = 32.0f;
    for (int i = 0; i < Y; ++i) {
        for (int j = 0; j < X; ++j) {
            char tile = TileMap[i][j];
            glColor3f(1.0f, 1.0f, 1.0f);
            switch(tile) {
                case 'B':
                    glColor3f(0.5f, 0.2f, 0.1f);
                    break;
                case 'A':
                    glColor3f(0.7f, 0.6f, 0.0f);
                    break;

                default:
                    continue;
            }

            glBegin(GL_QUADS);
                glVertex2f(j * blockSize, i * blockSize);
                glVertex2f((j + 1) * blockSize, i * blockSize);
                glVertex2f((j + 1) * blockSize, (i + 1) * blockSize);
                glVertex2f(j * blockSize, (i + 1) * blockSize);
            glEnd();
        }
    }
}

int jumpFrame = 0;
int jumpAnimationPlaying = 0;

bool gameStarted = false;
float gravity = -15.0f;
float verticalVelocity = 0.0f;
float jumpSpeed = 60.0f;
int isMoving = 0;
bool isJumping = false;
bool isAirborne = false;
bool isFlipped = false;

typedef struct {
    float x, y;
    float dx, dy;
    bool isAirborne;
    bool isMoving;
    float width, height;
    bool jumpPeakReached;
    bool isFlipped;
} Hero;
Hero hero = { .x = 0.0f, .y = 0.0f, .dx = 0.0f, .dy = 0.0f, .isAirborne = true, .width = 45.0f, .height = 80.0f };

float groundLevel = 0.0f;
bool isWallHit = false;
bool CheckCollision(float newX, float newY, Hero *hero, bool* isCeilingHit) {
    *isCeilingHit = false;
    int leftTile = (int)(newX / TILE_SIZE);
    int rightTile = (int)((newX + hero->width) / TILE_SIZE) + 1;
    int topTile = (int)(newY / TILE_SIZE);
    int bottomTile = (int)((newY + hero->height) / TILE_SIZE) + 1;

    for (int y = topTile; y < bottomTile; y++) {
        for (int x = leftTile; x < rightTile; x++) {
            char tile = TileMap[y][x];

            if (tile == 'B') {
                isWallHit = true;
                if (newX + hero->width > x * TILE_SIZE && newX + hero->width < (x + 1) * TILE_SIZE) {
                    hero->x -=hero->width/100;
                    hero->y=groundLevel;
                    hero->dx=0;
                    hero->dy=0;
                }
                if (newX < (x + 1) * TILE_SIZE && newX > x * TILE_SIZE) {
                    hero->x = (x + 1) * TILE_SIZE;
                    hero->y=groundLevel;
                    hero->dx=0;
                    hero->dy=0;
                }

    return true;
            } else {
                isWallHit = false;
            }

            if (tile == 'A'||tile == 'B') {
                if (newY + hero->height > y * TILE_SIZE && newY < (y + 1) * TILE_SIZE) {
                        if (newY + hero->height <= (y + 1) * TILE_SIZE) {
                    hero->y = y * TILE_SIZE - hero->height - 5;
                    hero->dy = 0;
                    *isCeilingHit = false;
                    return true;
                    }
                    else if (newY + hero->height > (y + 1) * TILE_SIZE) {
                        hero->y = (y + 1) * TILE_SIZE;
                        *isCeilingHit = true;
                        verticalVelocity = 0;
                        jumpAnimationPlaying = 0;
                            hero->dy=0;
                        hero->isAirborne = false;
                        return true;
                    }

                }
            }
        }
    }
    return false;
}
bool isSolidTileAt(float x, float y) {
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);
    if (tileX < 0 || tileX >= X || tileY < 0 || tileY >= Y) {
        return false;
    }
    char tile = TileMap[tileY][tileX];
    return tile == 'A' || tile == 'B';
}

void UpdateGroundLevel(Hero* hero) {
    float nearestGround = FLT_MAX;
    bool groundFound = false;

    float feetY = hero->y + hero->height;

    int tileXStart = (int)(hero->x / TILE_SIZE);
    int tileXEnd = (int)((hero->x + hero->width) / TILE_SIZE);

    for (int x = tileXStart; x <= tileXEnd; x++) {
        for (int y = (int)(feetY / TILE_SIZE); y < X; y++) {
            if (isSolidTileAt(x * TILE_SIZE, y * TILE_SIZE)) {
                float groundY = y * TILE_SIZE - hero->height;
                if (groundY < nearestGround) {
                    nearestGround = groundY;
                    groundFound = true;
                }
                break;
            }
        }
    }

    if (groundFound) {
        groundLevel = nearestGround;
    } else {
        groundLevel = FLT_MAX;
    }
}

float maxYVelocity = 47.0f;
void UpdateHeroPosition(Hero *hero, float deltaTime) {
    UpdateGroundLevel(hero);
    if (hero->dx < 0) {
        isFlipped = true;
    } if (hero->dx > 0) {
        isFlipped = false;
    }
    float potentialNewX = hero->x + hero->dx * deltaTime;
    float potentialNewY = hero->y + hero->dy * deltaTime;

    bool isCeilingHit = false;
    if (!CheckCollision(potentialNewX, hero->y, hero, &isCeilingHit)) {
        hero->x = potentialNewX;
    } else {
        if (isWallHit) {
            hero->dx = 0;
        }
    }
    hero->dy -= gravity * deltaTime;
    if (!CheckCollision(hero->x, potentialNewY, hero, &isCeilingHit)) {
        hero->y = potentialNewY;
        hero->isAirborne = true;
    } else {
        if (isWallHit) {
            hero->dy = 0;
        }
        else {
            hero->y += 5;
            hero->isAirborne = false;
            verticalVelocity = 0;
            jumpAnimationPlaying = 0;
        }
    }

    if (hero->dy > maxYVelocity) {
        hero->dy = maxYVelocity;
    } else if (hero->dy < -maxYVelocity) {
        hero->dy = -maxYVelocity;
    }

    if (hero->y >= groundLevel && !isWallHit) {
        hero->isAirborne = false;
        hero->dy = 0;
    } else {
        hero->isAirborne = true;
    }


}

void spriteAnimation(GLuint texture, float posX, float posY, float width, float height, float scale, int currentFrame, bool isFlipped) {

        float frameWidth = 1.0f / 8;
        float texLeft = currentFrame * frameWidth;
        float texRight = texLeft + frameWidth;
        float scaledWidth = width * scale;
        float scaledHeight = height * scale;

        if (isFlipped) {
            float temp = texLeft;
            texLeft = texRight;
            texRight = temp;
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glColor3f(1,1,1);
        glBegin(GL_QUADS);
            glTexCoord2f(texLeft, 0.0f); glVertex2f(posX, posY);
            glTexCoord2f(texRight, 0.0f); glVertex2f(posX + scaledWidth, posY);
            glTexCoord2f(texRight, 1.0f); glVertex2f(posX + scaledWidth, posY + scaledHeight);
            glTexCoord2f(texLeft, 1.0f); glVertex2f(posX, posY + scaledHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);

}

void Background(GLuint texture)
{
    static float vertices[] = {0.0f, 0.0f,  1296.0f, 0.0f,  1296.0f, 600.0f,  0.0f, 600.0f};
    static float TexCord[] = {0, 0,  1, 0,  1, 1,  0, 1};

    glClearColor(0, 0, 0, 0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, TexCord);

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_TEXTURE_2D);
}
int currentFrame = 0;
const int totalFrames = 8;

void UpdateAnimationFrame() {

        currentFrame = (currentFrame + 1) % totalFrames;

}

GLuint LoadTexture(const char *filename)
{
    int width, height, cnt;
    unsigned char *image = stbi_load(filename, &width, &height, &cnt, 0);
    if (image == NULL) {-+
        printf("Error in loading the image: %s\n", stbi_failure_reason());
        exit(1);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return textureID;
}

GLuint textureSprite1, textureSprite2, textureSprite3, textureBackground;

void Init(HWND hwnd)
{

     textureSprite1 = LoadTexture("walking.png");
     textureSprite2 = LoadTexture("jumping.png");
     textureSprite3 = LoadTexture("afk.png");
     textureBackground = LoadTexture("background.png");

     RECT rct;
     GetClientRect(hwnd, &rct);
     groundLevel = rct.bottom - 10;

     hero.dy = 0.0f;
     hero.x = 60.0f;
     hero.y = groundLevel - 100;
     hero.isAirborne = false;
}

void print_string(float x, float y, char *text, float r, float g, float b)
{
  static char buffer[99999]; // ~500 chars
  int num_quads;

  num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

  glColor3f(r,g,b);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 16, buffer);
  glDrawArrays(GL_QUADS, 0, num_quads*4);
  glDisableClientState(GL_VERTEX_ARRAY);
}

unsigned int texture;

void Tex_Init()
{
    int texwidth, texheight, cnt;
    unsigned char *data = stbi_load("reference.png", &texwidth, &texheight, &cnt, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight,
                                    0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}

float vertex[] = {-1,-1,0, 1,-1,0, 1,1,0, -1,1,0};
float texCoord[] = {0,1, 1,1, 1,0, 0,0};

void Tex_Menu()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1,1,1);
    glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, vertex);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
}


int width, height;

typedef struct{
    char name[20];
    float vert[8];
    BOOL hover;
} TButton;

TButton btn[] = {
    {"start", {0,0, 60,0, 60,20, 0,20}, FALSE},
    {"stop", {70,0, 120,0, 120,20, 70,20}, FALSE},
    {"quit", {130,0, 180,0, 180,20, 130,20}, FALSE},
};
int btnCnt = sizeof(btn) / sizeof(btn[0]);

void TButton_Show(TButton btn)
{
    glEnableClientState(GL_VERTEX_ARRAY);
        if (btn.hover) glColor3f(1,0,1);
        else glColor3f(1,1,1);
        glVertexPointer(2, GL_FLOAT, 0, btn.vert);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
        glTranslatef(btn.vert[0], btn.vert[1], 0);
        glScalef(2, 2, 2);
        print_string(3, 1, btn.name, 0, 0, 0);
    glPopMatrix();
}

BOOL PointInButton(int x, int y, TButton btn)
{
    return  (x > btn.vert[0]) && (x < btn.vert[4]) &&
            (y > btn.vert[1]) && (y < btn.vert[5]);

}

//n == 4 ? GL_RGBA : GL_RGB

void ShowMenu()
{
    glPushMatrix();
        glLoadIdentity();
        glOrtho(0,width, height,0, -1,1);
        for (int i = 0; i < btnCnt; i++)
            TButton_Show(btn[i]);
    glPopMatrix();
}


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1296,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    Init(hwnd);
    RECT rct;
    glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            GetClientRect(hwnd, &rct);
            glOrtho(0, rct.right, rct.bottom, 0, 1, -1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
    DWORD lastUpdateTime = GetTickCount();



    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            DWORD currentTime = GetTickCount();
            float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
            lastUpdateTime = currentTime;

            glClearColor(0.5f, 0.2f, 0.1f, 0.7f);
            glClear(GL_COLOR_BUFFER_BIT);
            Background(textureBackground);
            DrawCollision();
            float centerX = rct.right / 2.0f;
            float posY = 150.0f;
            float spriteWidth = 80.0f;
            float spriteHeight = 80.0f;


                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glPushMatrix();
                        for(int i = 0; i < btnCnt; i++)
                            TButton_Show(btn[i]);
                    glPopMatrix();
                    if(gameStarted){



                    float spriteAspectRatio = (float)50 / (float)80;
                    float renderedSpriteWidth = spriteHeight * spriteAspectRatio;
                    UpdateHeroPosition(&hero, deltaTime);

                    if(!isAirborne && !isMoving ){
                            glPushMatrix();

                             UpdateAnimationFrame();

                    spriteAnimation(textureSprite3, hero.x, hero.y, renderedSpriteWidth, spriteHeight, 1.0f, currentFrame, isFlipped);
                    glPopMatrix();
                    }
                    if (isMoving) {
                            glPushMatrix();

                    UpdateAnimationFrame();
                    hero.x += hero.dx;
                    hero.y += hero.dy;
                     if (hero.x < 0) hero.x = 0;
                    if (hero.x > rct.right - spriteWidth) hero.x = rct.right - spriteWidth;
                    spriteAnimation(textureSprite1, hero.x, hero.y, renderedSpriteWidth, spriteHeight, 1.0f, currentFrame, isFlipped);
                    glPopMatrix();

                    } if (isAirborne) {
                    glPushMatrix();
                    if (!jumpAnimationPlaying) {
                        jumpFrame = 0;
                        jumpAnimationPlaying = 1;
                    }
                    UpdateAnimationFrame();

                    spriteAnimation(textureSprite2, hero.x, hero.y, renderedSpriteWidth, spriteHeight, 1.0f, currentFrame, isFlipped);

                    hero.y -= verticalVelocity;
                    verticalVelocity += gravity;

                    if (hero.y >= groundLevel) {
                        hero.y = groundLevel;
                        isAirborne = false;
                        verticalVelocity = 0;
                        jumpAnimationPlaying = 0;
                    }
                    glPopMatrix();
                }
                    }

    glDisable(GL_BLEND);

            glDisable(GL_BLEND);

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (80);
        }
    }

            //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            //glClear(GL_COLOR_BUFFER_BIT);

            //Tex_Menu();

            //glPushMatrix();
            //glRotatef(theta, 0.0f, 0.0f, 1.0f);

            //glPushMatrix();
            //glScalef(256,256,1);
            //glTranslatef(1,0,0);

            //glBegin(GL_TRIANGLES);

                //glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
                //glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
                //glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);

            //glEnd();

            //glPopMatrix();
            //glPopMatrix();



    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_LBUTTONDOWN:
            for(int i = 0; i < btnCnt; i++)
                if (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]))
                {
                    printf("%s\n", btn[i].name);
                    if(strcmp(btn[i].name, "quit") == 0)
                        PostQuitMessage(0);
                    else if (strcmp(btn[i].name, "start") == 0) {
                        gameStarted = TRUE;
                    } else if (strcmp(btn[i].name, "stop") == 0) {
                        gameStarted = FALSE;
                    }
                }
        break;

        case WM_KEYDOWN:

        switch(wParam) {
            case VK_LEFT:

                hero.dx = -15.0f;
                isMoving = true;
                isFlipped = true;
                break;
            case VK_RIGHT:

                hero.dx = 15.0f;
                isMoving = true;
                isFlipped = false;
                break;
            case VK_UP:
                  if (!isAirborne) {
                        isAirborne = true;
                        verticalVelocity = jumpSpeed;

        }
                break;

        }
        break;

    case WM_MOUSEMOVE:
        for(int i = 0; i < btnCnt; i++)
            btn[i].hover = (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]));

    break;

    case WM_KEYUP:
    if (wParam == VK_LEFT || wParam == VK_RIGHT) {
        hero.dx = 0.0f;
        isMoving = 0;
    }
    break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

