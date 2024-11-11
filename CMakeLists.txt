cmake_minimum_required(VERSION 3.1)
project( opencv_contours )
find_package( OpenCV REQUIRED )

include_directories(/home/nishi/Documents/VisualStudio-CPP/opp_with_transformer_cpp)

link_directories(/home/nishi/Documents/VisualStudio-CPP/opp_with_transformer_cpp/build)


#add_executable( main main.cpp )
add_executable( cource_class_gen cource_class_gen.cpp )


#target_link_libraries( main ${OpenCV_LIBS} )
target_link_libraries( cource_class_gen ${OpenCV_LIBS} )

