#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout(this->centralWidget());
    QPushButton* im = new QPushButton("Choose image", this->centralWidget());
    connect(im, SIGNAL(pressed()), this, SLOT(OpenFile()));

    contrast = new QPushButton("Contrast strething", this->centralWidget());
    connect(contrast, SIGNAL(pressed()), this, SLOT(contrastStretching()));
    contrast->setDisabled(true);

    mkhist = new QPushButton("make histogramm equalizing", this->centralWidget());
    connect(mkhist, SIGNAL(pressed()), this, SLOT(histogramm()));
    mkhist->setDisabled(true);

    mkhistrgb = new QPushButton("make histogramm equalizing on 3 color components", this->centralWidget());
    connect(mkhistrgb, SIGNAL(pressed()), this, SLOT(colorHistogramEqualization()));
    mkhistrgb->setDisabled(true);

    mkhisthsv = new QPushButton("make histogramm equalizing on brightness component in hsv", this->centralWidget());
    connect(mkhisthsv, SIGNAL(pressed()), this, SLOT(hsvHistogramEqualization()));
    mkhisthsv->setDisabled(true);

    seg = new QPushButton("do segmentation", this->centralWidget());
    connect(seg, SIGNAL(pressed()), this, SLOT(segmentation()));
    seg->setDisabled(true);

    layout->addWidget(im);
    layout->addWidget(contrast);
    layout->addWidget(mkhist);
    layout->addWidget(mkhistrgb);
    layout->addWidget(mkhisthsv);
    layout->addWidget(seg);
}

void MainWindow::segmentation()
{
    cv::Mat iig;
    inputImage.copyTo(iig);
    if (inputImage.channels() == 3 || inputImage.type() == CV_8UC3)
    {
        cv::cvtColor(inputImage, iig, cv::COLOR_BGR2GRAY);
    }

    cv::Mat canny_output;
    Canny( inputImage, canny_output, 75, 150 );
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );
    cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );
    cv::RNG rng(12345);
    for( size_t i = 0; i< contours.size(); i++ )
    {
        cv::Scalar color = cv::Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 1, cv::LINE_8, hierarchy, 0 );
    }
    imshow("Input Image", inputImage);
    imshow( "Contours", drawing );

}

void MainWindow::OpenFile()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::AnyFile);
    QString fileName = fileDialog->getOpenFileName(this, tr("Open Image"), "C:/", tr("Image Files (*.png *.jpg *.bmp *.gif *.tiff *.pcx)"));
    inputImage = cv::imread(fileName.toStdString(), cv::IMREAD_ANYCOLOR);
    if (!inputImage.empty())
    {

        contrast->setDisabled(false);
        seg->setDisabled(false);
        if (inputImage.channels() == 3 || inputImage.type() == CV_8UC3)
        {
            mkhistrgb->setDisabled(false);
            mkhisthsv->setDisabled(false);
            mkhist->setDisabled(true);
        }
        else
        {
            mkhistrgb->setDisabled(true);
            mkhisthsv->setDisabled(true);
            mkhist->setDisabled(false);
        }
    }
    else
    {
        seg->setDisabled(true);
        contrast->setDisabled(true);
        mkhist->setDisabled(true);
        mkhistrgb->setDisabled(true);
        mkhisthsv->setDisabled(true);
    }


}

void MainWindow::histogramm()
{
    cv::Mat grayImage;
    //cv::cvtColor(inputImage, grayImage, cv::color);
    cv::Mat processedImage;
    cv::equalizeHist(inputImage, processedImage);
    cv::imshow("Input Image", inputImage);
    cv::imshow("Processed Image", processedImage);
}

void MainWindow::colorHistogramEqualization()
{
    // Разделение изображения на отдельные цветовые компоненты
    std::vector<cv::Mat> channels;
    cv::split(inputImage, channels);

    // Выравнивание гистограммы для каждой цветовой компоненты
    for (int i = 0; i < 3; i++) {
        equalizeHist(channels[i], channels[i]);
    }

    // Слияние цветовых компонент обратно в одно изображение
    cv::Mat result;
    merge(channels, result);

    cv::imshow("Input Image", inputImage);
    cv::imshow("Processed Image", result);
}

void MainWindow::hsvHistogramEqualization()
{
    // Преобразование изображения из RGB в HSV
    cv::Mat hsvImage;
    cv::cvtColor(inputImage, hsvImage, cv::COLOR_BGR2HSV);

    // Разделение изображения на отдельные каналы (H, S, V)
    std::vector<cv::Mat> channels;
    split(hsvImage, channels);

    // Выравнивание гистограммы только для компоненты яркости (V)
    equalizeHist(channels[2], channels[2]);

    // Слияние каналов обратно в одно изображение
    cv::Mat result;
    merge(channels, result);

    // Преобразование изображения обратно в пространство RGB
    cvtColor(result, result, cv::COLOR_HSV2BGR);

    cv::imshow("Input Image", inputImage);
    cv::imshow("Processed Image", result);
}

void MainWindow::contrastStretching()
{
     if (inputImage.channels() == 3 || inputImage.type() == CV_8UC3)
    {
        cv::Mat hsvImage;
        cv::cvtColor(inputImage, hsvImage, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsvChannels;
        cv::split(hsvImage, hsvChannels);
        cv::normalize(hsvChannels[2], hsvChannels[2], 0, 255, cv::NORM_MINMAX);
        cv::Mat processedHSVImage;
        cv::merge(hsvChannels, processedHSVImage);
        cv::Mat processedImage;
        cv::cvtColor(processedHSVImage, processedImage, cv::COLOR_HSV2BGR);
        cv::imshow("Input Image", inputImage);
        cv::imshow("Processed Image", processedImage);
        //cv::waitKey(0);
    }
    else if (inputImage.channels() == 1 || inputImage.type() == CV_8UC1)
    {
        double minVal, maxVal;
        cv::minMaxLoc(inputImage, &minVal, &maxVal);

        double lower = 0;
        double upper = 255;

        double factor = (upper - lower) / (maxVal - minVal);

        cv::Mat stretchedImage;
        inputImage.convertTo(stretchedImage, CV_8U, factor, -factor * minVal);

        cv::imshow("Input Image", inputImage);
        cv::imshow("Processed Image", stretchedImage);
        //cv::waitKey(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

