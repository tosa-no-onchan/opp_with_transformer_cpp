/*
* /home/nishi/Documents/VisualStudio-CPP/opp_with_transformer_cpp/cource_class_gen.cpp
* cource_class_gen.cpp
*
* http://www.netosa.com/blog/2024/09/ros2-turtlebot3-8-thetastarplanner.html
*
* コースライン画像からコースクラステキストを作る。
*
*  by nishi
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

//#include <chrono>
//#include <functional>
//#include <memory>
#include <string>
#include <math.h>

#include <filesystem>


#ifndef RADIANS_F
#define RADIANS_F   57.29577951308232    // [deg/rad]
#endif


/**
* @brief フォルダ以下のファイル一覧を取得する関数
* @param[in]    folderPath  フォルダパス
* @param[out]   file_names  ファイル名一覧
* return        true:成功, false:失敗
*
* https://phst.hateblo.jp/entry/2019/02/17/003027
* https://qiita.com/tes2840/items/8d295b1caaf10eaf33ad
* sort
* https://cpprefjp.github.io/reference/algorithm/sort.html
*/
bool getFileNames(std::string folderPath, std::vector<std::string> &file_names){
    //using namespace std::filesystem;
    namespace fs = std::filesystem;
    fs::path path1(folderPath);

    // https://en.cppreference.com/w/cpp/filesystem/path/filename

    //ファイルをリスト化
    if (fs::is_directory(path1)) {
        //std::cout << "Directory: " << fs::absolute(path1).string() << std::endl;
        //std::cout << "Files: " << std::endl;
        auto dir_it = fs::directory_iterator(path1);
        for (auto &p : dir_it) {
            std::string s = p.path().filename().string();
            //std::cout <<" p.path().filename().string():"<< s << std::endl;
            file_names.push_back(s);
            //file_names.push(s);
        }
    }
    else
        return false;

    return true;
}


class GenClassText{
public:
    std::vector<std::string> nn_l_files_;

    // コース画像は、global cost map から、コース矩形を抽出して、 2倍サイズで保存しています。
    //   global cost map resolution= 0.05
    //   コース画像 resolution= 0.05 * 2 -> 0.025
    // コース画像
    //   rows : 122  ->  122 x 0.025 = 3.05[M]
    //   class num : (rows - 12*2 ) / 2 = 49
    //           class id= 0 - 48
    //           center = 24
    //           'B'    = 49
    //   cols :
    // ロボットは、3.05[M] より、内側 0.3[M](robot radius) を通るので、 2.4[M] が、Max ライン幅
    //   0.3 / 0.025 = 12[dot] 内側を通る
    //
    // 注) コース幅を、 120 -> 122 に替えました。 by nishi 2024.10.12
    //     Center の コース線 2[dot] を、確保しました。

    int x_interval=4;       // 今は、適当。 model の出力による。
    //int x_interval=2;       // 今は、適当。 model の出力による。

    int rows_=122;

    int y_interval=1;
    int ry_min=12;           // 12  +y より、内側 0.3 [M]
    int ry_max=122-12;       // 108  -y より、内側 0.3[M]

    int black_color_th = 107;

    bool cont_f_ =true;      // yaml 作成を、途中から継続する。
    bool all_f_ = false;

    GenClassText(){}
    void init(std::string path){
        path_ = path;
    }

