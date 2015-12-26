#ifndef OPENCVVIEWER_HPP
#define OPENCVVIEWER_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>
#include <opencv2/core/mat.hpp>

class OpenCVViewer : public QOpenGLWidget, protected QOpenGLFunctions_2_0
{
    Q_OBJECT

public:
    explicit OpenCVViewer(QWidget *parent = 0);

signals:
    void imageSizeChanged(int outW, int outH);

public slots:
    bool showImage(const cv::Mat3b &srcImg);
    cv::Mat3b dumpImage();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void updateScene();
    void renderImage();

private:
    // Indicates whether OpenGL view has to re-render.
    bool isSceneChanged;

    QImage renderedImg;
    cv::Mat3b originalImg;

    // Default background color.
    QColor bgColor;

    // Image width, height and aspect ratio(H/W).
    int outW, outH;
    float imgRatio;

    // Top left position of the rendered image.
    int posX, posY;

    int scaleRatio;
};

#endif
