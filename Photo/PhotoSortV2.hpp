/*
 * common.hpp
 *
 *  Created on: Oct 25, 2011
 *      Author: prathamesh
 */

#ifndef COMMON_HPP_
#define COMMON_HPP_

//OpenCV Libraries
#include <opencv2/opencv.hpp>

//IO Libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


#include "json/json.h"

//Math Library
#include <math.h>
//Constant MACROS
#define Random(x) (rand() % x) 

static int cellsize_ = 0;
static int minboundingcell_ = 0;
static int minbounding_ = 0;

enum SortType
{
	SortType_Different = 0,
	SortType_Next_Same = -1,
	SortType_Same = 1

};
using namespace cv;
using namespace std;


class Photo {
public:
	Photo() {};
	Photo(cv::Size photo_size,int mark,int count):photo_size_(photo_size){

		count_ = count;
		mark_ = mark;
	};

	cv::Size photo_size_;

	int mark_;
	int count_;

};

struct Inputs
{
	vector<Photo> photos_;
	Rect background_;
};


class PhotoInterface {

public:
	PhotoInterface() {};
	string PhotoSortAuto(Rect &input_rect, vector<Photo> &input_photo);
	string PhotoSortSquare(Rect &input_rect, vector<Photo> &input_photo,int gap);
	string Encode(vector <Rect> &result,vector <Photo> temp);
	Rect Reshap_background(Rect &input_rect, vector<Photo> &input_photo,double magnificate);
	string Getstring(const int n);
	Inputs ReadJson(string & input_json);
};

class PossibilitySort {
public:
	PossibilitySort() {};
	PossibilitySort(int cellsize,int  minbounding);
	PossibilitySort(int level_index, Rect photo_rect,int cellsize, int  minbounding);
	int Add(int& traget_level_index, vector<Rect> target_photo_rects, int target_count);
	int Add_(int& traget_level_index, vector<Rect> target_photo_rects, SortType  sort_type);
	int FormatResult(vector<Rect>& rct = vector<Rect>(), PossibilitySort* ptr = nullptr);;
	int Sort();

	int level_index_;
	Rect photo_rect_;
	vector<PossibilitySort> sub_sorts_;

	int most_flag = 0;
	//to do  

	multimap <double,vector<Rect>> results_;

	int cellsize_;
	int minbounding_;
};
class PhotoSort {

public:
	PhotoSort() {};
	PhotoSort(int cellsize, int minboundingcell) : subsort(cellsize, minboundingcell) {
		cellsize_ = cellsize;
		minboundingcell_ = minboundingcell;
		//minbounding_ = cellsize+minboundingcell;
		minbounding_ = minboundingcell;
	};
	//Final Orientation and Position of Template 1
	//int cellsize_ = 3;
	//int minboundingcell_ = 3;
	//int minbounding_ = 6;
	//Runtime current coordinates
	cv::Point  current_position;
	int levelcount = 1;
	vector<Point> Size2Angle(cv::Size photo_size);


	//int SortAutoInterface(Mat backgound, vector<Photo> input_photos, multimap <double, vector<Rect>> &results_out_test);
	int SortAuto(vector<Photo>& temp,vector<Point> lcontours);
	int SortSquares(Rect backgroud, vector<Photo> input_photos, vector <Rect> &out_rects,int gap);
	multimap <double, vector<Rect>> results_;
	PossibilitySort subsort;
	Mat input_background;
	//PhotoSort* aaa;

private:
	vector <Point> rects_center_points(Rect border, Size each_photo ,int gap);

	//string getstring(const int n);
};



std::vector<cv::Point> PhotoSort::Size2Angle(cv::Size photo_size) {
	vector<Point> angle;
	angle.resize(4);
	angle[0].x = -minbounding_;
	angle[0].y = -minbounding_;
	angle[1].x = photo_size.width + minbounding_;
	angle[1].y = -minbounding_;
	angle[2].x = photo_size.width + minbounding_;
	angle[2].y = photo_size.height + minbounding_;
	angle[3].x = -minbounding_;
	angle[3].y = photo_size.height + minbounding_;
	return angle;
}