    void get_list(){
        getFileNames(path_, file_names_);
        for(std::string s : file_names_){
            //std::cout << " s:"<< s << std::endl;
            if(s.length() > 6){
                if(s.substr(s.size() - 6, 6) == "-l.jpg"){
                    //s.erase(s.size() - 4, 4);
                    //std::cout << " s:"<< s << std::endl;
                    nn_l_files_.push_back(s);
                    //gen_exec(s);
                }
            }
        }
    }
    void gen_exec(std::string fname){

        FILE* yaml_fp;

        if(fname.length() > 6 && fname.substr(fname.size() - 6, 6) == "-l.jpg"){
            yaml_names_ = fname.substr(0,fname.size() - 3) + "yaml";
            //std::cout<< " yaml_names_:"<< yaml_names_ << std::endl;
            image_names_ = fname.substr(0,fname.size() - 6) + ".jpg";
            //std::cout<< " image_names_:"<< image_names_ << std::endl;
        }
        else{
            std::cout<< " line image file name is not correct" << std::endl;
            return;
        }

        // gen_all の時、継続処理だと、既存の yaml は、更新しません。　add by nishi 2024.10.19
        if(all_f_==true && cont_f_ ==true){
            std::string fs = path_+"/"+yaml_names_;
            //std::cout <<"fs:"<< fs<< std::endl;
            if( std::filesystem::is_regular_file(fs) == true){
                return;
            }
        }

        std::cout<< " yaml_names_:"<< yaml_names_ << std::endl;
        std::cout<< " image_names_:"<< image_names_ << std::endl;


        int row_av_23_cnt=0;

        cv::Mat src_img = cv::imread(path_+"/"+fname, cv::IMREAD_GRAYSCALE);

        // 画像幅
        std::cout<< "src_img.cols:"<< src_img.cols << std::endl;
        // 画像高さ
        std::cout<< "src_img.rows:"<< src_img.rows << std::endl;

        //std::cout<< "src_img.depth():"<<src_img.depth() << std::endl;
        //std::cout<< "src_img.channels():"<<src_img.channels() << std::endl;
        //std::cout<< "src_img.step:"<<src_img.step << std::endl;
        //std::cout<< "src_img.type():"<<src_img.type() << std::endl;

        int cols_num=0;
        std::string data_s="";
        for(int cols_x =0; cols_x < src_img.cols; cols_x+=x_interval){
            // row は、全て拾う事
            int row_sum=0;
            int row_cnt=0;
            int row_av=0;
            // 1 列分を処理 上から下へ、黒を拾う
            // 注) 古い画像は、rows=120 新しい画像は、 rows=122
            //  古い画像の下 2[dot] は、無視する。
            //for(int rows_x=ry_min; rows_x < src_img.rows-ry_min ; rows_x++){
            for(int rows_x=ry_min; rows_x < ry_max ; rows_x++){
                //cv::Point p = cv::Point(rows_x,cols_x);
                //auto x0 = src_img.at<cv::Vec3b>(rows_x,cols_x)[0];
                auto x = src_img.data[rows_x*src_img.step+cols_x];
                //printf("x:%d",x);
                if(x <= black_color_th){
                    //printf("x:%d",x);
                    //std::cout <<" rows_x:"<<rows_x<< " cols_x:"<<cols_x << std::endl;
                    row_sum += rows_x;
                    row_cnt++;
                }
            }
            if(row_cnt >0){
                row_av = row_sum / row_cnt;
                row_av -= ry_min;
                row_av /=2;
            }
            else{
                row_av=-1;
            }
            std::ostringstream oss;
            oss << row_av;

            if(data_s==""){
                data_s=oss.str();
            }
            else{
                data_s+=" "+oss.str();
            }
            //std::cout <<" cols_num:" << cols_num <<" row_av:"<< row_av << " cols_x:"<<cols_x << std::endl;
            cols_num++;
            if(row_av == 23){
                row_av_23_cnt++;
            }
        }

        // rows=122
        // (122 - 12*2 ) / 2 = 49
        int class_num = (rows_ -ry_min*2 ) / 2;

        std::cout <<"yaml path:"<<path_+"/"+yaml_names_<< std::endl;

        yaml_fp = fopen((path_+"/"+yaml_names_).c_str(), "w");

        //fprintf(yaml_fp, "image: %s\nresolution: %f\norigin: [%f, %f, %f]\nnegate: 0\noccupied_thresh: 0.65\nfree_thresh: 0.196\n\n",
        //        mapdatafile.c_str(), map.info.resolution, map.info.origin.position.x, map.info.origin.position.y, yaw);

        fprintf(yaml_fp, "image: %s\ncols: %d\nrows: %d\nx_interval: %d\ndata_num: %d\nclass_num: %d\n",
                image_names_.c_str(),src_img.cols,src_img.rows, x_interval, cols_num,class_num);

        fprintf(yaml_fp, "data: %s\n\n",
                data_s.c_str());

        fclose(yaml_fp);

        if(row_av_23_cnt >= 3){
            std::cout <<" row_av_23_cnt:"<< row_av_23_cnt << "  cnt exceed!!" << std::endl;
        }

        //std::cout <<" cols_num:"<< cols_num << std::endl;
        //cv::imshow("src_img", src_img);
        //cv::waitKey(1000);
    }

