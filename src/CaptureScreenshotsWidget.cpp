#include "CaptureScreenshotsWidget.h"

#include <qpushbutton.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>

CaptureScreenshotsWidget::CaptureScreenshotsWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Capture Screenshots"));

    QPushButton* browseButtonCameras = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonCameras, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseCameraFiles);
    lineCameras = new QLineEdit(this);

    QPushButton* browseButtonDir = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonDir, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseMainDir);
    lineMainDir = new QLineEdit(this);

    QPushButton* clearButton = new QPushButton(tr("&Clear data"), this);
    connect(clearButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::clearParameters);

    QPushButton* startButton = new QPushButton(tr("&Start capturing..."), this);
    connect(startButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::startCapturing);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Cameras:")), 0, 0);
    mainLayout->addWidget(lineCameras, 0, 1);
    mainLayout->addWidget(browseButtonCameras, 0, 2);

    mainLayout->addWidget(new QLabel(tr("Main dir:")), 1, 0);
    mainLayout->addWidget(lineMainDir, 1, 1);
    mainLayout->addWidget(browseButtonDir, 1, 2);

    mainLayout->addWidget(clearButton, 2, 1, Qt::AlignRight);
    mainLayout->addWidget(startButton, 2, 2, Qt::AlignRight);
}

void CaptureScreenshotsWidget::browseCameraFiles()
{

}

void CaptureScreenshotsWidget::browseMainDir()
{
}

void CaptureScreenshotsWidget::startCapturing()
{
}

void CaptureScreenshotsWidget::clearParameters()
{
}