int PhotoSort::SortAuto(vector<Photo>& temp, vector<Point>  border_contours) {

	     
	//Number of holes in leaf

	bool inside_contours_flg = true;

	/* -----------------------------
	* Check for Template 1
	* -----------------------------
	*/
	Rect rect_bounding = boundingRect(border_contours);           //找到点集外边框
	rectangle(input_background, rect_bounding, Scalar(100, 255, 100), 1, 8);
	
	imshow("input", input_background);
	//cv::resize(input_background, input_background, input_background.size() * 3, 0, 0, 0);
	for (int photo_index = 0; photo_index < temp.size(); photo_index++) {
		std::vector<Rect> rects;
      		for (int x_index = rect_bounding.x / cellsize_; x_index < (int)(rect_bounding.br().x / cellsize_ ); x_index++) {                                   //应该是要加上x的初始值的?
			for (int y_index = rect_bounding.y / cellsize_; y_index < (int)(rect_bounding.br().y / cellsize_); y_index++) {
				current_position.x = cellsize_*x_index;
				current_position.y = cellsize_*y_index;

				//Variables used in switch below
				inside_contours_flg = true;
				for (int angle_index = 0; angle_index < 4; angle_index++) {
					double distance = pointPolygonTest(border_contours, current_position + Size2Angle(temp[photo_index].photo_size_)[angle_index], true);
					if (distance <= 0) {                                               //增加了=情况
						inside_contours_flg = false;
						break;
					}
				}
				if (inside_contours_flg) {
					//Final and filtered set of useful combinations
					Rect photo_rect(current_position, temp[photo_index].photo_size_); 
					rects.push_back(photo_rect);					
				}

			}
		}
	for (auto rect : rects)
				
				rectangle(input_background, rect, Scalar(100, 255, 100), 1, 8);

			//cv::resize(input_background, input_background, input_background.size() * 10, 0, 0, 0);
			
			cv::imshow("tempresult", input_background);
			//waitKey();
		if (subsort.Add(levelcount, rects, temp[photo_index].count_)<0) {
			return -1;
		}
	}
	int res = subsort.Sort();
	results_ = subsort.results_;
	return res;
}

PossibilitySort::PossibilitySort(int level_index, Rect photo_rect, int cellsize, int  minbounding) :level_index_(level_index),
photo_rect_(photo_rect),
cellsize_(cellsize),
minbounding_(minbounding) {

}

PossibilitySort::PossibilitySort(int cellsize, int  minbounding) :photo_rect_(0, 0, 0, 0),
cellsize_(cellsize),
minbounding_(minbounding) {
	level_index_ = 0;
}

