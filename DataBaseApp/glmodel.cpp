#include "stdafx.h"
#include <math.h>
#include "glmodel.h"

#define NUM_OF_FACES        50
#define NUM_OF_TRIANGLES    (4 * NUM_OF_FACES - 4)
#define RADIUS              0.25

const static float pi=3.141593, k=pi/180;

//int ng = 12;

GLfloat VertexArray[NUM_OF_FACES * 2][3];
GLfloat NormalArray[NUM_OF_TRIANGLES][3];
GLfloat ColorArray[NUM_OF_FACES * 2][3];
GLubyte IndexArray[NUM_OF_TRIANGLES][3];

GLModel::GLModel(QWidget* parent) : QGLWidget(parent)
{
   xRot=-90; yRot=0; zRot=0; zTra=0; nSca=1;
}

void GLModel::initializeGL()
{
   qglClearColor(Qt::blue);

   //glDisable(GL_DEPTH_TEST);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);

//   glClearDepth(1.0f);
//   GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; // Значения фонового света ( НОВОЕ )
//   GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f }; // Значения диффузного света ( НОВОЕ )
//   GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };     // Позиция света ( НОВОЕ )
//   glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);    // Установка Фонового Света
//   glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);    // Установка Диффузного Света
//   glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);   // Позиция света
//   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
//   GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
//   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//   float pos[4] = {3,3,3,1};
//   float dir[3] = {-1,-1,-1};

//   GLfloat mat_specular[] = {1,1,1,1 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
//    glLightfv(GL_LIGHT0, GL_POSITION, pos);
//    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//    glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

//   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT3);
//   float pos[4] = {3,3,3,0};
//   float color[4] = {1,1,1,1};
//   float sp[4] = {1,1,1,1};
//   float mat_specular[] = {1,1,1,1};
   glEnable(GL_COLOR_MATERIAL);
//   float ambient[4] = {0.5, 0.5, 0.5, 1};
//   glEnable(GL_LIGHTING);
//   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
//   glEnable(GL_LIGHT3);
//       glEnable(GL_LIGHT5);
//       glEnable(GL_LIGHT6);
//   glLightfv(GL_LIGHT3, GL_SPECULAR, sp);
//   glLightfv(GL_LIGHT5, GL_SPECULAR, sp);
//   glLightfv(GL_LIGHT6, GL_SPECULAR, sp);
//   color[1]=color[2]=0;
//   glLightfv(GL_LIGHT3, GL_DIFFUSE, color);
//   color[0]=0;
//       color[1]=1;
//   glLightfv(GL_LIGHT5, GL_DIFFUSE, color);
//   color[1]=0;
//       color[2]=1;
//   glLightfv(GL_LIGHT6, GL_DIFFUSE, color);
//   glLightfv(GL_LIGHT3, GL_POSITION, pos);
//       pos[0] = -3;
//   glLightfv(GL_LIGHT5, GL_POSITION, pos);
//       pos[0]=0;pos[1]=-3;
//   glLightfv(GL_LIGHT6, GL_POSITION, pos);
//   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//   glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
  // glEnable(GL_LIGHT0);
   //glEnable(GL_CULL_FACE);

   //getNormalArray();
   getVertexArray();
   getColorArray();
   getIndexArray();

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
}

void GLModel::resizeGL(int nWidth, int nHeight)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   GLfloat ratio=(GLfloat)nHeight/(GLfloat)nWidth;

   if (nWidth>=nHeight)
      glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 1.0);
   else
      glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 1.0);

   glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

void GLModel::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
//   GLfloat position[] = {0.0f,0.0f,1.5f,1.0f};
//  // GLfloat back_color[] = {0,0,1,1};
//  // glMaterialfv(GL_FRONT, GL_DIFFUSE, front_color);
//  // glMaterialfv(GL_BACK, GL_DIFFUSE, front_color);

//   glPushMatrix; // запомнили мировую систему координат
//   //glRotatef (spin, 1.0, О О, 0.0); // поворот системы координат
//   glLightfv (GL_LIGHT0, GL_POSITION, position); // задаем новую позицию источника света
//   glTranslated (0.0, 0.0, 1.5); // перемещаемся в точку, где
//   // располагается источник света
//   glDisable (GL_LIGHTING); // отключаем источник света
//   //glutWireCube (0.1); // визуализируем источник света
//   glEnable (GL_LIGHTING); // включаем источник света
//   glPopMatrix; // возвращаемся в мировую систему координат

   glTranslatef(0.0f, 0.0f, zTra);
   //draw figure
   glPushMatrix();
        glScalef(nSca, nSca, nSca);
        glRotatef(xRot, 1.0f, 0.0f, 0.0f);
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
        glRotatef(zRot, 0.0f, 0.0f, 1.0f);
        drawFigure();
   glPopMatrix();
   //draw axis
   glPushMatrix();
        glTranslatef(0.8f, -0.8f, -0.8f);
        glRotatef(xRot, 1.0f, 0.0f, 0.0f);
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
        glRotatef(zRot, 0.0f, 0.0f, 1.0f);
        drawAxis();
   glPopMatrix();
   glPushMatrix();
   glPopMatrix();


}

