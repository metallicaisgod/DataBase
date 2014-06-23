#include "stdafx.h"
#include <math.h>
#include "glmodel.h"

#define NUM_OF_FACES        10
#define NUM_OF_TRIANGLES    (4 * NUM_OF_FACES - 4)
#define RADIUS              0.05
#define HEIGHT              0.5

const static float pi=3.141593, k=pi/180;
const float scale = 0.05;

//int ng = 12;

GLModel::GLModel(QWidget* parent) : QGLWidget(parent)
{
   xRot=-90; yRot=0; zRot=0; zTra=0; nSca=1;
   m_curImpl = NULL;
   m_curAbut = NULL;
   m_implVertexArray = NULL;
   m_implNormalArray = NULL;
   m_implColorArray = NULL;
   m_implIndexArray = NULL;
   m_abutVertexArray = NULL;
   m_abutNormalArray = NULL;
   m_abutColorArray = NULL;
   m_abutIndexArray = NULL;
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
        getImplVertexArray();
        getImplColorArray();
        getImplIndexArray();
    }

    if(isVisible())
        updateGL();
}

void GLModel::setCurrentAbutment(db::DbAbutment *abut)
{
    m_curAbut = abut;

    if(abut != NULL)
    {
        getAbutVertexArray();
        getAbutColorArray();
        getAbutIndexArray();
    }

    if(isVisible())
        updateGL();
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

        glPushMatrix();
            glScalef(nSca, nSca, nSca);
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glPopMatrix();
    if(m_curImpl || m_curAbut)
    {
       //draw figure
        glPushMatrix();
            glScalef(nSca, nSca, nSca);
            //glRotatef(xRot, 1.0f, 0.0f, 0.0f);
            glRotatef(xRot, 0.0f, 0.0f, 1.0f);
            glRotatef(xRot, 0.0f, 1.0f, 0.0f);
            glRotatef(yRot, 1.0f, 0.0f, 0.0f);
            glRotatef(yRot, 0.0f, 0.0f, 1.0f);
            glRotatef(zRot, 0.0f, 1.0f, 0.0f);
            glRotatef(zRot, 1.0f, 0.0f, 0.0f);
            float length_impl = 0.0;
            float length_abut = 0.0;

            if(m_curImpl)
            {
                length_impl =  ((m_curImpl->L1 + m_curImpl->L2)) * scale ;
            }
            if(m_curAbut)
            {
                length_abut = (m_curAbut->L1 / cos( M_PI * m_curAbut->Alpha / 180.0)) * scale;
            }


            if(m_curAbut)
            {
                glPushMatrix();
                    glTranslatef(0.0f, 0.0f, (length_impl + length_abut) / 2 - length_abut);
                    glRotatef(m_curAbut->Alpha, 1.0f, 0.0f, 0.0f);
                    glRotatef(m_curAbut->Alpha, 0.0f, 1.0f, 0.0f);
                    drawAbutment();
                glPopMatrix();
            }
            if(m_curImpl)
            {
                glPushMatrix();
                    glTranslatef(0.0f, 0.0f, -((length_impl + length_abut) / 2 - m_curImpl->L1 * scale));
                    drawImplant();
                glPopMatrix();
            }

        glPopMatrix();
    }
       //draw axis
        glPushMatrix();
            glTranslatef(0.8f, -0.8f, -0.8f);
            glRotatef(xRot, 0.0f, 0.0f, 1.0f);
            glRotatef(xRot, 0.0f, 1.0f, 0.0f);
            glRotatef(yRot, 1.0f, 0.0f, 0.0f);
            glRotatef(yRot, 0.0f, 0.0f, 1.0f);
            glRotatef(zRot, 0.0f, 1.0f, 0.0f);
            glRotatef(zRot, 1.0f, 0.0f, 0.0f);
            drawAxis();
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

   glDisable(GL_DEPTH_TEST);
   renderText(0.17f,  0.0f,  0.0f, "X");
    glEnable(GL_DEPTH_TEST);

    glColor4f(0.00f, 1.00f, 0.00f, 1.0f);
   glBegin(GL_LINES);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.0f, 0.15f,  0.0f);
   glEnd();
   glDisable(GL_DEPTH_TEST);
   renderText( 0.0f,  0.17f,  0.0f, "Y");
    glEnable(GL_DEPTH_TEST);

   glColor4f(1.00f, 0.00f, 1.00f, 1.0f);
   glBegin(GL_LINES);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.0f, 0.0f,  0.15f);
   glEnd();
   glDisable(GL_DEPTH_TEST);
   renderText( 0.0f,  0.0f,  0.17f, "Z");
   glEnable(GL_DEPTH_TEST);
}