int PossibilitySort::Add(int& traget_level_index, vector<Rect> target_photo_rects, int target_count)
{
	if (target_photo_rects.empty()) return -1;

	if (target_count > 1) {
		    int res = 0;
			res = Add_(traget_level_index, target_photo_rects, SortType_Next_Same);
		traget_level_index++;
		if (res < 0)	return res;
		for (int add_index = 1; add_index < target_count; ++add_index) {
			res = Add_(traget_level_index, target_photo_rects, SortType_Same);
			traget_level_index++;
			if (res<0) return res;
		}
		return 0;
	}
	else if (target_count == 1) {
		int res = Add_(traget_level_index, target_photo_rects, SortType_Different);
		traget_level_index++;
		return res;
	}
}
int PossibilitySort::Add_(int& traget_level_index, vector<Rect> target_photo_rects, SortType  sort_type)
{
	vector<Rect> target_photo_rects_last;
	if (traget_level_index <= level_index_) { return -1; }
	bool find_result_flg = false;
	for (auto target_photo_rect : target_photo_rects)
	{
		auto temp = (Rect(photo_rect_.tl() - Point(minbounding_, minbounding_), photo_rect_.br() + Point(minbounding_, minbounding_))&Rect(target_photo_rect.tl() - Point(minbounding_, minbounding_), target_photo_rect.br() + Point(minbounding_, minbounding_)));
 		if (temp.area() == 0) {

			target_photo_rects_last.push_back(target_photo_rect);
			if (traget_level_index == level_index_ + 1) {                    //判断是否为下一级
				if (sort_type == SortType_Same) {
					if (((photo_rect_.x > target_photo_rect.x) || ((photo_rect_.x ==  target_photo_rect.x) &&(photo_rect_.y > target_photo_rect.y)))) 	continue;
					find_result_flg = true;
				}
				else	find_result_flg = true;


       				sub_sorts_.push_back(PossibilitySort(traget_level_index, target_photo_rect, cellsize_, minbounding_));
					if ( sub_sorts_.size()>3)            //most number is 50
						break;
			}
			else find_result_flg = true;
	    }
	}

	if (find_result_flg)
	{
		if (traget_level_index != level_index_ + 1) {
			if (!sub_sorts_.empty()) {
				for (auto itr_subsort = sub_sorts_.begin(); itr_subsort != sub_sorts_.end(); ) {
					if (itr_subsort->Add_(traget_level_index, target_photo_rects_last, sort_type) < 0) {
						itr_subsort = sub_sorts_.erase(itr_subsort);
					}
					else  ++itr_subsort;
				}
				if (sub_sorts_.empty()) 	return  -1;
			}
		}
		else 	return 0;
	}
	else return -1;
}

int PossibilitySort::FormatResult(vector<Rect>& rct /*= vector<Rect>()*/, PossibilitySort* ptr /*= nullptr*/)
{
	//most large number is 50
	
	if (ptr == nullptr)	ptr = this;
	rct.push_back(photo_rect_);
	if (sub_sorts_.empty()) {
		vector<Point> points;
		for (auto rect : rct) {
			if (rect.area() == 0) continue;
			points.push_back(rect.br());
			points.push_back(Point(rect.x + rect.width, rect.y));
			points.push_back(rect.tl());
			points.push_back(Point(rect.x, rect.y + rect.height));
		}

		auto rect_bounding = boundingRect(Mat(points));

		if (points.empty()) return -1;
		double area = rect_bounding.area();

		double l = 2 * (rect_bounding.width + rect_bounding.height);
		ptr->results_.insert(make_pair(l, rct));
		
	}
	else {

		for (auto subsort : sub_sorts_) 	subsort.FormatResult(rct, ptr);
	}
	rct.pop_back();
	return ptr->results_.size();
}

int PossibilitySort::Sort() {
	int res = FormatResult();
	if (res < 0) return res;
	return res;
}


int PhotoSort::SortSquares(Rect range_border, vector<Photo> input_photos, vector <Rect> &out_rects,int gap)
{
	if (input_photos.size() >9 || input_photos.size()==0)
	{
		return -1;
	}
	int max_height = 0;
	int max_width = 0;
	for (auto input_photo : input_photos)
	{
		int max_temp_height = input_photo.photo_size_.height;
		int max_temp_width = input_photo.photo_size_.width;
		if (max_height <max_temp_height)
		{
			max_height = max_temp_height;
		}
		if (max_width < max_temp_width)
		{
			max_width = max_temp_width;
		}

	}

	if (2*gap + 3 * max_width >= range_border.width || 2*gap + 3 * max_height >= range_border.height)
	{
		return -2;
	}
	//double max_threshould;
	vector <Point> temp_center_points;
	vector <Rect> return_center_rects;
	//max_threshould = max(range_border.width / 3.0, range_border.height / 3.0);
	//以上为判断
	temp_center_points = rects_center_points(range_border, Size(max_width,max_height),gap);
	int picture_index = 0;
	for (auto input_photo : input_photos)
	{
		for (int count=0;count<input_photo.count_;count++)
		{		
			Point leftup_point = Point(temp_center_points[picture_index].x-input_photo.photo_size_.width/2, temp_center_points[picture_index].y - input_photo.photo_size_.height / 2);
			return_center_rects.push_back(Rect(leftup_point, input_photo.photo_size_));
			picture_index++;
		}
	}

	out_rects = return_center_rects;
	return 0;

}


