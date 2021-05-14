#pragma once
#include <qwidget.h>

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

class CaptureScreenshotsWidget : public QWidget
{
    Q_OBJECT

public:
    CaptureScreenshotsWidget(QWidget* parent = nullptr);

private slots:
    void browseCameraFiles();
    void browseMainDir();
    void startCapturing();
    void clearParameters();

private:
    QStringList findFiles(const QStringList& files, const QString& text);

    QStringList cameraFiles;
    QString mainDir;
    QLineEdit* lineCameras;
    QLineEdit* lineMainDir;
};