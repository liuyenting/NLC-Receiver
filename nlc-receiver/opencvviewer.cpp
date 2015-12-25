#include "opencvviewer.hpp"
#include <QOpenGLFunctions>

OpenCVViewer::OpenCVViewer(QWidget *parent)
    : QOpenGLWidget(parent), scaleRatio(2)
{
    isSceneChanged = false;
    bgColor = QColor::fromRgb(150, 150, 150);

    outH = outW = 0;
    imgRatio = 4.0f/3.0f;

    posX = posY = 0;
}

void OpenCVViewer::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();

    float r = ((float)bgColor.darker().red())/255.0f;
    float g = ((float)bgColor.darker().green())/255.0f;
    float b = ((float)bgColor.darker().blue())/255.0f;
    glClearColor(r, g, b, 1.0f);
}

void OpenCVViewer::resizeGL(int width, int height)
{
    makeCurrent();
    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, width, 0, height, 0, 1);	// To Draw image in the center of the area

    glMatrixMode(GL_MODELVIEW);

    // ---> Scaled Image Sizes
    outH = width/imgRatio;
    outW = width;

    if(outH>height)
    {
        outW = height*imgRatio;
        outH = height;
    }

    emit imageSizeChanged( outW, outH );
    // <--- Scaled Image Sizes

    posX = (width-outW)/2;
    posY = (height-outH)/2;

    isSceneChanged = true;

    updateScene();
}

void OpenCVViewer::updateScene()
{
    if(isSceneChanged && this->isVisible())
        update();
}

void OpenCVViewer::paintGL()
{
    makeCurrent();

    if(!isSceneChanged)
        return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderImage();

    isSceneChanged = false;
}

void OpenCVViewer::renderImage()
{
    makeCurrent();

    glClear(GL_COLOR_BUFFER_BIT);

    if (!renderedImg.isNull()) {
        glLoadIdentity();

        QImage image;

        // TODO: CLEANUP THE CODES HERE
        glPushMatrix();
        {
            int inW = renderedImg.width();
            int inH = renderedImg.height();

            // Resize the image to the viewer.
            if((inW != this->size().width()) && (inH != this->size().height()))
            {
                fprintf(stderr, "need to resize\n");

                image = renderedImg.scaled(this->size() * scaleRatio,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
            }
            else
                image = renderedImg;

            // Centering the image.
            posX = (this->size().width() - inW/scaleRatio)/2;
            posY = (this->size().height() - inH/scaleRatio)/2;
            glRasterPos2i(posX, posY);

            // Update to the rescaled size.
            inW = image.width();
            inH = image.height();

            glDrawPixels(inW, inH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        glFlush();
    }
}

bool OpenCVViewer::showImage(cv::Mat image)
{
    image.copyTo(originalImg);

    imgRatio = (float)image.cols/(float)image.rows;

    if( originalImg.channels() == 3)
        renderedImg = QImage((const unsigned char*)(originalImg.data),
                              originalImg.cols, originalImg.rows,
                              originalImg.step, QImage::Format_RGB888)/*.rgbSwapped()*/;
    else if( originalImg.channels() == 1)
        renderedImg = QImage((const unsigned char*)(originalImg.data),
                              originalImg.cols, originalImg.rows,
                              originalImg.step, QImage::Format_Indexed8);
    else
        return false;

    isSceneChanged = true;
    updateScene();

    return true;
}