vector <Point> PhotoSort::rects_center_points(Rect border,Size max_size,int gap)
{
	Point border_center = Point(border.width / 2+ border.x, border.height / 2+border.y);

	 vector <Point> temp_center_points;
	 for (int index_col = -1; index_col < 2; index_col++)
		 for (int index_row = -1; index_row < 2; index_row++)
		 {
			 Point center_point = Point(border_center.x+(index_col)*(max_size.width+gap), border_center.y+ (index_row)*(max_size.height + gap));
			 temp_center_points.push_back(center_point);
		 }
			
	 return temp_center_points;
}




// int PhotoSort::SortAutoInterface(Mat backgound, vector<Photo> input_photos, multimap <double, vector<Rect>> & results_out_test)
// {
// 	input_background= backgound.clone();
//     
// 	/*auto src = imread("clean.png", IMREAD_COLOR);
// 	imshow(" ",src);
// 	cv::resize(src, src, src.size() / 20);*/
// 	string data;
// 	vector<vector<Point> > lcontours;
// 	vector<Vec4i> lhierarchy;
// 	Mat mat_grey;
// 	cvtColor(backgound, mat_grey, CV_BGR2GRAY);
// 	findContours(mat_grey, lcontours, lhierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0));
// 	//vector<Photo> temp;
// 	//initTemplate(temp);
// 
// //	int bordercell = backgound.width / 3;
// 	while (true)
// 	{
// 		int bordercell = --minbounding_;
// 		//PhotoSort instanse(backgound.width / 6, --bordercell);
// 
// 		//instanse.input_background = src.clone();
// 
// 		int res = SortAuto(input_photos, lcontours[0]);
// 
// 
// 		//if ((res>0)|| bordercell == 1)
// 		if (bordercell == 1)
// 			break;
// 	}
// 
// 	results_out_test = results_;
// 	return 0;

//}


string PhotoInterface::Encode(vector <Rect> &result,vector <Photo> temp)
{

	string result_s{};
	
	Json::Value root;
	Json::Value photo_obj;
	for (int number_index = 0; number_index < result.size(); number_index++)
	{
		
		int mark = 0;
		vector <Photo>::iterator each_photo_erase = temp.end();
		for (vector <Photo>::iterator each_photo = temp.begin(); each_photo != temp.end(); ++each_photo) {
			if (each_photo->photo_size_ == result[number_index].size())
			{
				mark = each_photo->mark_;
				each_photo_erase = each_photo;
				Json::Value photo;
				photo["mark"] = mark;
				photo["X"] = result[number_index].x;
				photo["Y"] = result[number_index].y;

				photo_obj.append(photo);
				break;
			}
		}
		if (each_photo_erase != temp.end())
		{
			temp.erase(each_photo_erase);
		}	

	}
	root["photo"] = photo_obj;
	result_s = root.toStyledString();
	fstream file;
	file.open("D://Program//VS code//Algrithem//PhotoV2//Photo//file.txt");
	file << result_s;
	file.close();
	return result_s;

}
string PhotoInterface::Getstring(const int n)

