#pragma once
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <iostream>

#define M 3
#define PI 3.14159265
#define deg2rad(x) x/180*3.14159
#define POSE_OTHERS 0
#define POSE_CLOSE 1
#define POSE_POINTING 2
#define POSE_FINGER_GUN 3
#define POSE_OPEN 4

using namespace std;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}


struct Finger {
	cv::Point2f ctr;
	cv::Size2f size;
	cv::Size2f ref_size;
	float p, t;	// phi, theta
				// t denotes the degree for the bending (z-axis). t = [0, 90]

	cv::Point2i proj_pt[4];
};
struct Palm {
	cv::Point2f ctr;
	cv::Size2f size;
	cv::Size2f ref_size;
	float p;

	cv::Point2i proj_pt[4];
};
class Hand {
public:
	Hand();
	Hand(float init_x, float init_y);
	Hand(float init_x, float init_y, float prev_ctrx, float prev_ctry, float prev_angle);
	void init_params(float init_x, float init_y);
	void scale_hand(float new_scale);
	void rotate_hand(float pp, float f0p, float f1p, float f2p, float f3p, float f4p);
	void rotate_hand(float *p_set);
	void bend_finger(float f0t, float f1t, float f2t, float f3t, float f4t);
	void bend_finger(float *t_set);
	void move_hand(float x, float y);
	void position_update();

	void draw_hand(char *title, cv::Mat &img);

	float compute_score(cv::Mat &img, int n, int opt);
	void compute_m_score(cv::Mat &img, int n);
	void compute_m_score2(cv::Mat &img);
	void compute_v_score(cv::Mat &img, int n);
	void compute_v_score2(cv::Mat &img);
	void compute_i_score();
	void update_total_score();
	void update_total_score2();
	void export_variables(float *x);

	int classify_pose();

	float compute_zoom_angle();
	void export_tip_pos(cv::Point2f &x_and_y, float IM_SCALE);
	
	// auxiliary functions
	void rotate_pt(cv::Point2f ref_p, cv::Point2f &trg_p, float deg);
	void rectangle2(cv::Mat &img, cv::Point2f ctr, cv::Size2f size, cv::Scalar &color);
	void rectangle3(cv::Mat &img, cv::Point2f ctr, cv::Size2f size, float theta, cv::Scalar &color, int thickness = 1);
	void rectangle_from_4pt(cv::Mat &img, cv::Point2i *proj_pt, cv::Scalar &color, int thickness = 1);
private:
	Finger f[5];
	Palm p;
	cv::Point2f j[5];	// joint point
	float scale;

	float m_score_f[5], m_score_p, M_score;	// match_score
	float v_score_f[5], v_collision[5], V_score;	// visual_score
	float I_score;	// inertia_score
	float T_score;	// total_score

	float prev_state[14];

};
class Gesture {
public:
	Gesture();
	int gesture_main(const cv::Mat im_origin,
		bool &bOverview, bool &bZoom, bool &bRelocal,
		float &fZoomMag, float &fDeltaX, float &fDeltaY, bool draw = 1);

	// main functions
	void find_best_hand_model();	// extract and analysis contour (In = imgae, Out = scores, best_idx)
	void analysis_hand_pose();	// anlaysis hand pose (In = best hand model, Out = filtered_pose)
	void generate_output_signal();
	void apply_kalman_filter();
	void draw_filtered_gesture();


	// auxiliary functions
	int extract_contours(const cv::Mat im_origin, vector<cv::Mat> &contours_filled, double im_scale, float p = 1.5);
	void check_skin_color(cv::Mat &im_gray, const cv::Mat im_color, float p);
	void check_skin_color_rgb(cv::Mat &im_gray, const cv::Mat im_color);
	void get_ctr_pt(const cv::Mat im_bw, cv::Point2i &ctr, double &max_dist);
	float analysis_contours(cv::Mat & contour, Hand &h0);
	int estimate_pose(vector<int> &pose_queue, int curr_pose);
	void balance_white(cv::Mat &im);
private:
	float IM_SCALE;
	cv::Mat im_origin, im_show;

	// find_best_hand_model()
	Hand best_hand;
	bool is_there_hand;

	// analysis_hand_pose()
	vector<int> pose_queue;
	int filtered_pose;

	// generate_output_signal()
	int iOpenCnt, iCloseCnt, iZoomCnt, iRelocalCnt;
	bool bOverview, bZoom, bRelocal;
	float fAngle1, fAngle2, fZoomMag;
	float fDeltaX, fDeltaY;
	cv::Point2f p2fTipRef;	// after demo

	// --- display string
	char pose_type_str[30];
	char gesture_overview_str[30];
	char gesture_str[30];

	// Kalman filter
	cv::KalmanFilter kf;
	float x_kf[14];

	// etc
	char title_gesture[20];
	bool bDraw;


	// afterDemo
	cv::Point2f p2fTip;
	cv::Rect rSearchArea;
	cv::Mat mSearchPatch, mTipPatch;
};