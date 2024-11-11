/*
* /home/nishi/usr/local/src/cpp-nishi/nishi-many-test/opencv_RotatedRect/main.cpp
*
* main.cpp
*    OpenCVで画像から回転矩形領域を切り出す
*    https://qiita.com/vs4sh/items/93d65468a992af5b8f92
*
*   https://docs.hsp.moe/OpenCV/453/cpp/ja/classcv_1_1_rotated_rect.html
*     arctan を求める
*     https://cpprefjp.github.io/reference/cmath/atan2.html
*
*
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

#ifndef RADIANS_F
#define RADIANS_F   57.29577951308232    // [deg/rad]
#endif

int main() {


    float ry_ = 1.5; // robo 走行コース幅の半分 [M] 

    cv::Mat  binary,blur,gray;
    //cv::Mat src = cv::imread("aaa.png", cv::IMREAD_COLOR);
    //cv::Mat gray = cv::imread("../house_map.pgm", cv::IMREAD_GRAYSCALE);
    //cv::Mat gray = cv::imread("../house_map-alt.pgm", cv::IMREAD_GRAYSCALE);

    cv::Mat src = cv::imread("../grace_hopper_processed.bmp", cv::IMREAD_COLOR);

    //cv::Mat src2 = src.clone();

    cv::imshow("src", src);
    cv::waitKey(1000);

    int sx_m=90;
    int sy_m=200;
    int dx_m=400;
    int dy_m=290;

    // OpenCVで画像から回転矩形領域を切り出す
    // https://qiita.com/vs4sh/items/93d65468a992af5b8f92


    // arctan を求める
    // https://cpprefjp.github.io/reference/cmath/atan2.html
    int off_x = dx_m - sx_m;
    int off_y = dy_m - sy_m;

    float tan_rad = std::atan2((float)off_y, (float)off_x);

    // 走行幅矩形のCenter を求める。
    int cnter_x_m = (dx_m - sx_m)/2 + sx_m;
    int cnter_y_m = (dy_m - sy_m)/2 + sy_m;


    // https://docs.hsp.moe/OpenCV/453/cpp/ja/classcv_1_1_rotated_rect.html

    cv::Point2f center_f((float)cnter_x_m,(float)cnter_y_m);

    // start - stop の距離を求める。
    float dist = std::sqrt((float)(off_x*off_x + off_y*off_y));

    // 高さ = ry_
    //float h_f = (ry_ / get_gcostmap_->mapm_.resolution)*2;
    float h_f = 80;

    cv::Size2f size_f(dist,h_f);

    cv::RotatedRect rect(center_f, size_f, tan_rad * RADIANS_F); // 回転矩形

    cv::Rect rect_val =	rect.boundingRect();

    //auto rect_val =	rect.boundingRect();

    //printf("%s",rect_val);

    std::cout << rect_val << std::endl;


    cv::Mat M; // 回転行列
    cv::Mat rotated; // 回転された元画像
    cv::Mat cropped; // 切り出された画像

    float angle = rect.angle;
    cv::Size rect_size = rect.size;
    if (rect.angle < -45.) {
        angle += 90.0;
        std::swap(rect_size.width, rect_size.height);
    }

    // 回転矩形の角度から回転行列を計算する．
    M = cv::getRotationMatrix2D(rect.center, angle, 1.0);
    // 元画像を回転させる．
    cv::warpAffine(src, rotated, M, src.size(), cv::INTER_CUBIC);

    // 回転した画像から矩形領域を切り出す．
    // http://opencv.jp/opencv-2svn/cpp/imgproc_geometric_image_transformations.html
    cv::getRectSubPix(rotated, rect_size, rect.center, cropped);

    cv::imshow("cropped", cropped);
    cv::waitKey(1000);

    #define TEST1
    #if defined(TEST1)
        // 2. 描画
        // 塗りつぶさない場合 - cv::polylines()
        cv::Point2f vertices2f[4];
        rect.points(vertices2f);
        std::vector<cv::Point> vertices;
        for (int i = 0; i < 4; ++i) {
            vertices.push_back(vertices2f[i]);
        }
        const cv::Point* pts = (const cv::Point*) cv::Mat(vertices).data;
        int npts = cv::Mat(vertices).rows;
        cv::polylines(src, &pts, &npts, 1, true, cv::Scalar(0, 255, 0), 1);


        cv::imshow("src_2", src);
        cv::waitKey();
    #endif

    return 0;
}