void GLModel::mousePressEvent(QMouseEvent* pe)
{
   ptrMousePosition = pe->pos();
}

void GLModel::mouseReleaseEvent(QMouseEvent* pe)
{

}

void GLModel::mouseMoveEvent(QMouseEvent* pe)
{
   xRot += 180/nSca*(GLfloat)(pe->y()-ptrMousePosition.y())/height();
   zRot += 180/nSca*(GLfloat)(pe->x()-ptrMousePosition.x())/width();

   ptrMousePosition = pe->pos();

   updateGL();
}

void GLModel::wheelEvent(QWheelEvent* pe)
{
   if ((pe->delta())>0) scale_plus(); else if ((pe->delta())<0) scale_minus();

   updateGL();
}

void GLModel::keyPressEvent(QKeyEvent* pe)
{
   switch (pe->key())
   {
      case Qt::Key_Plus:
         scale_plus();
      break;

      case Qt::Key_Equal:
         scale_plus();
      break;

      case Qt::Key_Minus:
         scale_minus();
      break;

      case Qt::Key_Up:
         rotate_up();
      break;

      case Qt::Key_Down:
         rotate_down();
      break;

      case Qt::Key_Left:
        rotate_left();
      break;

      case Qt::Key_Right:
         rotate_right();
      break;

      case Qt::Key_Z:
         translate_down();
      break;

      case Qt::Key_X:
         translate_up();
      break;

      case Qt::Key_Space:
         defaultScene();
      break;

      case Qt::Key_Escape:
         this->close();
      break;
   }

   updateGL();
}

void GLModel::scale_plus()
{
   nSca = nSca*1.1;
}

void GLModel::scale_minus()
{
   nSca = nSca/1.1;
}

void GLModel::rotate_up()
{
   xRot += 1.0;
}

void GLModel::rotate_down()
{
   xRot -= 1.0;
}

void GLModel::rotate_left()
{
   zRot += 1.0;
}

void GLModel::rotate_right()
{
   zRot -= 1.0;
}

void GLModel::translate_down()
{
    zTra-= 0.05;
}

void GLModel::translate_up()
{
   zTra += 0.05;
}

void GLModel::defaultScene()
{
   xRot=-90; yRot=0; zRot=0; zTra=0; nSca=1;
}

void GLModel::drawAxis()
{
   glLineWidth(1.0f);

   glColor4f(1.00f, 0.00f, 0.00f, 1.0f);
   glBegin(GL_LINES);
      glVertex3f( 0.0f,  0.0f,  0.0f);
      glVertex3f(0.15f,  0.0f,  0.0f);
   glEnd();
   renderText(0.17f,  0.0f,  0.0f, "X");

    glColor4f(0.00f, 1.00f, 0.00f, 1.0f);
   glBegin(GL_LINES);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.0f, 0.15f,  0.0f);
   glEnd();
   renderText( 0.0f,  0.17f,  0.0f, "Y");

   glColor4f(1.00f, 0.00f, 1.00f, 1.0f);
   glBegin(GL_LINES);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.0f, 0.0f,  0.15f);
   glEnd();
   renderText( 0.0f,  0.0f,  0.17f, "Z");
}

void GLModel::getVertexArray()
{
    int ng = NUM_OF_FACES;
   // float vert[12+1][2];
    /// при инициализации
    float rad = RADIUS;

    for(int i = 0; i < ng; i++)
    {
        float seta = i*360.0/ng;
        float vx = sin(M_PI * seta / 180.0)*rad;
        float vy = cos(M_PI * seta / 180.0)*rad;

        VertexArray[2 * i][0] = vx;
        VertexArray[2 * i][1] = vy;
        VertexArray[2 * i][2] = 0.5;

        VertexArray[2 * i + 1][0] = vx;
        VertexArray[2 * i + 1][1] = vy;
        VertexArray[2 * i + 1][2] = -0.5;
    }
    getNormalArray();
}