{
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

string PhotoInterface::PhotoSortAuto(Rect &input_rect, vector<Photo> &input_photo)
{
	Mat src = Mat::zeros(input_rect.size(), CV_8UC3);             //set a circle masking
	input_rect=Reshap_background(input_rect, input_photo,2.0);
	cv::rectangle(src,
		input_rect,                                  //挂画区域
		cv::Scalar(255, 255, 255),
		-1
	);
	imshow(" ", src);

	//PhotoSort instance(background_rect.width/6, background_rect.width/3);
	string data;
	vector<vector<Point> > lcontours;
	vector<Vec4i> lhierarchy;
	Mat mat_grey;
	cvtColor(src, mat_grey, CV_BGR2GRAY);
	findContours(mat_grey, lcontours, lhierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0));
	vector<vector<Rect>> results;
	int bordercell = input_rect.width / 30;
	while (true)
	{
		PhotoSort instanse(input_rect.width / 30, 1);
		instanse.input_background = src.clone();
		int res = instanse.SortAuto(input_photo, lcontours[0]);
		int number_flag = 0;
		for (auto result : instanse.results_)  //遍历所有结果
		{
			number_flag++;
			results.push_back(result.second);
			auto a = src.clone();
			std::cout << "area = " << result.first << endl;
			for (auto rect : result.second)
				rectangle(a, rect, Scalar(100, 255, 100), 1, 8);
			//cv::resize(a, a, a.size() * 10, 0, 0, 0);
			cv::imshow("result", a);
			cout << "bordercell : " << bordercell << endl;
			waitKey();  
			if (number_flag > 100)
				break;

		}
	//	if (bordercell == 1)
		break;
	}
	vector<Rect> choosen = results[Random(results.size())];
	return Encode(choosen, input_photo);
}



string PhotoInterface::PhotoSortSquare(Rect &input_rect, vector<Photo> &input_photo,int gap)
{
	PhotoSort square;
	Mat src = Mat::zeros(input_rect.size(), CV_8UC3);             //set a circle masking
	//input_rect=Reshap_background(input_rect, input_photo,2.0);
	cv::rectangle(src,
		input_rect,                                  //挂画区域
		cv::Scalar(255, 255, 255),
		-1
	);
	Mat back = src.clone();
	vector <Rect> out_rects;
	int check_size = square.SortSquares(input_rect, input_photo, out_rects,gap);
	if (-1 == check_size)
	{
		cout << "the picture is too large";
	}
	if (-2 == check_size)
	{
		cout << "the gap is too large";
	}
	for (auto out_rect : out_rects)
	{
		for (auto out_rect : out_rects)
			rectangle(back, out_rect, Scalar(100, 255, 100), 1, 8);
	}
	cv::resize(back, back, back.size(), 0, 0, 0);
	imshow("九宫格", back);
	return Encode(out_rects, input_photo);
}

Inputs PhotoInterface::ReadJson(string & input_json)
{
	
	
	vector<Photo> input_photo;
	Rect background;
	Json::Reader reader;
    Json::Value value;
	if (reader.parse(input_json, value))
	{
		Json::Value photo_obj = value["photo"];
		Json::Value background_obj = value["background"];
		int photo_num = photo_obj.size();
		for (int index = 0; index < photo_num; index++)
		{
			Photo temp;
			temp.mark_ = photo_obj[index]["mark"].asInt();
			temp.photo_size_.width = photo_obj[index]["width"].asInt();
			temp.photo_size_.height = photo_obj[index]["height"].asInt();
			input_photo.push_back(temp);
		}

		background = Rect(0, 0, background_obj["height"].asInt(), background_obj["width"].asInt());

	}
	Inputs inputs_read;
	inputs_read.background_ = background;
	inputs_read.photos_ = input_photo;
	return inputs_read;
}



Rect PhotoInterface::Reshap_background(Rect &input_rect, vector<Photo> &input_photo,double magnificate)
{

	double all_area = 0.0;
	//calculate all the photo square
	for (size_t num = 0; num < input_photo.size(); num++)
	{
		all_area += input_photo[num].photo_size_.area();
	}

	double infactor = (double)input_rect.width / input_rect.height;
	//double magnificate = 2.0;
	int height = sqrt(magnificate*all_area / infactor);
	int width = height*infactor;

	if (height<input_rect.height || width<input_rect.width)
	{
		Point center = Point(input_rect.width / 2, input_rect.height / 2);
		input_rect.x = center.x - width / 2;
		input_rect.y = center.y - height / 2;
		input_rect.height = height;
		input_rect.width = width;
	}


	return input_rect;
}



#endif /* COMMON_HPP_ */
