#ifndef _HAND_MODEL_H_
#define _HAND_MODEL_H_

#include "basic_func.h"
#define POSE_OTHERS 0
#define POSE_CLOSE 1
#define POSE_POINTING 2
#define POSE_FINGER_GUN 3
#define POSE_OPEN 4

using namespace std;
using namespace cv;

struct Finger {
	Point2f ctr;
	Size2f size;
	float p, t;	// phi, theta
	// t denotes the degree for the bending (z-axis). t = [0, 90]

	Point2i proj_pt[4];
};
struct Palm {
	Size2f size;
	Point2f ctr;
	float p;

	Point2i proj_pt[4];
};

class Hand {
public:
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

	void draw_hand(char *title, Mat &img);
	
	float compute_score(Mat &img, int n, int opt);
		void compute_m_score(Mat &img, int n);
		void compute_m_score2(Mat &img);
		void compute_v_score(Mat &img, int n);
		void compute_v_score2(Mat &img);
		void compute_i_score();
		void update_total_score();
		void update_total_score2();
	void export_variables(float *x);

	int classify_pose();

	float compute_zoom_angle();
	void export_tip_pos(float &x, float &y);
	//int iOpenCnt, iCloseCnt, iZoomCnt;
private:
	Finger f[5];
	Palm p;
	Point2f j[5];	// joint point
	float scale;

	float m_score_f[5], m_score_p, M_score;	// match_score
	float v_score_f[5], v_collision[5], V_score;	// visual_score
	float I_score;	// inertia_score
	float T_score;	// total_score

	float prev_state[14];

};