    /*
    * 平均値を使う
    */
    void gen_exec_ave(std::string fname){

        FILE* yaml_fp;

        if(fname.length() > 6 && fname.substr(fname.size() - 6, 6) == "-l.jpg"){
            yaml_names_ = fname.substr(0,fname.size() - 3) + "yaml";
            //std::cout<< " yaml_names_:"<< yaml_names_ << std::endl;
            image_names_ = fname.substr(0,fname.size() - 6) + ".jpg";
            //std::cout<< " image_names_:"<< image_names_ << std::endl;
        }
        else{
            std::cout<< " line image file name is not correct" << std::endl;
            return;
        }

        // gen_all の時、継続処理だと、既存の yaml は、更新しません。　add by nishi 2024.10.19
        if(all_f_==true && cont_f_ ==true){
            std::string fs = path_+"/"+yaml_names_;
            //std::cout <<"fs:"<< fs<< std::endl;
            if( std::filesystem::is_regular_file(fs) == true){
                return;
            }
        }

        std::cout<< " yaml_names_:"<< yaml_names_ << std::endl;
        std::cout<< " image_names_:"<< image_names_ << std::endl;


        int row_av_23_cnt=0;

        cv::Mat src_img = cv::imread(path_+"/"+fname, cv::IMREAD_GRAYSCALE);

        // 画像幅
        std::cout<< "src_img.cols:"<< src_img.cols << std::endl;
        // 画像高さ
        std::cout<< "src_img.rows:"<< src_img.rows << std::endl;

        //std::cout<< "src_img.depth():"<<src_img.depth() << std::endl;
        //std::cout<< "src_img.channels():"<<src_img.channels() << std::endl;
        //std::cout<< "src_img.step:"<<src_img.step << std::endl;
        //std::cout<< "src_img.type():"<<src_img.type() << std::endl;

        int cols_num=0;
        std::string data_s="";
        for(int cols_x =x_interval-1; cols_x < src_img.cols; cols_x+=x_interval){
            // row は、全て拾う事
            int row_sum=0;
            int row_cnt=0;
            int row_av=0;
            bool black_line=false;
            // 1 列分を処理 上から下へ、黒を拾う
            // 注) 古い画像は、rows=120 新しい画像は、 rows=122
            //  古い画像の下 2[dot] は、無視する。
            //for(int rows_x=ry_min; rows_x < src_img.rows-ry_min ; rows_x++){
            for(int cur_cols_x = cols_x - (x_interval-1); cur_cols_x <= cols_x; cur_cols_x++){
                int cur_row_cnt=0;
                for(int rows_x=ry_min; rows_x < ry_max ; rows_x++){
                    //cv::Point p = cv::Point(rows_x,cols_x);
                    //auto x0 = src_img.at<cv::Vec3b>(rows_x,cols_x)[0];
                    auto x = src_img.data[rows_x*src_img.step+cur_cols_x];
                    //printf("x:%d",x);
                    if(x <= black_color_th){
                        //printf("x:%d",x);
                        //std::cout <<" rows_x:"<<rows_x<< " cols_x:"<<cols_x << std::endl;
                        row_sum += rows_x;
                        row_cnt++;
                        cur_row_cnt++;
                    }
                }
                // 1列でも、コースラインがなければ、Black にする。
                if(cur_row_cnt == 0){
                    black_line=true;
                }
            }
            if(row_cnt >0 && black_line==false){
                row_av = row_sum / row_cnt;
                row_av -= ry_min;
                row_av /=2;
            }
            else{
                row_av=-1;
            }
            std::ostringstream oss;
            oss << row_av;

            if(data_s==""){
                data_s=oss.str();
            }
            else{
                data_s+=" "+oss.str();
            }
            //std::cout <<" cols_num:" << cols_num <<" row_av:"<< row_av << " cols_x:"<<cols_x << std::endl;
            cols_num++;
            if(row_av == 23){
                row_av_23_cnt++;
            }
        }

        // rows=122
        // (122 - 12*2 ) / 2 = 49
        int class_num = (rows_ -ry_min*2 ) / 2;

        std::cout <<"yaml path:"<<path_+"/"+yaml_names_<< std::endl;

        yaml_fp = fopen((path_+"/"+yaml_names_).c_str(), "w");

        //fprintf(yaml_fp, "image: %s\nresolution: %f\norigin: [%f, %f, %f]\nnegate: 0\noccupied_thresh: 0.65\nfree_thresh: 0.196\n\n",
        //        mapdatafile.c_str(), map.info.resolution, map.info.origin.position.x, map.info.origin.position.y, yaw);

        fprintf(yaml_fp, "image: %s\ncols: %d\nrows: %d\nx_interval: %d\ndata_num: %d\nclass_num: %d\n",
                image_names_.c_str(),src_img.cols,src_img.rows, x_interval, cols_num,class_num);

        fprintf(yaml_fp, "data: %s\n\n",
                data_s.c_str());

        fclose(yaml_fp);

        if(row_av_23_cnt >= 3){
            std::cout <<" row_av_23_cnt:"<< row_av_23_cnt << "  cnt exceed!!" << std::endl;
        }

        //std::cout <<" cols_num:"<< cols_num << std::endl;
        //cv::imshow("src_img", src_img);
        //cv::waitKey(1000);
    }

    void gen_all(){
        get_list();
        for(std::string s : nn_l_files_){
            all_f_=true;
            //gen_exec(s);
            // 平均値を使う
            gen_exec_ave(s);
        }
    }
private:
    std::string path_="/home/nishi/colcon_ws/src/turtlebot3_navi_my/ml_data/image";
    std::vector<std::string> file_names_;
    std::string image_names_;
    std::string yaml_names_;
};


int main() {

    std::string data_path="/home/nishi/colcon_ws/src/turtlebot3_navi_my/ml_data/image";
    GenClassText genClassText;

    genClassText.init(data_path);
    //genClassText.cont_f_ =false;
    genClassText.gen_all();

    //genClassText.gen_exec_ave("1-l.jpg");
    //genClassText.gen_exec("4-l.jpg");
    return 0;

}