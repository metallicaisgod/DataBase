#include "stdafx.h"
#include <math.h>
#include "glmodel.h"

#define NUM_OF_FACES        10
#define NUM_OF_TRIANGLES    (4 * NUM_OF_FACES - 4)
#define RADIUS              0.05
#define HEIGHT              0.5

const static float pi=3.141593, k=pi/180;

//int ng = 12;

GLModel::GLModel(QWidget* parent) : QGLWidget(parent)
{
   xRot=-90; yRot=0; zRot=0; zTra=0; nSca=1;
   m_curImpl = NULL;
   m_curAbut = NULL;
   m_vertexArray = NULL;
   m_normalArray = NULL;
   m_colorArray = NULL;
   m_indexArray = NULL;
}

void GLModel::initializeGL()
{
    qglClearColor(Qt::blue);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    lightPos[0] = 3;
    lightPos[1] = 3;
    lightPos[2] = 3;
    lightPos[3] = 0;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_COLOR_MATERIAL);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
}

void GLModel::setCurrentImplant(db::DbImplant *impl)
{
    m_curImpl = impl;

    if(impl != NULL)
    {
        getVertexArray();
        getColorArray();
        getIndexArray();
    }

    if(isVisible())
        updateGL();
}

void GLModel::setCurrentAbutment(db::DbAbutment *abut)
{
    m_curAbut = abut;

//    if(abut != NULL)
//    {
//        getVertexArray();
//        getColorArray();
//        getIndexArray();
//    }

//    if(isVisible())
//        updateGL();
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

    //glTranslatef(0.0f, 0.0f, 0.0f);
    if(m_curImpl || m_curAbut)
    {
        glPushMatrix();
            glScalef(nSca, nSca, nSca);
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glPopMatrix();

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
    }

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
    if(m_vertexArray != NULL)
    {
        free(m_vertexArray);
        m_vertexArray = NULL;
    }
    if(m_colorArray != NULL)
    {
        free(m_colorArray);
        m_colorArray = NULL;
    }
    if(m_normalArray != NULL)
    {
        free(m_normalArray);
        m_normalArray = NULL;
    }
    if(m_indexArray != NULL)
    {
        free(m_indexArray);
        m_indexArray = NULL;
    }

    m_faceCount = NUM_OF_FACES;

    m_vertexCount = m_faceCount * 2;

    m_trianglesCount = 4 * m_faceCount - 4;
    if(/*m_curImpl &&*/ m_curImpl->L1 > 0)
    {
         m_trianglesCount += 2 * m_faceCount;
         m_vertexCount += m_faceCount;
    }

    m_vertexArray =( GLFloatTriplet*)malloc(m_vertexCount * sizeof(GLFloatTriplet));
    m_colorArray = ( GLFloatTriplet*)malloc(m_vertexCount * sizeof(GLFloatTriplet));
    m_normalArray = ( GLFloatTriplet*)malloc(m_trianglesCount * sizeof(GLFloatTriplet));
    m_indexArray = (GLUShortTriplet *)malloc(m_trianglesCount * sizeof(GLUShortTriplet));

    float scale = 0.05;

    float R1 = m_curImpl->D1 / 2 * scale;
    float R2 = m_curImpl->D2 / 2 * scale;
    float L1 = m_curImpl->L1 * scale;
    float L2 = m_curImpl->L2 * scale;

    for(int  i = 0; i < m_faceCount; i++)
    {
        float seta = i*360.0/m_faceCount;
        float v1x = sin(M_PI * seta / 180.0)*R1;
        float v1y = cos(M_PI * seta / 180.0)*R1;

        float v2x = sin(M_PI * seta / 180.0)*R2;
        float v2y = cos(M_PI * seta / 180.0)*R2;

        m_vertexArray[2 * i].x     = v2x;
        m_vertexArray[2 * i].y     = v2y;
        m_vertexArray[2 * i].z     = L2;

        m_vertexArray[2 * i + 1].x = v1x;
        m_vertexArray[2 * i + 1].y = v1y;
        m_vertexArray[2 * i + 1].z = 0.0;

        if(L1 > 0)
        {
            m_vertexArray[2 * m_faceCount + i].x = v1x;
            m_vertexArray[2 * m_faceCount + i].y = v1y;
            m_vertexArray[2 * m_faceCount + i].z = -L1;
        }

    }
    getNormalArray();
}

void CalcNormals(GLFloatTriplet  v1, GLFloatTriplet  v2, GLFloatTriplet  v3, GLFloatTriplet * norm)
{
    GLfloat Qx, Qy, Qz, Px, Py, Pz, x, y, z;

    Qx = v2.x - v1.x;
    Qy = v2.y - v1.y;
     Qz = v2.z - v1.z;
     Px = v3.x - v1.x;
     Py = v3.y - v1.y;
     Pz = v3.z - v1.z;

     x = Py*Qz - Pz*Qy;
     y = Pz*Qx - Px*Qz;
     z = Px*Qy - Py*Qx;

     GLfloat vec_length = sqrt(x*x + y*y + z*z);
     GLfloat inv_length = 1.0f;
    if(vec_length != 0)
        inv_length = 1.0f/ vec_length;
     norm->x = x * inv_length;
     norm->y = y * inv_length;
     norm->z = z * inv_length;
}