void GLModel::getImplVertexArray()
{
    if(m_implVertexArray != NULL)
    {
        free(m_implVertexArray);
        m_implVertexArray = NULL;
    }
    if(m_implColorArray != NULL)
    {
        free(m_implColorArray);
        m_implColorArray = NULL;
    }
    if(m_implNormalArray != NULL)
    {
        free(m_implNormalArray);
        m_implNormalArray = NULL;
    }
    if(m_implIndexArray != NULL)
    {
        free(m_implIndexArray);
        m_implIndexArray = NULL;
    }

    m_faceCount = NUM_OF_FACES;

    m_implVertexCount = m_faceCount * 2;

    m_implTrianglesCount = 4 * m_faceCount - 4;
    if(/*m_curImpl &&*/ m_curImpl->L1 > 0)
    {
         m_implTrianglesCount += 2 * m_faceCount;
         m_implVertexCount += m_faceCount;
    }

    m_implVertexArray =( GLFloatTriplet*)malloc(m_implVertexCount * sizeof(GLFloatTriplet));
    m_implColorArray = ( GLFloatTriplet*)malloc(m_implVertexCount * sizeof(GLFloatTriplet));
    m_implNormalArray = ( GLFloatTriplet*)malloc(m_implTrianglesCount * sizeof(GLFloatTriplet));
    m_implIndexArray = (GLUShortTriplet *)malloc(m_implTrianglesCount * sizeof(GLUShortTriplet));

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

        m_implVertexArray[2 * i].x     = v2x;
        m_implVertexArray[2 * i].y     = v2y;
        m_implVertexArray[2 * i].z     = L2;

        m_implVertexArray[2 * i + 1].x = v1x;
        m_implVertexArray[2 * i + 1].y = v1y;
        m_implVertexArray[2 * i + 1].z = 0.0;

        if(L1 > 0)
        {
            m_implVertexArray[2 * m_faceCount + i].x = v1x;
            m_implVertexArray[2 * m_faceCount + i].y = v1y;
            m_implVertexArray[2 * m_faceCount + i].z = -L1;
        }

    }
    getImplNormalArray();
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



void GLModel::getImplNormalArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        CalcNormals(m_implVertexArray[2 * i], m_implVertexArray[2 * i + 1], m_implVertexArray[2 * i + 3], &m_implNormalArray[2 * i]);
        CalcNormals(m_implVertexArray[2 * i], m_implVertexArray[2 * i + 3], m_implVertexArray[2 * i + 2], &m_implNormalArray[2 * i + 1]);
        if(m_curImpl->L1 > 0)
        {
            CalcNormals(m_implVertexArray[2 * i + 1], m_implVertexArray[2 * m_faceCount + i], m_implVertexArray[2 * m_faceCount + i + 1], &m_implNormalArray[2 * m_faceCount + 2 * i]);
            CalcNormals(m_implVertexArray[2 * i + 1], m_implVertexArray[2 * m_faceCount + i + 1], m_implVertexArray[2 * i + 3], &m_implNormalArray[2 * m_faceCount + 2 * i + 1]);
        }
    }
    CalcNormals(m_implVertexArray[2 * m_faceCount - 2], m_implVertexArray[2 * m_faceCount - 1], m_implVertexArray[1], &m_implNormalArray[2 * m_faceCount - 2]);
    CalcNormals(m_implVertexArray[2 * m_faceCount - 2], m_implVertexArray[1], m_implVertexArray[0], &m_implNormalArray[2 * m_faceCount - 1]);

    if(m_curImpl->L1 > 0)
    {
        CalcNormals(m_implVertexArray[2 * m_faceCount - 1], m_implVertexArray[3 * m_faceCount - 1], m_implVertexArray[1], &m_implNormalArray[4 * m_faceCount - 2]);
        CalcNormals(m_implVertexArray[2 * m_faceCount - 1], m_implVertexArray[2 * m_faceCount], m_implVertexArray[1], &m_implNormalArray[4 * m_faceCount - 1]);
    }

    int st = 2 * m_faceCount;
    if(m_curImpl->L1 > 0)
    {
        st *= 2;
    }
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        CalcNormals(m_implVertexArray[0], m_implVertexArray[2 * i + 2], m_implVertexArray[2 * i + 4], &m_implNormalArray[st + i]);
    }
    st += m_faceCount - 2;
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        if(m_curImpl->L1 > 0)
            CalcNormals(m_implVertexArray[2 * m_faceCount], m_implVertexArray[2 * m_faceCount + i + 1], m_implVertexArray[2 * m_faceCount + i + 2], &m_implNormalArray[st + i]);
        else
            CalcNormals(m_implVertexArray[1], m_implVertexArray[2 * i + 3], m_implVertexArray[2 * i + 5],&m_implNormalArray[st + i]);
    }
}

void GLModel::getImplColorArray()
{
   for (int i=0; i < m_implVertexCount; i++)
   {
      m_implColorArray[i].x=1.0f;
      m_implColorArray[i].y=0.0f;
      m_implColorArray[i].z=0.0f;
   }
}