void CalcNormals(GLfloat * v1, GLfloat * v2, GLfloat * v3, GLfloat * norm)
{
    GLfloat Qx, Qy, Qz, Px, Py, Pz, x, y, z;

    Qx = v2[0] - v1[0];
    Qy = v2[1] - v1[1];
     Qz = v2[2] - v1[2];
     Px = v3[0] - v1[0];
     Py = v3[1] - v1[1];
     Pz = v3[2] - v1[2];

     x = Py*Qz - Pz*Qy;
     y = Pz*Qx - Px*Qz;
     z = Px*Qy - Py*Qx;

     GLfloat inv_length = 1.0f / sqrt(x*x + y*y + z*z);
     norm[0] = x * inv_length;
     norm[1] = y * inv_length;
     norm[2] = z * inv_length;
}



void GLModel::getNormalArray()
{
    for(int i = 0; i < NUM_OF_FACES - 1; i++)
    {
        CalcNormals(VertexArray[2 * i], VertexArray[2 * i + 1], VertexArray[2 * i + 3], NormalArray[2 * i]);
        CalcNormals(VertexArray[2 * i], VertexArray[2 * i + 3], VertexArray[2 * i + 3], NormalArray[2 * i + 1]);
    }
    CalcNormals(VertexArray[NUM_OF_FACES * 2 - 2], VertexArray[NUM_OF_FACES * 2 - 1], VertexArray[1], NormalArray[NUM_OF_FACES * 2 - 2]);
    CalcNormals(VertexArray[NUM_OF_FACES * 2 - 2], VertexArray[0], VertexArray[1], NormalArray[NUM_OF_FACES * 2 - 1]);
    int st = NUM_OF_FACES * 2;
    for(int i = 0; i < NUM_OF_FACES - 2; i++)
    {
        CalcNormals(VertexArray[0], VertexArray[2 * i + 2], VertexArray[2 * i + 4], NormalArray[st + i]);
    }
    st = NUM_OF_FACES * 2 + NUM_OF_FACES - 2;
    for(int i = 0; i < NUM_OF_FACES - 2; i++)
    {
        CalcNormals(VertexArray[1], VertexArray[2 * i + 3], VertexArray[2 * i + 5], NormalArray[st + i]);
    }
}

void GLModel::getColorArray()
{
   for (int i=0; i < NUM_OF_FACES * 2; i++)
   {
      ColorArray[i][0]=1.0f;
      ColorArray[i][1]=0.0f;
      ColorArray[i][2]=0.0f;
   }
}

void GLModel::getIndexArray()
{
    for(int i = 0; i < NUM_OF_FACES - 1; i++)
    {
        IndexArray[2 * i][0] = 2 * i;
        IndexArray[2 * i][1] = 2 * i + 1;
        IndexArray[2 * i][2] = 2 * i + 3;

        IndexArray[2 * i + 1][0] = 2 * i;
        IndexArray[2 * i + 1][1] = 2 * i + 2;
        IndexArray[2 * i + 1][2] = 2 * i + 3;
    }

   IndexArray[NUM_OF_FACES * 2 - 2][0] = NUM_OF_FACES * 2 - 2;
   IndexArray[NUM_OF_FACES * 2 - 2][1] = NUM_OF_FACES * 2 - 1;
   IndexArray[NUM_OF_FACES * 2 - 2][2] = 1;

   IndexArray[NUM_OF_FACES * 2 - 1][0] = NUM_OF_FACES * 2 - 2;
   IndexArray[NUM_OF_FACES * 2 - 1][1] = 0;
   IndexArray[NUM_OF_FACES * 2 - 1][2] = 1;

   //верхняя грань
   int st = NUM_OF_FACES * 2;
   for(int i = 0; i < NUM_OF_FACES - 2; i++)
   {
       IndexArray[st + i][0] = 0;
       IndexArray[st + i][1] = i * 2 + 2;
       IndexArray[st + i][2] = i * 2 + 4;
   }
   st = NUM_OF_FACES * 2 + NUM_OF_FACES - 2;
   for(int i = 0; i < NUM_OF_FACES - 2; i++)
   {
        IndexArray[st + i][0] = 1;
        IndexArray[st + i][1] = i * 2 + 3;
        IndexArray[st + i][2] = i * 2 + 5;
   }
}

void GLModel::drawFigure()
{
    glNormalPointer(GL_FLOAT, 0, NormalArray);
    glVertexPointer(3, GL_FLOAT, 0, VertexArray);
   glColorPointer(3, GL_FLOAT, 0, ColorArray);
   glDrawElements(GL_TRIANGLES, (NUM_OF_TRIANGLES) * 3, GL_UNSIGNED_BYTE, IndexArray);
}