void GLModel::getNormalArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        CalcNormals(m_vertexArray[2 * i], m_vertexArray[2 * i + 1], m_vertexArray[2 * i + 3], &m_normalArray[2 * i]);
        CalcNormals(m_vertexArray[2 * i], m_vertexArray[2 * i + 3], m_vertexArray[2 * i + 2], &m_normalArray[2 * i + 1]);
        if(m_curImpl->L1 > 0)
        {
            CalcNormals(m_vertexArray[2 * i + 1], m_vertexArray[2 * m_faceCount + i], m_vertexArray[2 * m_faceCount + i + 1], &m_normalArray[2 * m_faceCount + 2 * i]);
            CalcNormals(m_vertexArray[2 * i + 1], m_vertexArray[2 * m_faceCount + i + 1], m_vertexArray[2 * i + 3], &m_normalArray[2 * m_faceCount + 2 * i + 1]);
        }
    }
    CalcNormals(m_vertexArray[2 * m_faceCount - 2], m_vertexArray[2 * m_faceCount - 1], m_vertexArray[1], &m_normalArray[2 * m_faceCount - 2]);
    CalcNormals(m_vertexArray[2 * m_faceCount - 2], m_vertexArray[1], m_vertexArray[0], &m_normalArray[2 * m_faceCount - 1]);

    if(m_curImpl->L1 > 0)
    {
        CalcNormals(m_vertexArray[2 * m_faceCount - 1], m_vertexArray[3 * m_faceCount - 1], m_vertexArray[1], &m_normalArray[4 * m_faceCount - 2]);
        CalcNormals(m_vertexArray[2 * m_faceCount - 1], m_vertexArray[2 * m_faceCount], m_vertexArray[1], &m_normalArray[4 * m_faceCount - 1]);
    }

    int st = 2 * m_faceCount;
    if(m_curImpl->L1 > 0)
    {
        st *= 2;
    }
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        CalcNormals(m_vertexArray[0], m_vertexArray[2 * i + 2], m_vertexArray[2 * i + 4], &m_normalArray[st + i]);
    }
    st += m_faceCount - 2;
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        if(m_curImpl->L1 > 0)
            CalcNormals(m_vertexArray[2 * m_faceCount], m_vertexArray[2 * m_faceCount + i + 1], m_vertexArray[2 * m_faceCount + i + 2], &m_normalArray[st + i]);
        else
            CalcNormals(m_vertexArray[1], m_vertexArray[2 * i + 3], m_vertexArray[2 * i + 5],&m_normalArray[st + i]);
    }
}

void GLModel::getColorArray()
{
   for (int i=0; i < m_vertexCount; i++)
   {
      m_colorArray[i].x=1.0f;
      m_colorArray[i].y=0.0f;
      m_colorArray[i].z=0.0f;
   }
}

void GLModel::getIndexArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        m_indexArray[2 * i].x = 2 * i;
        m_indexArray[2 * i].y = 2 * i + 1;
        m_indexArray[2 * i].z = 2 * i + 3;

        m_indexArray[2 * i + 1].x = 2 * i;
        m_indexArray[2 * i + 1].y = 2 * i + 3;
        m_indexArray[2 * i + 1].z = 2 * i + 2;

        if(m_curImpl->L1 > 0)
        {
            m_indexArray[2 * m_faceCount + 2 * i].x = 2 * i + 1;
            m_indexArray[2 * m_faceCount + 2 * i].y = 2 * m_faceCount + i;
            m_indexArray[2 * m_faceCount + 2 * i].z = 2 * m_faceCount + i + 1;

            m_indexArray[2 * m_faceCount + 2 * i + 1].x = 2 * i + 1;
            m_indexArray[2 * m_faceCount + 2 * i + 1].y = 2 * m_faceCount + i + 1;
            m_indexArray[2 * m_faceCount + 2 * i + 1].z = 2 * i + 3;
        }

    }

   m_indexArray[m_faceCount * 2 - 2].x = m_faceCount * 2 - 2;
   m_indexArray[m_faceCount * 2 - 2].y = m_faceCount * 2 - 1;
   m_indexArray[m_faceCount * 2 - 2].z = 1;

   m_indexArray[m_faceCount * 2 - 1].x = m_faceCount * 2 - 2;
   m_indexArray[m_faceCount * 2 - 1].y = 1;
   m_indexArray[m_faceCount * 2 - 1].z = 0;

   if(m_curImpl->L1 > 0)
   {
       m_indexArray[m_faceCount * 4 - 2].x = 2 * m_faceCount - 1;
       m_indexArray[m_faceCount * 4 - 2].y = 3 * m_faceCount - 1;
       m_indexArray[m_faceCount * 4 - 2].z = 2 * m_faceCount;

       m_indexArray[m_faceCount * 4 - 1].x = 2 * m_faceCount - 1;
       m_indexArray[m_faceCount * 4 - 1].y = 2 * m_faceCount;
       m_indexArray[m_faceCount * 4 - 1].z = 1;
   }


   int st = 2 * m_faceCount;
   if(m_curImpl->L1 > 0)
   {
       st *= 2;
   }
   //верхн€€ грань
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       m_indexArray[st + i].x = 0;
       m_indexArray[st + i].y = i * 2 + 2;
       m_indexArray[st + i].z = i * 2 + 4;
   }
   st += m_faceCount - 2;
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       if(m_curImpl->L1 > 0)
       {
           m_indexArray[st + i].x = 2 * m_faceCount;
           m_indexArray[st + i].y = 2 * m_faceCount + i + 1;
           m_indexArray[st + i].z = 2 * m_faceCount + i + 2;
       }
       else
       {
           m_indexArray[st + i].x = 1;
           m_indexArray[st + i].y = i * 2 + 3;
           m_indexArray[st + i].z = i * 2 + 5;
       }
   }

}

void GLModel::drawFigure()
{
    glNormalPointer(GL_FLOAT, 0, m_normalArray);
    glVertexPointer(3, GL_FLOAT, 0, m_vertexArray);
    glColorPointer(3, GL_FLOAT, 0, m_colorArray);
    glDrawElements(GL_TRIANGLES, (m_trianglesCount) * 3, GL_UNSIGNED_SHORT, m_indexArray);
}