void GLModel::getImplIndexArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        m_implIndexArray[2 * i].x = 2 * i;
        m_implIndexArray[2 * i].y = 2 * i + 1;
        m_implIndexArray[2 * i].z = 2 * i + 3;

        m_implIndexArray[2 * i + 1].x = 2 * i;
        m_implIndexArray[2 * i + 1].y = 2 * i + 3;
        m_implIndexArray[2 * i + 1].z = 2 * i + 2;

        if(m_curImpl->L1 > 0)
        {
            m_implIndexArray[2 * m_faceCount + 2 * i].x = 2 * i + 1;
            m_implIndexArray[2 * m_faceCount + 2 * i].y = 2 * m_faceCount + i;
            m_implIndexArray[2 * m_faceCount + 2 * i].z = 2 * m_faceCount + i + 1;

            m_implIndexArray[2 * m_faceCount + 2 * i + 1].x = 2 * i + 1;
            m_implIndexArray[2 * m_faceCount + 2 * i + 1].y = 2 * m_faceCount + i + 1;
            m_implIndexArray[2 * m_faceCount + 2 * i + 1].z = 2 * i + 3;
        }

    }

   m_implIndexArray[m_faceCount * 2 - 2].x = m_faceCount * 2 - 2;
   m_implIndexArray[m_faceCount * 2 - 2].y = m_faceCount * 2 - 1;
   m_implIndexArray[m_faceCount * 2 - 2].z = 1;

   m_implIndexArray[m_faceCount * 2 - 1].x = m_faceCount * 2 - 2;
   m_implIndexArray[m_faceCount * 2 - 1].y = 1;
   m_implIndexArray[m_faceCount * 2 - 1].z = 0;

   if(m_curImpl->L1 > 0)
   {
       m_implIndexArray[m_faceCount * 4 - 2].x = 2 * m_faceCount - 1;
       m_implIndexArray[m_faceCount * 4 - 2].y = 3 * m_faceCount - 1;
       m_implIndexArray[m_faceCount * 4 - 2].z = 2 * m_faceCount;

       m_implIndexArray[m_faceCount * 4 - 1].x = 2 * m_faceCount - 1;
       m_implIndexArray[m_faceCount * 4 - 1].y = 2 * m_faceCount;
       m_implIndexArray[m_faceCount * 4 - 1].z = 1;
   }


   int st = 2 * m_faceCount;
   if(m_curImpl->L1 > 0)
   {
       st *= 2;
   }
   //верхн€€ грань
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       m_implIndexArray[st + i].x = 0;
       m_implIndexArray[st + i].y = i * 2 + 2;
       m_implIndexArray[st + i].z = i * 2 + 4;
   }
   st += m_faceCount - 2;
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       if(m_curImpl->L1 > 0)
       {
           m_implIndexArray[st + i].x = 2 * m_faceCount;
           m_implIndexArray[st + i].y = 2 * m_faceCount + i + 1;
           m_implIndexArray[st + i].z = 2 * m_faceCount + i + 2;
       }
       else
       {
           m_implIndexArray[st + i].x = 1;
           m_implIndexArray[st + i].y = i * 2 + 3;
           m_implIndexArray[st + i].z = i * 2 + 5;
       }
   }

}

void GLModel::drawImplant()
{
    glNormalPointer(GL_FLOAT, 0, m_implNormalArray);
    glVertexPointer(3, GL_FLOAT, 0, m_implVertexArray);
    glColorPointer(3, GL_FLOAT, 0, m_implColorArray);
    glDrawElements(GL_TRIANGLES, (m_implTrianglesCount) * 3, GL_UNSIGNED_SHORT, m_implIndexArray);
}

void GLModel::getAbutVertexArray()
{
    if(m_abutVertexArray != NULL)
    {
        free(m_abutVertexArray);
        m_abutVertexArray = NULL;
    }
    if(m_abutColorArray != NULL)
    {
        free(m_abutColorArray);
        m_abutColorArray = NULL;
    }
    if(m_abutNormalArray != NULL)
    {
        free(m_abutNormalArray);
        m_abutNormalArray = NULL;
    }
    if(m_abutIndexArray != NULL)
    {
        free(m_abutIndexArray);
        m_abutIndexArray = NULL;
    }

    m_faceCount = NUM_OF_FACES;

    m_abutVertexCount = m_faceCount * 2;

    m_abutTrianglesCount = 4 * m_faceCount - 4;

    m_abutVertexArray =( GLFloatTriplet*)malloc(m_abutVertexCount * sizeof(GLFloatTriplet));
    m_abutColorArray = ( GLFloatTriplet*)malloc(m_abutVertexCount * sizeof(GLFloatTriplet));
    m_abutNormalArray = ( GLFloatTriplet*)malloc(m_abutTrianglesCount * sizeof(GLFloatTriplet));
    m_abutIndexArray = (GLUShortTriplet *)malloc(m_abutTrianglesCount * sizeof(GLUShortTriplet));

    float R1 = m_curAbut->D1 / 2 * scale;
    float L1 = m_curAbut->L1 * scale;

    for(int  i = 0; i < m_faceCount; i++)
    {
        float seta = i*360.0/m_faceCount;
        float v1x = sin(M_PI * seta / 180.0)*R1;
        float v1y = cos(M_PI * seta / 180.0)*R1;

        m_abutVertexArray[2 * i].x     = v1x;
        m_abutVertexArray[2 * i].y     = v1y;
        m_abutVertexArray[2 * i].z     = L1;

        m_abutVertexArray[2 * i + 1].x = v1x;
        m_abutVertexArray[2 * i + 1].y = v1y;
        m_abutVertexArray[2 * i + 1].z = 0.0;
    }
    getAbutNormalArray();
}

