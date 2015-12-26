#include "opencvviewer.hpp"
#include <QOpenGLFunctions>
#include <opencv2/core/affine.hpp>

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

    // Draw image in the center of the area.
    glOrtho(0, width, 0, height, 0, 1);

    glMatrixMode(GL_MODELVIEW);

    outH = width/imgRatio;
    outW = width;

    if(outH > height) {
        outW = height*imgRatio;
        outH = height;
    }

    emit imageSizeChanged(outW, outH);

    posX = (width - outW)/2;
    posY = (height - outH)/2;

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

        glPushMatrix();
        {
            int inW = renderedImg.width();
            int inH = renderedImg.height();

            // Target screen region, full-area size.
            outW = this->size().width() * scaleRatio;
            outH = this->size().height() * scaleRatio;

            // Resize the image to the viewer.
            if((inW != outW) && (inH != outH)) {
                image = renderedImg.scaled(outW, outH,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation);
            }
            else
                image = renderedImg;

            // Update to the rescaled image size.
            outW = image.width();
            outH = image.height();

            // Centering the image.
            posX = (this->size().width() - outW/scaleRatio)/2;
            posY = (this->size().height() - outH/scaleRatio)/2;
            glRasterPos2i(posX, posY);

            glDrawPixels(outW, outH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        }
        glPopMatrix();

        glFlush();
    }
}

bool OpenCVViewer::showImage(const cv::Mat3b &srcImg)
{
    cv::Mat3b tmp;
    // Source image is always flipped.
    cv::flip(srcImg, originalImg, 0);

    imgRatio = (float)originalImg.cols/(float)originalImg.rows;

    // Copy the array into QImage displayable format.
    renderedImg = QImage(originalImg.cols, originalImg.rows, QImage::Format_ARGB32);
    for (int y = 0; y < originalImg.rows; ++y) {
        const cv::Vec3b *srcrow = originalImg[y];
        QRgb *destrow = (QRgb*)renderedImg.scanLine(y);
        for (int x = 0; x < originalImg.cols; ++x) {
            // Original image is sent in as BGR, manually reverse back to RGB.
            destrow[x] = qRgba(srcrow[x][0], srcrow[x][1], srcrow[x][2], 255);
        }
    }

    isSceneChanged = true;
    updateScene();

    return true;
}

cv::Mat3b OpenCVViewer::dumpImage() {
    return originalImg;
}
