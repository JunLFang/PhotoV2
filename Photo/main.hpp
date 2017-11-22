/*
 * SortAlgorithm V1
 *  Created on: Dec 16, 2017
 *      Author: prathamesh
 */

#include "PhotoSortV2.hpp"
void initTemplate(vector<Photo>& temp) {
       	temp.push_back(Photo(cv::Size(200, 100),1,1));
//         	temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1)); 
// 			temp.push_back(Photo(cv::Size(200, 100), 2, 1));
			temp.push_back(Photo(cv::Size(230, 200), 3, 1));
			temp.push_back(Photo(cv::Size(200, 100), 1, 1));
			temp.push_back(Photo(cv::Size(100, 100), 2, 1));
			temp.push_back(Photo(cv::Size(300, 200), 3, 1));
// 			temp.push_back(Photo(cv::Size(100, 100), 1, 1));
// 			temp.push_back(Photo(cv::Size(100, 80), 2, 1));
// 			temp.push_back(Photo(cv::Size(100, 60), 3, 1));
        //	temp.push_back(Photo(cv::Size(10, 6), 1,1));
       
       }
              

int main(int argc, char* argv[])
{
	//rotate();

	Rect background_rect = Rect(0, 0, 1920,1080);
	vector<Photo> temp;
	initTemplate(temp);
	PhotoInterface sortphoto;
	string jsonstring;
	Inputs inputs;
	inputs.background_ = background_rect;
	inputs.photos_ = temp;


	//inputs=sortphoto.ReadJson(jsonstring);
	jsonstring=sortphoto.PhotoSortAuto(inputs.background_, inputs.photos_);


// 	for (int gap = 0; ;gap++)
// 	{
// 		
// 	jsonstring=sortphoto.PhotoSortSquare(inputs.background_, inputs.photos_,gap);
// 	waitKey();
// 	}




	//waitKey();
	return 0;
}
