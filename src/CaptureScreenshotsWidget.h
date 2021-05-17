#pragma once
#include <vtkSmartPointer.h>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class vtkRenderWindow;

class CaptureScreenshotsWidget : public QWidget
{
    Q_OBJECT

public:
    CaptureScreenshotsWidget(QWidget* parent = nullptr);

private slots:
    void browseProjectFiles();
    void browseMainDir();
    void startCapturing();
    void clearParameters();

private:
    void takeScreenshot(vtkSmartPointer<vtkRenderWindow> renderWindow,
        const std::string& filename, const bool setTransparentBackgroud = false);

    QStringList projectFiles;
    QString mainDir;
    QLineEdit* lineProjects;
    QLineEdit* lineMainDir;
};