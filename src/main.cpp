#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2); /* VTK was built with vtkRenderingOpenGL2 */
VTK_MODULE_INIT(vtkInteractionStyle);

#include <QApplication>
#include <qsurfaceformat.h>

#include <QVTKOpenGLNativeWidget.h>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    MainWindow mainWin;
    mainWin.setMinimumSize(800, 600);
    mainWin.show();
    return app.exec();
}