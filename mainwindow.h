#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>


#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgcodecs.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OpenFile();
    void contrastStretching();
    void histogramm();
    void colorHistogramEqualization();
    void hsvHistogramEqualization();
    void segmentation();

private:
    Ui::MainWindow *ui;
    cv::Mat inputImage;
    QPushButton* contrast;
    QPushButton* mkhist;
    QPushButton* mkhistrgb;
    QPushButton* mkhisthsv;
    QPushButton* seg;


};
#endif // MAINWINDOW_H