void GLModel::getAbutNormalArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        CalcNormals(m_abutVertexArray[2 * i], m_abutVertexArray[2 * i + 1], m_abutVertexArray[2 * i + 3], &m_abutNormalArray[2 * i]);
        CalcNormals(m_abutVertexArray[2 * i], m_abutVertexArray[2 * i + 3], m_abutVertexArray[2 * i + 2], &m_abutNormalArray[2 * i + 1]);
    }
    CalcNormals(m_abutVertexArray[2 * m_faceCount - 2], m_abutVertexArray[2 * m_faceCount - 1], m_abutVertexArray[1], &m_abutNormalArray[2 * m_faceCount - 2]);
    CalcNormals(m_abutVertexArray[2 * m_faceCount - 2], m_abutVertexArray[1], m_abutVertexArray[0], &m_abutNormalArray[2 * m_faceCount - 1]);

    int st = 2 * m_faceCount;
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        CalcNormals(m_abutVertexArray[0], m_abutVertexArray[2 * i + 2], m_abutVertexArray[2 * i + 4], &m_abutNormalArray[st + i]);
    }
    st += m_faceCount - 2;
    for(int i = 0; i < m_faceCount - 2; i++)
    {
        CalcNormals(m_abutVertexArray[1], m_abutVertexArray[2 * i + 3], m_abutVertexArray[2 * i + 5],&m_abutNormalArray[st + i]);
    }
}

void GLModel::getAbutColorArray()
{
   for (int i=0; i < m_abutVertexCount; i++)
   {
      m_abutColorArray[i].x=0.0f;
      m_abutColorArray[i].y=1.0f;
      m_abutColorArray[i].z=0.0f;
   }
}

void GLModel::getAbutIndexArray()
{
    for(int i = 0; i < m_faceCount - 1; i++)
    {
        m_abutIndexArray[2 * i].x = 2 * i;
        m_abutIndexArray[2 * i].y = 2 * i + 1;
        m_abutIndexArray[2 * i].z = 2 * i + 3;

        m_abutIndexArray[2 * i + 1].x = 2 * i;
        m_abutIndexArray[2 * i + 1].y = 2 * i + 3;
        m_abutIndexArray[2 * i + 1].z = 2 * i + 2;
    }

   m_abutIndexArray[m_faceCount * 2 - 2].x = m_faceCount * 2 - 2;
   m_abutIndexArray[m_faceCount * 2 - 2].y = m_faceCount * 2 - 1;
   m_abutIndexArray[m_faceCount * 2 - 2].z = 1;

   m_abutIndexArray[m_faceCount * 2 - 1].x = m_faceCount * 2 - 2;
   m_abutIndexArray[m_faceCount * 2 - 1].y = 1;
   m_abutIndexArray[m_faceCount * 2 - 1].z = 0;

   int st = 2 * m_faceCount;
   //верхн€€ грань
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       m_abutIndexArray[st + i].x = 0;
       m_abutIndexArray[st + i].y = i * 2 + 2;
       m_abutIndexArray[st + i].z = i * 2 + 4;
   }
   st += m_faceCount - 2;
   for(int i = 0; i < m_faceCount - 2; i++)
   {
       m_abutIndexArray[st + i].x = 1;
       m_abutIndexArray[st + i].y = i * 2 + 3;
       m_abutIndexArray[st + i].z = i * 2 + 5;
   }
}

void GLModel::drawAbutment()
{
    glNormalPointer(GL_FLOAT, 0, m_abutNormalArray);
    glVertexPointer(3, GL_FLOAT, 0, m_abutVertexArray);
    glColorPointer(3, GL_FLOAT, 0, m_abutColorArray);
    glDrawElements(GL_TRIANGLES, (m_abutTrianglesCount) * 3, GL_UNSIGNED_SHORT, m_abutIndexArray);
}
