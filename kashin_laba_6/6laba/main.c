#include <windows.h>
#include <gl/gl.h>
#include <math.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

float vert[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};
float vertex[] = {1,1,0, 1,-1,0, -1,-1,0, -1,1,0};

float xAlfa = 10;
float zAlfa = 0;
POINTFLOAT pos = {0,0};

float normal_vert[]={0,0,1, 0,0,1, 0,0,1, 0,0,1};

void Draw()
{
    //glNormal3f(0,0,1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertex);
    glDrawArrays(GL_TRIANGLE_FAN, 0,4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void ShowWorld()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &vert);
    glNormalPointer(GL_FLOAT,0,&normal_vert);


        int n = 25;
        for (int i = -n; i < n; i++)
            for (int j = -n; j < n; j++)
            {
                glPushMatrix();
                    if ((i+j) % 2 == 0) glColor3f(1,1,1);
                    else glColor3f (0.2,0.2,0.2);
                    glTranslatef(i*2, j*2, 0);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                glPopMatrix();
            }
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex3f(-100.0f, 0.0f, 0.1f);
        glVertex3f(100.0f, 0.0f, 0.1f);
        glEnd();

        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, -100.0f, 0.1f);
        glVertex3f(0.0f, 100.0f, 0.1f);
        glEnd();

        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -100.0f);
        glVertex3f(0.0f, 0.0f, 100.0f);
        glEnd();


    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_NORMAL_ARRAY);
}



void Draw_Cube(){
GLfloat vertices[] = {
-0.5f, -0.5f, 0.0f,
0.5f, -0.5f, 0.0f,
0.5f, 0.5f, 0.0f,
-0.5f, 0.5f, 0.0f,
-0.5f, -0.5f, 1.0f,
0.5f, -0.5f, 1.0f,
0.5f, 0.5f, 1.0f,
-0.5f, 0.5f, 1.0f
};
GLuint indices[] = {
 0, 1, 2,
 2, 3, 0,
 1, 5, 6,
 6, 2, 1,
 7, 6, 5,
 5, 4, 7,
 4, 0, 3,
 3, 7, 4,
 4, 5, 1,
 1, 0, 4,
 3, 2, 6,
 6, 7, 3
};
GLfloat normals[] = {
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, 1.0f,
 0.0f, 0.0f, 1.0f,
 0.0f, 0.0f, 1.0f,
 0.0f, 0.0f, 1.0f,
 -1.0f, 0.0f, 0.0f,
 -1.0f, 0.0f, 0.0f,
 -1.0f, 0.0f, 0.0f,
 -1.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
 1.0f, 0.0f, 0.0f,
 0.0f, -1.0f, 0.0f,
 0.0f, -1.0f, 0.0f,
 0.0f, -1.0f, 0.0f,
 0.0f, -1.0f, 0.0f,
 0.0f, 1.0f, 0.0f,
 0.0f, 1.0f, 0.0f,
 0.0f, 1.0f, 0.0f,
 0.0f, 1.0f, 0.0f
};

glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, 0, vertices);
glEnableClientState(GL_NORMAL_ARRAY);
glNormalPointer(GL_FLOAT, 0, normals);
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_NORMAL_ARRAY);
}

void MoveCamera()
{
    if (GetKeyState(VK_UP) < 0) xAlfa = ++xAlfa > 180 ? 180 : xAlfa;
    if (GetKeyState(VK_DOWN) < 0) xAlfa = --xAlfa < 0 ? 0 : xAlfa;
    if (GetKeyState(VK_LEFT) < 0) zAlfa++;
    if (GetKeyState(VK_RIGHT) < 0) zAlfa--;
    float angle = -zAlfa / 180 * M_PI;
    float speed = 0;
    if (GetKeyState('W') < 0) speed = 0.1;
    if (GetKeyState('S') < 0) speed = -0.1;
    if (GetKeyState('A') < 0)
        {
            speed = 0.1;
            angle -= M_PI * 0.5;
        }
    if (GetKeyState('D') < 0)
        {
            speed = 0.1;
            angle += M_PI * 0.5;
        }


    if (speed != 0)
    {
        pos.x += sin(angle) * speed;
        pos.y += cos(angle) * speed;
    }
    glRotatef(-xAlfa, 1,0,0);
    glRotatef(-zAlfa, 0,0,1);
    glTranslatef(-pos.x,-pos.y,-3);
}



void Init_Light()
{
 glEnable(GL_LIGHTING); //общее освещения для всего пространства
 glShadeModel(GL_SMOOTH);
 GLfloat light_position[] = { 0.0f, 0.0f, 5.0f, 1.0f }; //позиция источника


 GLfloat light_spot_direction[] = {0.0, 0.0, -1.0, 1.0}; // позиция цели
 GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; //параметры
 GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //параметры
 GLfloat light_specular[] = { 0.2f, 0.2f, 0.2f, 32.0f }; //параметры
 glLightfv(GL_LIGHT0, GL_POSITION, light_position);
glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15); // конус для
//направленного источника
 glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spot_direction);
 glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 8.0); // экспонента
//убывания интенсивности
//задействование настроек для источника LIGHT0
 glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
 glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
 glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
 glEnable(GL_LIGHT0); // источник света LIGHT0

 glTranslatef(0,0,5);
 glScalef(0.2, 0.2, 0.2);
 glColor3f(1,1,1);
 Draw();
}


void Init_Material()
{
glEnable(GL_COLOR_MATERIAL); //разрешения использования
//материала
 glShadeModel(GL_SMOOTH); // сглаживает границы
 GLfloat material_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
 GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
 GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 32.0f };
 GLfloat material_shininess[] = { 50.0f }; //блеск материала
 glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
 glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
 glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
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
                          1500,
                          1500,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glFrustum(-1, 1, -1, 1, 2, 80);
    glEnable(GL_DEPTH_TEST);

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

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



            glPushMatrix();
            MoveCamera();
            ShowWorld();
            Draw_Cube();

            //glPushMatrix();
            glRotatef(theta, 1,1,0);
            Init_Light();
            Init_Material();



            glPopMatrix();
            //glPopMatrix();

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

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

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
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