void Hand::export_variables(float *x) {
	x[0] = scale;
	x[1] = p.ctr.x;
	x[2] = p.ctr.y;
	x[3] = p.p;
	x[4] = f[0].p;
	x[5] = f[1].p;
	x[6] = f[2].p;
	x[7] = f[3].p;
	x[8] = f[4].p;
	x[9] = f[0].t;
	x[10] = f[1].t;
	x[11] = f[2].t;
	x[12] = f[3].t;
	x[13] = f[4].t;
}
Hand::Hand(float init_x, float init_y) {
	init_params(init_x, init_y);
	position_update();
}
Hand::Hand(float init_x, float init_y, float prev_ctrx, float prev_ctry, float prev_angle) {
	init_params(init_x, init_y);
	position_update();
	
	prev_state[1] = prev_ctrx;
	prev_state[2] = prev_ctry;
	prev_state[3] = prev_angle;
}
void Hand::init_params(float init_x, float init_y) {
	scale = 1;

	p.ctr = Point2f(init_x, init_y);	p.size = Size2f(85, 80);	p.p = 0;
	
	f[0].size = Size2f(18, 70);
	f[1].size = Size2f(14, 87);
	f[2].size = Size2f(14, 98);
	f[3].size = Size2f(14, 82);
	f[4].size = Size2f(14, 60);
	//f[0].size = Size2f(25, 70);
	//f[1].size = Size2f(20, 87);
	//f[2].size = Size2f(20, 98);
	//f[3].size = Size2f(20, 82);
	//f[4].size = Size2f(19, 60);

	f[0].p = p.p+120;	f[0].t = 45;
	f[1].p = p.p+90;	f[1].t = 45;
	f[2].p = p.p+90;	f[2].t = 45;
	f[3].p = p.p+90;	f[3].t = 45;
	f[4].p = p.p+90;	f[4].t = 45;

	//iOpenCnt = 0;
	//iCloseCnt = 0;
	//iZoomCnt = 0;
}
void Hand::scale_hand(float new_scale) {
	scale = new_scale;

	p.size = p.size * new_scale;
	for(int i=0; i<5; i++) {
		f[i].size = f[i].size * new_scale;
	}
	position_update();
}
void Hand::move_hand(float x, float y) {
	p.ctr.x = x;
	p.ctr.y = y;

	position_update();
}
void Hand::rotate_hand(float pp, float f0p, float f1p, float f2p, float f3p, float f4p) {
	if (pp>=0) {
		p.p = pp;
		f[0].p = min(max(f[0].p, 100.f), 170.f);
		f[1].p = min(max(f[1].p, 60.f), 120.f);
		f[2].p = min(max(f[2].p, 70.f), 110.f);
		f[3].p = min(max(f[3].p, 70.f), 110.f);
		f[4].p = min(max(f[4].p, 60.f), 110.f);
	}

	if (f0p>=0)	f[0].p = min(max(f0p, 100.f), 170.f);
	if (f1p>=0)	f[1].p = min(max(f1p, 60.f), 120.f);
	if (f2p>=0)	f[2].p = min(max(f2p, 70.f), 110.f);
	if (f3p>=0)	f[3].p = min(max(f3p, 70.f), 110.f);
	if (f4p>=0)	f[4].p = min(max(f4p, 60.f), 110.f);

	position_update();
}
void Hand::rotate_hand(float *p_set) {
	if (p_set[0]>=0) {	// pp
		p.p = p_set[0];
		f[0].p = min(max(f[0].p, 100.f), 170.f);
		f[1].p = min(max(f[1].p, 60.f), 120.f);
		f[2].p = min(max(f[2].p, 70.f), 110.f);
		f[3].p = min(max(f[3].p, 70.f), 110.f);
		f[4].p = min(max(f[4].p, 60.f), 110.f);
	}

	if (p_set[1]>=0) f[0].p = min(max(p_set[1], 100.f), 170.f);
	if (p_set[2]>=0) f[1].p = min(max(p_set[2], 60.f), 120.f);
	if (p_set[3]>=0) f[2].p = min(max(p_set[3], 70.f), 110.f);
	if (p_set[4]>=0) f[3].p = min(max(p_set[4], 70.f), 110.f);
	if (p_set[5]>=0) f[4].p = min(max(p_set[5], 60.f), 110.f);

	position_update();
}
void Hand::bend_finger(float f0t, float f1t, float f2t, float f3t, float f4t) {
	if (f0t>=0) {	f[0].t = min(max(f0t, 0.f), 90.f);	}	// range = [0, 90]
	if (f1t>=0) {	f[1].t = min(max(f1t, 0.f), 90.f);	}
	if (f2t>=0) {	f[2].t = min(max(f2t, 0.f), 90.f);	}
	if (f3t>=0) {	f[3].t = min(max(f3t, 0.f), 90.f);	}
	if (f4t>=0) {	f[4].t = min(max(f4t, 0.f), 90.f);	}

	position_update();
}
void Hand::bend_finger(float *t_set) {
	if (t_set[0]>=0) {	f[0].t = min(max(t_set[0], 0.f), 90.f);	}	// range = [0, 90]
	if (t_set[1]>=0) {	f[1].t = min(max(t_set[1], 0.f), 90.f);	}
	if (t_set[2]>=0) {	f[2].t = min(max(t_set[2], 0.f), 90.f);	}
	if (t_set[3]>=0) {	f[3].t = min(max(t_set[3], 0.f), 90.f);	}
	if (t_set[4]>=0) {	f[4].t = min(max(t_set[4], 0.f), 90.f);	}

	position_update();
}
void Hand::position_update() {
	for(int i=0; i<5; i++) {
		if (i==0)	// refine joint0 (¾öÁö)
			j[i] = p.ctr - Point2f(p.size.width/2+p.size.width/8, 0) + Point2f(0, p.size.height/8);
		else if (i==4) // joint4
			//j[i] = p.ctr - Point2f(p.size.width/2, p.size.height/2) + Point2f(p.size.width/8*(i*2-1), p.size.height/10);
			j[i] = p.ctr - Point2f(p.size.width/2, p.size.height/2) + Point2f(p.size.width, p.size.height/10);
		else if (i==3) // joint 3
			j[i] = p.ctr - Point2f(p.size.width/2, p.size.height/2) + Point2f(p.size.width/8*(6), 0);
		else if (i==2) // joint 2
			j[i] = p.ctr - Point2f(p.size.width/2, p.size.height/2) + Point2f(p.size.width/8*(4), 0);
		else
			j[i] = p.ctr - Point2f(p.size.width/2, p.size.height/2) + Point2f(p.size.width/8*(i*2-1), 0);

		rotate_pt(p.ctr, j[i], p.p);

		
		Point2f tmp_pt;
		float h;
		float half_w, half_h;
		float rot_deg;

		// compute proj_pt (for palm)
		rot_deg = p.p;
		half_w = p.size.width/2;
		half_h = p.size.height/2;

		tmp_pt = Point2f(p.ctr.x - half_w, p.ctr.y - half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[0] = Point2i(tmp_pt);	// UL
		tmp_pt = Point2f(p.ctr.x + half_w, p.ctr.y - half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[1] = Point2i(tmp_pt);	// UR
		tmp_pt = Point2f(p.ctr.x + half_w, p.ctr.y + half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[2] = Point2i(tmp_pt);	// LR
		tmp_pt = Point2f(p.ctr.x - half_w, p.ctr.y + half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[3] = Point2i(tmp_pt);	// LL
		
		// compute ctr      (Add 90: sin->cos, cos->-sin)
		h = f[i].size.height*cosd(f[i].t);
		f[i].ctr = j[i] - Point2f(-h/2*cosd(f[i].p+p.p), h/2*sind(f[i].p+p.p));

		// compute proj_pt (for finger)
		rot_deg = f[i].p-90+p.p;
		half_w = f[i].size.width/2;
		half_h = f[i].size.height/2*cosd(f[i].t);

		tmp_pt = Point2f(f[i].ctr.x - half_w, f[i].ctr.y - half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[0] = Point2i(tmp_pt);	// UL
		tmp_pt = Point2f(f[i].ctr.x + half_w, f[i].ctr.y - half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[1] = Point2i(tmp_pt);	// UR
		tmp_pt = Point2f(f[i].ctr.x + half_w, f[i].ctr.y + half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[2] = Point2i(tmp_pt);	// LR
		tmp_pt = Point2f(f[i].ctr.x - half_w, f[i].ctr.y + half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[3] = Point2i(tmp_pt);	// LL

	}
}
void Hand::draw_hand(char *title, Mat &img) {
	Scalar c_red(0, 0, 128);
	Scalar c_blue(128, 0, 0);
	Scalar c_gray(128, 128, 128);
	Scalar c_palm(14, 201, 255);
	Scalar c_finger(29, 230, 181);
	Scalar c_joint(201, 174, 255);

	rectangle_from_4pt(img, p.proj_pt, c_palm, 2);
	circle(img, p.ctr, 3, c_palm);

	for(int i=0; i<5; i++) {
		rectangle_from_4pt(img, f[i].proj_pt, c_finger, 2);
		
		circle(img, f[i].ctr, 3, c_finger);
		circle(img, j[i], 3, c_joint);

		//line(img, f[i].ctr, j[i], c_gray);
	}
	if (title != NULL)
		imshow(title, img);
}
void Hand::compute_m_score(Mat &img, int n) {
// n = 0~4: each finger
// n = 5 : palm
	// new version for faster speed
	Point2f uni_pt[4];
	uni_pt[0] = f[n].proj_pt[0];
	uni_pt[1] = f[n].proj_pt[1];
	uni_pt[2] = f[n].proj_pt[2];
	uni_pt[3] = f[n].proj_pt[3];

	for(int i=0; i<4; i++)
		rotate_pt(f[n].ctr, uni_pt[i], (-1)*(f[n].p-90));
	
	// find the smallest square (UL~LR) and crop it
	Point2i UL, LR;
	Point2i cur_pt[4];
	
	if (n==5) {
		cur_pt[0] = p.proj_pt[0];	cur_pt[1] = p.proj_pt[1];	cur_pt[2] = p.proj_pt[2];	cur_pt[3] = p.proj_pt[3];
	} else {
		cur_pt[0] = f[n].proj_pt[0];	cur_pt[1] = f[n].proj_pt[1];	cur_pt[2] = f[n].proj_pt[2];	cur_pt[3] = f[n].proj_pt[3];
	}
	
	UL.x = min(cur_pt[0].x, min(cur_pt[1].x, min(cur_pt[2].x, cur_pt[3].x)));
	UL.y = min(cur_pt[0].y, min(cur_pt[1].y, min(cur_pt[2].y, cur_pt[3].y)));
	LR.x = max(cur_pt[0].x, max(cur_pt[1].x, max(cur_pt[2].x, cur_pt[3].x)));
	LR.y = max(cur_pt[0].y, max(cur_pt[1].y, max(cur_pt[2].y, cur_pt[3].y)));
	
	UL.x = max(UL.x, 0);
	UL.y = max(UL.y, 0);
	LR.x = min(LR.x, img.cols-1);
	LR.y = min(LR.y, img.rows-1);

	if (UL.x > img.cols-1 || UL.y > img.rows-1 || LR.x < 0 || LR.y < 0) {
		if (n==5)	m_score_p = 0;
		else		m_score_f[n] = 0;
		return;
	}////////////////////////////////////////////////

	// fill the patch
	Mat patch_ref, patch_trg;
	patch_ref = img(Rect(UL, LR));
	patch_trg = Mat::zeros(patch_ref.rows, patch_ref.cols, CV_8U);
	
	Point2i tmp_pt2i[4];
	for(int i=0; i<4; i++)
		tmp_pt2i[i] = cur_pt[i]-UL;
	fillConvexPoly(patch_trg, tmp_pt2i, 4, Scalar(1));

	// compute n_all and n_filled to calculate final_score.
	Scalar tmp_scalar;
	tmp_scalar = sum(patch_trg);
	int n_all = tmp_scalar.val[0];
	tmp_scalar = sum(patch_ref & patch_trg);
	int n_filled = tmp_scalar.val[0];

	// compute final_score and refine it.
	int knee_pt;	// slope of m_score graph is changed at knee_pt
	if (n==5)	knee_pt = 90;
	else		knee_pt = 60;

	//printf("Im-%d) %3d/%3d\t\n", n, n_filled, n_all);
	float final_score;
	if (n_all < 50)
		final_score = 100;
	else
		final_score = float(n_filled)/n_all*100;

	if(final_score<=knee_pt)
		final_score = (-1)*n_all;
	else
		final_score = (final_score-knee_pt)*100/(100-knee_pt);

	if (n==5)	m_score_p = final_score;
	else		m_score_f[n] = final_score;

	//cout<<final_score<<endl;
}
void Hand::compute_m_score2(Mat &img) {
	Scalar scalar_tp, scalar_fn, scalar_fp;
	float n_tp, n_fn, n_fp;
	Mat im_tp_fp, im_tp_fn, im_tp;

	int lower_bound = max(f[0].proj_pt[3].y,max(p.proj_pt[2].y,p.proj_pt[3].y));
	lower_bound = min(img.rows-1,lower_bound);

	//im_tp_fp = Mat::zeros(img.rows, img.cols, CV_8U);		// (2) comment this line for setting lower bound
	im_tp_fp = Mat::zeros(lower_bound+1, img.cols, CV_8U);	// (2) uncomment this line for setting lower bound
	//fillConvexPoly(im_tp_fp, p.proj_pt, 4, Scalar(255));	// (1) comment this line for excluding palm
	for(int n=0; n<5; n++)
		fillConvexPoly(im_tp_fp, f[n].proj_pt, 4, Scalar(255));
	//im_tp_fn = img.clone();								// (2) comment this line for setting lower bound
	im_tp_fn = img.rowRange(0, lower_bound+1).clone();		// (2) uncomment this line for setting lower bound
	fillConvexPoly(im_tp_fn, p.proj_pt, 4, Scalar(0));		// (1) uncomment this line for excluding palm
	im_tp = im_tp_fn & im_tp_fp;
	
	scalar_tp = sum(im_tp);
	scalar_fn = sum(im_tp_fn) - scalar_tp;
	scalar_fp = sum(im_tp_fp) - scalar_tp;

	n_tp = scalar_tp.val[0]/255.f;
	n_fn = scalar_fn.val[0]/255.f;
	n_fp = scalar_fp.val[0]/255.f;

	float precision = n_tp/(n_tp+n_fp);
	float recall = n_tp/(n_tp+n_fn);

	float fscore = 2 * precision*recall / (precision+recall);

	M_score = fscore*100;
}
void Hand::compute_v_score(Mat &img, int n) {
// n = 0~4: each finger
	v_score_f[n] = cosd(f[n].t);	// basic score

	// collision check
	v_collision[n] = 0;

	bool cond1, cond2, is_collision = false;
	Point2f tip, rot_tip;

	float h = f[n].size.height*cosd(f[n].t);
	tip = j[n] - Point2f(-h*cosd(f[n].p+p.p), h*sind(f[n].p+p.p));

		
	// collision check - left side finger
	for(int k=1; k<n; k++) {
		Point2f tmp_UR, tmp_LR;
		//rot_tip = tip;
		rot_tip = f[n].proj_pt[0];
		rotate_pt(j[k], rot_tip, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, rot_tip, (-1)*p.p);
		tmp_UR = f[k].proj_pt[1];
		rotate_pt(j[k], tmp_UR, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, tmp_UR, (-1)*p.p);
		tmp_LR = f[k].proj_pt[2];
		rotate_pt(j[k], tmp_LR, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, tmp_LR, (-1)*p.p);

		cond1 = (rot_tip.x < tmp_UR.x) && (rot_tip.y > tmp_UR.y);
		cond2 = (rot_tip.x < tmp_LR.x) && (rot_tip.y < tmp_LR.y);
		//cond1 = (rot_tip.x < tmp_UR.x);
		//cond2 = (rot_tip.x < tmp_LR.x);
		if ( cond1 && cond2 )
			is_collision = true;
	}
		
	// collision check - right side finger
	for(int k=n+1; k<5; k++) {
		Point2f tmp_UL, tmp_LL;
		//rot_tip = tip;
		rot_tip = f[n].proj_pt[1];
		rotate_pt(j[k], rot_tip, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, rot_tip, (-1)*p.p);
		tmp_UL = f[k].proj_pt[0];
		rotate_pt(j[k], tmp_UL, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, tmp_UL, (-1)*p.p);
		tmp_LL = f[k].proj_pt[3];
		rotate_pt(j[k], tmp_LL, (-1)*(f[k].p-90));
		rotate_pt(p.ctr, tmp_LL, (-1)*p.p);

		cond1 = (rot_tip.x > tmp_UL.x) && (rot_tip.y > tmp_UL.y);
		cond2 = (rot_tip.x > tmp_LL.x) && (rot_tip.y < tmp_LL.y);
		//cond1 = (rot_tip.x > tmp_UL.x);
		//cond2 = (rot_tip.x > tmp_LL.x);
		if ( cond1 && cond2 )
			is_collision = true;
	}
	if (is_collision)
		v_collision[n] = 10000;
	
}
void Hand::compute_v_score2(Mat &img) {
	
	// collision check
	for(int n=0; n<5; n++) {
		v_collision[n] = 0;

		bool cond1, cond2, is_collision = false;
		Point2f tip, rot_tip;

		float h = f[n].size.height*cosd(f[n].t);
		tip = j[n] - Point2f(-h*cosd(f[n].p+p.p), h*sind(f[n].p+p.p));

		
		// collision check - left side finger
		for(int k=1; k<n; k++) {
			Point2f tmp_UR, tmp_LR;
			//rot_tip = tip;
			rot_tip = f[n].proj_pt[0];
			rotate_pt(j[k], rot_tip, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, rot_tip, (-1)*p.p);
			tmp_UR = f[k].proj_pt[1];
			rotate_pt(j[k], tmp_UR, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, tmp_UR, (-1)*p.p);
			tmp_LR = f[k].proj_pt[2];
			rotate_pt(j[k], tmp_LR, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, tmp_LR, (-1)*p.p);

			cond1 = (rot_tip.x < tmp_UR.x) && (rot_tip.y > tmp_UR.y);
			cond2 = (rot_tip.x < tmp_LR.x) && (rot_tip.y < tmp_LR.y);
			//cond1 = (rot_tip.x < tmp_UR.x);
			//cond2 = (rot_tip.x < tmp_LR.x);
			if ( cond1 && cond2 )
				is_collision = true;
		}
		
		// collision check - right side finger
		for(int k=n+1; k<5; k++) {
			Point2f tmp_UL, tmp_LL;
			//rot_tip = tip;
			rot_tip = f[n].proj_pt[1];
			rotate_pt(j[k], rot_tip, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, rot_tip, (-1)*p.p);
			tmp_UL = f[k].proj_pt[0];
			rotate_pt(j[k], tmp_UL, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, tmp_UL, (-1)*p.p);
			tmp_LL = f[k].proj_pt[3];
			rotate_pt(j[k], tmp_LL, (-1)*(f[k].p-90));
			rotate_pt(p.ctr, tmp_LL, (-1)*p.p);

			cond1 = (rot_tip.x > tmp_UL.x) && (rot_tip.y > tmp_UL.y);
			cond2 = (rot_tip.x > tmp_LL.x) && (rot_tip.y < tmp_LL.y);
			//cond1 = (rot_tip.x > tmp_UL.x);
			//cond2 = (rot_tip.x > tmp_LL.x);
			if ( cond1 && cond2 )
				is_collision = true;
		}
		if (is_collision)
			v_collision[n] = 100;
	}
	V_score = (-1)*(v_collision[0]+v_collision[1]+v_collision[2]+v_collision[3]+v_collision[4]);
}
void Hand::compute_i_score() {
	I_score = 0;

	float dist_ctr0 = pow(pow((prev_state[1] - p.ctr.x),2.f) + pow((prev_state[2] - p.ctr.y),2.f),0.5f);
	float dist_ctr_ref = pow(pow(p.size.width,2.f)+pow(p.size.height,2.f),0.5f)/2;

	float dist_deg0 = max(prev_state[2],p.p) - min(prev_state[2],p.p);
	dist_deg0 = min(360-dist_deg0, dist_deg0);
	float dist_deg_ref = 30;

	if (prev_state[1] > 0 && prev_state[2] > 0) {
		//I_score += (exp(-dist_ctr0/dist_ctr_ref)-exp(-1.f)) / (1-exp(-1.f)) * 20;
		I_score += (-1*(dist_ctr0/dist_ctr_ref) + 1) * 10;
	}
	else	// initial case
		I_score += 0;

	if (prev_state[2] > 0) {
		//I_score += (exp(-dist_deg0/dist_deg_ref)-exp(-1.f)) / (1-exp(-1.f)) * 10;
		I_score += (-1*(dist_deg0/dist_deg_ref) + 1) * 10;
	}
	else	// initial case
		I_score += 0;
}
void Hand::update_total_score() {
	float w_f0 = 3;
	float w_f1 = 3;
	int w_palm = 3;	// weight for plam
	float lambda = 20;	// for V_score

	M_score = (m_score_f[0]+m_score_f[1]+m_score_f[2]+m_score_f[3]+m_score_f[4]+w_palm*m_score_p)/(5+w_palm);
	V_score = lambda * (w_f0*v_score_f[0]+w_f1*v_score_f[1]+v_score_f[2]+v_score_f[3]+v_score_f[4])/(w_f0+w_f1+3) - (v_collision[0]+v_collision[1]+v_collision[2]+v_collision[3]+v_collision[4]); 
	//T_score = M_score + V_score + I_score;
	T_score = M_score + V_score;

}
void Hand::update_total_score2() {
	T_score = M_score + V_score;
}
float Hand::compute_score(Mat &img, int n = -1, int opt = 0) {
// n = 0~4: compute the score of each finger
// n = -1:  compute all scores
/*
	if (0<=n && n<=4) {
		compute_m_score(img, n);
		compute_v_score(img, n);
	} else if (n<0) {
		for(int i=0; i<5; i++) {
			compute_m_score(img, i);
			compute_v_score(img, i);
		}
	}
	compute_m_score(img, 5);	// palm
	//compute_m_score2(img);
	//compute_i_score();			// inertia
	update_total_score();
*/	
	compute_m_score2(img);
	compute_v_score2(img);
	update_total_score2();

	if (opt==1) {
		if (V_score<-1)
			printf("Total score = %.2f with collision (%d)\n", M_score, int(V_score));
		else
			printf("Total score = %.2f without collision.\n", M_score);
	} else if (opt==2) {
		printf("m_score_p: %.0f\n", m_score_p);
		for(int i=0; i<5; i++)
			printf("m_score_f[%d]: %.0f\n", i, m_score_f[i]);
		for(int i=0; i<5; i++)
			printf("v_score_f[%d]: %.0f\n", i, v_score_f[i]);
		printf("i_score: %.0f\n", I_score);
		printf("Total score: %.2f = %.2f + %.2f + %.2f\n", T_score, M_score, V_score, I_score);
	}

	return T_score;
}
int Hand::classify_pose() {
	int t0 = int(f[0].t);
	int t1 = int(f[1].t);
	int t234 = int((f[2].t+f[3].t+f[4].t)/3);
	return t0*10000+t1*100+t234;
}
float Hand::compute_zoom_angle()
{
	// p f[1]
	float dy = f[1].ctr.y - p.ctr.y;
	float dx = f[1].ctr.x - p.ctr.x;
	dx = -dx;	// ccw direction = zoom in, cw direction = zoom out
	return atan2(dy, dx) * 180 / PI;

}
void Hand::export_tip_pos(float &x, float &y)
{
	x = (f[1].proj_pt[0].x + f[1].proj_pt[1].x) / 2;
	y = (f[1].proj_pt[0].y + f[1].proj_pt[1].y) / 2;
}
#endif