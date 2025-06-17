# cource_classgen.cpp  

It makes the course class file(n-l.yaml) from cource line images(n-l.jpg)  

Save n.jpg image files from a part of Ros2 Global cost map.   
The robo moves from left center to right centr avoiding obstacle.  

Usual, run C++ Auto Mower with ml_data:=True  
$ ros2 launch turtlebot3_navi_my go_auto_mower.launch.py use_sim_time:=[True] ml_data:=True

This images size 122 x Max 600 with map resolution 0.025  
Usually, ROS2 Global costmap resolution is 0.05 , then it's original images size is 61 x Max 300  

![n.jpg file](https://github.com/tosa-no-onchan/opp_with_transformer_cpp/blob/main/images/1.jpg)  

With Gimp, drow a center line for robo mower move on it avoiding obstacle inflatuion and save n-l.jpg as cource line images.  
Line is 2 dot width.  
On Gimp, you had better to add a lines layer and white layer avobe or below original layer.  
Drow a center black line with 2 dot pencil on a lines layer.  
If no obstacle is on center , the line becames a straight-line.   
At last,export lines layer and white layer as to n-l.jpg and save all data to n.xcf(gimp original format file).  

![n-l.jpg file](https://github.com/tosa-no-onchan/opp_with_transformer_cpp/blob/main/images/1-l.jpg)  

Run cource_classgen to make cource class file n-l.yaml.  

``````
image: 1.jpg
cols: 178
rows: 122
x_interval: 4
data_num: 44
class_num: 49
data: 24 24 24 24 24 24 24 23 22 21 21 21 22 23 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 23 22 21 21 21 22
``````
Then, You can run opp_with_lstm training or opp_with_transformer_mltu trainning.  

#### 1. how to build  
$ git clone https://github.com/tosa-no-onchan/opp_with_transformer_cpp.git  

cd opp_with_transformer_cpp  
edit cource_classgen.cpp  

    std::string data_path="????/image";  
    genClassText.cont_f_ =false;

$ mkdir build  
$ cd build  
$ cmake ..  
$ make  

#### 2. how to run.  
$ ./cource_classgen  

#### 3. test data of mine  

[test data](https://huggingface.co/datasets/tosa-no-onchan/opp)  
How to get  
``````
$ source kivy_env-3.12/bin/activate
(kivy_env-3.12) $ python -m pip install "huggingface_hub[cli]"
(kivy_env-3.12) $ huggingface-cli download tosa-no-onchan/opp --repo-type dataset
Fetching 3 files:   0%|                                   | 0/3 [00:00<?, ?it/s]Downloading '.gitattributes' to '/home/nishi/.cache/huggingface/hub/datasets--tosa-no-onchan--opp/blobs/55cab133643a2a73e083373d2106533678d0edd5.incomplete'
.gitattributes: 100%|██████████████████████| 2.42k/2.42k [00:00<00:00, 7.98MB/s]
Download complete. Moving file to /home/nishi/.cache/huggingface/hub/datasets--tosa-no-onchan--opp/blobs/55cab133643a2a73e083373d2106533678d0edd5
Fetching 3 files: 100%|███████████████████████████| 3/3 [00:00<00:00,  5.05it/s]
/home/nishi/.cache/huggingface/hub/datasets--tosa-no-onchan--opp/snapshots/d42d642453a5c8f218a3503e5d117f9f12a52711
(kivy_env-3.12) nishi@ns-ub1x:~$ deactivate
check and find zip file ~/.cache/huggingface/hub/datasets--tosa-no-onchan--opp/blobs
~/.cache/huggingface/hub/datasets--tosa-no-onchan--opp/blobs/16ae24a1abf0a96891568121152952588f59b147627af8ac22fee427b5848f78   --> zip file
unzip 16ae24a1abf0a96891568121152952588f59b147627af8ac22fee427b5848f78
you can find image/1.jpg 1.xcf 1-l.jpg 1-l.yaml ....
``````
