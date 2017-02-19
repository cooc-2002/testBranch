#include "Gesture.h"

using namespace std;

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
Hand::Hand() {
	init_params(80, 60);
	position_update();
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

	p.ctr = cv::Point2f(init_x, init_y);	p.ref_size = cv::Size2f(85, 80);	p.p = 0;

	f[0].ref_size = cv::Size2f(18, 70);
	f[1].ref_size = cv::Size2f(14, 87);
	f[2].ref_size = cv::Size2f(14, 98);
	f[3].ref_size = cv::Size2f(14, 82);
	f[4].ref_size = cv::Size2f(14, 60);
	//f[0].ref_size = cv::Size2f(25, 70);
	//f[1].ref_size = cv::Size2f(20, 87);
	//f[2].ref_size = cv::Size2f(20, 98);
	//f[3].ref_size = cv::Size2f(20, 82);
	//f[4].ref_size = cv::Size2f(19, 60);

	f[0].p = p.p + 120;	f[0].t = 45;
	f[1].p = p.p + 90;	f[1].t = 45;
	f[2].p = p.p + 90;	f[2].t = 45;
	f[3].p = p.p + 90;	f[3].t = 45;
	f[4].p = p.p + 90;	f[4].t = 45;
}
void Hand::scale_hand(float new_scale) {
	scale = new_scale;

	p.size = p.ref_size * new_scale;
	for (int i = 0; i<5; i++) {
		f[i].size = f[i].ref_size * new_scale;
	}
	position_update();
}
void Hand::move_hand(float x, float y) {
	p.ctr.x = x;
	p.ctr.y = y;

	position_update();
}
void Hand::rotate_hand(float pp, float f0p, float f1p, float f2p, float f3p, float f4p) {
	if (pp >= 0) {
		p.p = pp;
		f[0].p = min(max(f[0].p, 100.f), 170.f);
		f[1].p = min(max(f[1].p, 60.f), 120.f);
		f[2].p = min(max(f[2].p, 70.f), 110.f);
		f[3].p = min(max(f[3].p, 70.f), 110.f);
		f[4].p = min(max(f[4].p, 60.f), 110.f);
	}

	if (f0p >= 0)	f[0].p = min(max(f0p, 100.f), 170.f);
	if (f1p >= 0)	f[1].p = min(max(f1p, 60.f), 120.f);
	if (f2p >= 0)	f[2].p = min(max(f2p, 70.f), 110.f);
	if (f3p >= 0)	f[3].p = min(max(f3p, 70.f), 110.f);
	if (f4p >= 0)	f[4].p = min(max(f4p, 60.f), 110.f);

	position_update();
}
void Hand::rotate_hand(float *p_set) {
	if (p_set[0] >= 0) {	// pp
		p.p = p_set[0];
		f[0].p = min(max(f[0].p, 100.f), 170.f);
		f[1].p = min(max(f[1].p, 60.f), 120.f);
		f[2].p = min(max(f[2].p, 70.f), 110.f);
		f[3].p = min(max(f[3].p, 70.f), 110.f);
		f[4].p = min(max(f[4].p, 60.f), 110.f);
	}

	if (p_set[1] >= 0) f[0].p = min(max(p_set[1], 100.f), 170.f);
	if (p_set[2] >= 0) f[1].p = min(max(p_set[2], 60.f), 120.f);
	if (p_set[3] >= 0) f[2].p = min(max(p_set[3], 70.f), 110.f);
	if (p_set[4] >= 0) f[3].p = min(max(p_set[4], 70.f), 110.f);
	if (p_set[5] >= 0) f[4].p = min(max(p_set[5], 60.f), 110.f);

	position_update();
}
void Hand::bend_finger(float f0t, float f1t, float f2t, float f3t, float f4t) {
	if (f0t >= 0) { f[0].t = min(max(f0t, 0.f), 90.f); }	// range = [0, 90]
	if (f1t >= 0) { f[1].t = min(max(f1t, 0.f), 90.f); }
	if (f2t >= 0) { f[2].t = min(max(f2t, 0.f), 90.f); }
	if (f3t >= 0) { f[3].t = min(max(f3t, 0.f), 90.f); }
	if (f4t >= 0) { f[4].t = min(max(f4t, 0.f), 90.f); }

	position_update();
}
void Hand::bend_finger(float *t_set) {
	if (t_set[0] >= 0) { f[0].t = min(max(t_set[0], 0.f), 90.f); }	// range = [0, 90]
	if (t_set[1] >= 0) { f[1].t = min(max(t_set[1], 0.f), 90.f); }
	if (t_set[2] >= 0) { f[2].t = min(max(t_set[2], 0.f), 90.f); }
	if (t_set[3] >= 0) { f[3].t = min(max(t_set[3], 0.f), 90.f); }
	if (t_set[4] >= 0) { f[4].t = min(max(t_set[4], 0.f), 90.f); }

	position_update();
}
void Hand::position_update() {
	for (int i = 0; i<5; i++) {
		if (i == 0)	// refine joint0 (엄지)
			j[i] = p.ctr - cv::Point2f(p.size.width / 2 + p.size.width / 8, 0) + cv::Point2f(0, p.size.height / 8);
		else if (i == 4) // joint4
						 //j[i] = p.ctr - cv::Point2f(p.size.width/2, p.size.height/2) + cv::Point2f(p.size.width/8*(i*2-1), p.size.height/10);
			j[i] = p.ctr - cv::Point2f(p.size.width / 2, p.size.height / 2) + cv::Point2f(p.size.width, p.size.height / 10);
		else if (i == 3) // joint 3
			j[i] = p.ctr - cv::Point2f(p.size.width / 2, p.size.height / 2) + cv::Point2f(p.size.width / 8 * (6), 0);
		else if (i == 2) // joint 2
			j[i] = p.ctr - cv::Point2f(p.size.width / 2, p.size.height / 2) + cv::Point2f(p.size.width / 8 * (4), 0);
		else
			j[i] = p.ctr - cv::Point2f(p.size.width / 2, p.size.height / 2) + cv::Point2f(p.size.width / 8 * (i * 2 - 1), 0);

		rotate_pt(p.ctr, j[i], p.p);


		cv::Point2f tmp_pt;
		float h;
		float half_w, half_h;
		float rot_deg;

		// compute proj_pt (for palm)
		rot_deg = p.p;
		half_w = p.size.width / 2;
		half_h = p.size.height / 2;

		tmp_pt = cv::Point2f(p.ctr.x - half_w, p.ctr.y - half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[0] = cv::Point2i(tmp_pt);	// UL
		tmp_pt = cv::Point2f(p.ctr.x + half_w, p.ctr.y - half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[1] = cv::Point2i(tmp_pt);	// UR
		tmp_pt = cv::Point2f(p.ctr.x + half_w, p.ctr.y + half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[2] = cv::Point2i(tmp_pt);	// LR
		tmp_pt = cv::Point2f(p.ctr.x - half_w, p.ctr.y + half_h);	rotate_pt(p.ctr, tmp_pt, rot_deg);	p.proj_pt[3] = cv::Point2i(tmp_pt);	// LL

																																	// compute ctr      (Add 90: sin->cos, cos->-sin)
		h = f[i].size.height*cos(f[i].t * PI / 180);
		f[i].ctr = j[i] - cv::Point2f(-h / 2 * cos((f[i].p + p.p) * PI / 180), h / 2 * sin((f[i].p + p.p) * PI / 180));
		
		// compute proj_pt (for finger)
		rot_deg = f[i].p - 90 + p.p;
		half_w = f[i].size.width / 2;
		half_h = f[i].size.height / 2 * cos(f[i].t * PI / 180);

		tmp_pt = cv::Point2f(f[i].ctr.x - half_w, f[i].ctr.y - half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[0] = cv::Point2i(tmp_pt);	// UL
		tmp_pt = cv::Point2f(f[i].ctr.x + half_w, f[i].ctr.y - half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[1] = cv::Point2i(tmp_pt);	// UR
		tmp_pt = cv::Point2f(f[i].ctr.x + half_w, f[i].ctr.y + half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[2] = cv::Point2i(tmp_pt);	// LR
		tmp_pt = cv::Point2f(f[i].ctr.x - half_w, f[i].ctr.y + half_h);	rotate_pt(f[i].ctr, tmp_pt, rot_deg);	f[i].proj_pt[3] = cv::Point2i(tmp_pt);	// LL

	}
}
void Hand::draw_hand(char *title, cv::Mat &img) {
	cv::Scalar c_red(0, 0, 128);
	cv::Scalar c_blue(128, 0, 0);
	cv::Scalar c_gray(128, 128, 128);
	cv::Scalar c_palm(14, 201, 255);
	cv::Scalar c_finger(29, 230, 181);
	cv::Scalar c_joint(201, 174, 255);

	rectangle_from_4pt(img, p.proj_pt, c_palm, 2);
	circle(img, p.ctr, 3, c_palm);

	for (int i = 0; i<5; i++) {
		rectangle_from_4pt(img, f[i].proj_pt, c_finger, 2);

		circle(img, f[i].ctr, 3, c_finger);
		circle(img, j[i], 3, c_joint);

		//line(img, f[i].ctr, j[i], c_gray);
	}
	if (title != NULL)
		imshow(title, img);
}
void Hand::compute_m_score(cv::Mat &img, int n) {
	// n = 0~4: each finger
	// n = 5 : palm
	// new version for faster speed
	cv::Point2f uni_pt[4];
	uni_pt[0] = f[n].proj_pt[0];
	uni_pt[1] = f[n].proj_pt[1];
	uni_pt[2] = f[n].proj_pt[2];
	uni_pt[3] = f[n].proj_pt[3];

	for (int i = 0; i<4; i++)
		rotate_pt(f[n].ctr, uni_pt[i], (-1)*(f[n].p - 90));

	// find the smallest square (UL~LR) and crop it
	cv::Point2i UL, LR;
	cv::Point2i cur_pt[4];

	if (n == 5) {
		cur_pt[0] = p.proj_pt[0];	cur_pt[1] = p.proj_pt[1];	cur_pt[2] = p.proj_pt[2];	cur_pt[3] = p.proj_pt[3];
	}
	else {
		cur_pt[0] = f[n].proj_pt[0];	cur_pt[1] = f[n].proj_pt[1];	cur_pt[2] = f[n].proj_pt[2];	cur_pt[3] = f[n].proj_pt[3];
	}

	UL.x = min(cur_pt[0].x, min(cur_pt[1].x, min(cur_pt[2].x, cur_pt[3].x)));
	UL.y = min(cur_pt[0].y, min(cur_pt[1].y, min(cur_pt[2].y, cur_pt[3].y)));
	LR.x = max(cur_pt[0].x, max(cur_pt[1].x, max(cur_pt[2].x, cur_pt[3].x)));
	LR.y = max(cur_pt[0].y, max(cur_pt[1].y, max(cur_pt[2].y, cur_pt[3].y)));

	UL.x = max(UL.x, 0);
	UL.y = max(UL.y, 0);
	LR.x = min(LR.x, img.cols - 1);
	LR.y = min(LR.y, img.rows - 1);

	if (UL.x > img.cols - 1 || UL.y > img.rows - 1 || LR.x < 0 || LR.y < 0) {
		if (n == 5)	m_score_p = 0;
		else		m_score_f[n] = 0;
		return;
	}////////////////////////////////////////////////

	 // fill the patch
	cv::Mat patch_ref, patch_trg;
	patch_ref = img(cv::Rect(UL, LR));
	patch_trg = cv::Mat::zeros(patch_ref.rows, patch_ref.cols, CV_8U);

	cv::Point2i tmp_pt2i[4];
	for (int i = 0; i<4; i++)
		tmp_pt2i[i] = cur_pt[i] - UL;
	fillConvexPoly(patch_trg, tmp_pt2i, 4, cv::Scalar(1));

	// compute n_all and n_filled to calculate final_score.
	cv::Scalar tmp_scalar;
	tmp_scalar = sum(patch_trg);
	int n_all = tmp_scalar.val[0];
	tmp_scalar = sum(patch_ref & patch_trg);
	int n_filled = tmp_scalar.val[0];

	// compute final_score and refine it.
	int knee_pt;	// slope of m_score graph is changed at knee_pt
	if (n == 5)	knee_pt = 90;
	else		knee_pt = 60;

	//printf("Im-%d) %3d/%3d\t\n", n, n_filled, n_all);
	float final_score;
	if (n_all < 50)
		final_score = 100;
	else
		final_score = float(n_filled) / n_all * 100;

	if (final_score <= knee_pt)
		final_score = (-1)*n_all;
	else
		final_score = (final_score - knee_pt) * 100 / (100 - knee_pt);

	if (n == 5)	m_score_p = final_score;
	else		m_score_f[n] = final_score;

	//cout<<final_score<<endl;
}
void Hand::compute_m_score2(cv::Mat &img) {
	cv::Scalar scalar_tp, scalar_fn, scalar_fp;
	float n_tp, n_fn, n_fp;
	cv::Mat im_tp_fp, im_tp_fn, im_tp;

	int lower_bound = max(f[0].proj_pt[3].y, max(p.proj_pt[2].y, p.proj_pt[3].y));
	lower_bound = min(img.rows - 1, lower_bound);

	//im_tp_fp = cv::Mat::zeros(img.rows, img.cols, CV_8U);		// (2) comment this line for setting lower bound
	im_tp_fp = cv::Mat::zeros(lower_bound + 1, img.cols, CV_8U);	// (2) uncomment this line for setting lower bound
	//fillConvexPoly(im_tp_fp, p.proj_pt, 4, cv::Scalar(255));	// (1) comment this line for excluding palm
	for (int n = 0; n<5; n++)
		fillConvexPoly(im_tp_fp, f[n].proj_pt, 4, cv::Scalar(255));
	//im_tp_fn = img.clone();								// (2) comment this line for setting lower bound
	im_tp_fn = img.rowRange(0, lower_bound + 1).clone();		// (2) uncomment this line for setting lower bound
	fillConvexPoly(im_tp_fn, p.proj_pt, 4, cv::Scalar(0));		// (1) uncomment this line for excluding palm
	im_tp = im_tp_fn & im_tp_fp;

	scalar_tp = sum(im_tp);
	scalar_fn = sum(im_tp_fn) - scalar_tp;
	scalar_fp = sum(im_tp_fp) - scalar_tp;

	n_tp = scalar_tp.val[0] / 255.f;
	n_fn = scalar_fn.val[0] / 255.f;
	n_fp = scalar_fp.val[0] / 255.f;

	float precision = n_tp / (n_tp + n_fp);
	float recall = n_tp / (n_tp + n_fn);

	float fscore = 2 * precision*recall / (precision + recall);

	M_score = fscore * 100;
}
void Hand::compute_v_score(cv::Mat &img, int n) {
	// n = 0~4: each finger
	v_score_f[n] = cos(f[n].t * PI / 180);	// basic score

									// collision check
	v_collision[n] = 0;

	bool cond1, cond2, is_collision = false;
	cv::Point2f tip, rot_tip;

	float h = f[n].size.height*cos(f[n].t * PI / 180);
	tip = j[n] - cv::Point2f(-h*cos((f[n].p + p.p) * PI / 180), h*sin((f[n].p + p.p) * PI / 180));


	// collision check - left side finger
	for (int k = 1; k<n; k++) {
		cv::Point2f tmp_UR, tmp_LR;
		//rot_tip = tip;
		rot_tip = f[n].proj_pt[0];
		rotate_pt(j[k], rot_tip, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, rot_tip, (-1)*p.p);
		tmp_UR = f[k].proj_pt[1];
		rotate_pt(j[k], tmp_UR, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, tmp_UR, (-1)*p.p);
		tmp_LR = f[k].proj_pt[2];
		rotate_pt(j[k], tmp_LR, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, tmp_LR, (-1)*p.p);

		cond1 = (rot_tip.x < tmp_UR.x) && (rot_tip.y > tmp_UR.y);
		cond2 = (rot_tip.x < tmp_LR.x) && (rot_tip.y < tmp_LR.y);
		//cond1 = (rot_tip.x < tmp_UR.x);
		//cond2 = (rot_tip.x < tmp_LR.x);
		if (cond1 && cond2)
			is_collision = true;
	}

	// collision check - right side finger
	for (int k = n + 1; k<5; k++) {
		cv::Point2f tmp_UL, tmp_LL;
		//rot_tip = tip;
		rot_tip = f[n].proj_pt[1];
		rotate_pt(j[k], rot_tip, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, rot_tip, (-1)*p.p);
		tmp_UL = f[k].proj_pt[0];
		rotate_pt(j[k], tmp_UL, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, tmp_UL, (-1)*p.p);
		tmp_LL = f[k].proj_pt[3];
		rotate_pt(j[k], tmp_LL, (-1)*(f[k].p - 90));
		rotate_pt(p.ctr, tmp_LL, (-1)*p.p);

		cond1 = (rot_tip.x > tmp_UL.x) && (rot_tip.y > tmp_UL.y);
		cond2 = (rot_tip.x > tmp_LL.x) && (rot_tip.y < tmp_LL.y);
		//cond1 = (rot_tip.x > tmp_UL.x);
		//cond2 = (rot_tip.x > tmp_LL.x);
		if (cond1 && cond2)
			is_collision = true;
	}
	if (is_collision)
		v_collision[n] = 10000;

}
void Hand::compute_v_score2(cv::Mat &img) {

	// collision check
	for (int n = 0; n<5; n++) {
		v_collision[n] = 0;

		bool cond1, cond2, is_collision = false;
		cv::Point2f tip, rot_tip;

		float h = f[n].size.height*cos(f[n].t * PI / 180);
		tip = j[n] - cv::Point2f(-h*cos((f[n].p + p.p) * PI / 180), h*sin((f[n].p + p.p) * PI / 180));


		// collision check - left side finger
		for (int k = 1; k<n; k++) {
			cv::Point2f tmp_UR, tmp_LR;
			//rot_tip = tip;
			rot_tip = f[n].proj_pt[0];
			rotate_pt(j[k], rot_tip, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, rot_tip, (-1)*p.p);
			tmp_UR = f[k].proj_pt[1];
			rotate_pt(j[k], tmp_UR, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, tmp_UR, (-1)*p.p);
			tmp_LR = f[k].proj_pt[2];
			rotate_pt(j[k], tmp_LR, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, tmp_LR, (-1)*p.p);

			cond1 = (rot_tip.x < tmp_UR.x) && (rot_tip.y > tmp_UR.y);
			cond2 = (rot_tip.x < tmp_LR.x) && (rot_tip.y < tmp_LR.y);
			//cond1 = (rot_tip.x < tmp_UR.x);
			//cond2 = (rot_tip.x < tmp_LR.x);
			if (cond1 && cond2)
				is_collision = true;
		}

		// collision check - right side finger
		for (int k = n + 1; k<5; k++) {
			cv::Point2f tmp_UL, tmp_LL;
			//rot_tip = tip;
			rot_tip = f[n].proj_pt[1];
			rotate_pt(j[k], rot_tip, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, rot_tip, (-1)*p.p);
			tmp_UL = f[k].proj_pt[0];
			rotate_pt(j[k], tmp_UL, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, tmp_UL, (-1)*p.p);
			tmp_LL = f[k].proj_pt[3];
			rotate_pt(j[k], tmp_LL, (-1)*(f[k].p - 90));
			rotate_pt(p.ctr, tmp_LL, (-1)*p.p);

			cond1 = (rot_tip.x > tmp_UL.x) && (rot_tip.y > tmp_UL.y);
			cond2 = (rot_tip.x > tmp_LL.x) && (rot_tip.y < tmp_LL.y);
			//cond1 = (rot_tip.x > tmp_UL.x);
			//cond2 = (rot_tip.x > tmp_LL.x);
			if (cond1 && cond2)
				is_collision = true;
		}
		if (is_collision)
			v_collision[n] = 100;
	}
	V_score = (-1)*(v_collision[0] + v_collision[1] + v_collision[2] + v_collision[3] + v_collision[4]);
}
void Hand::compute_i_score() {
	I_score = 0;

	float dist_ctr0 = pow(pow((prev_state[1] - p.ctr.x), 2.f) + pow((prev_state[2] - p.ctr.y), 2.f), 0.5f);
	float dist_ctr_ref = pow(pow(p.size.width, 2.f) + pow(p.size.height, 2.f), 0.5f) / 2;

	float dist_deg0 = max(prev_state[2], p.p) - min(prev_state[2], p.p);
	dist_deg0 = min(360 - dist_deg0, dist_deg0);
	float dist_deg_ref = 30;

	if (prev_state[1] > 0 && prev_state[2] > 0) {
		//I_score += (exp(-dist_ctr0/dist_ctr_ref)-exp(-1.f)) / (1-exp(-1.f)) * 20;
		I_score += (-1 * (dist_ctr0 / dist_ctr_ref) + 1) * 10;
	}
	else	// initial case
		I_score += 0;

	if (prev_state[2] > 0) {
		//I_score += (exp(-dist_deg0/dist_deg_ref)-exp(-1.f)) / (1-exp(-1.f)) * 10;
		I_score += (-1 * (dist_deg0 / dist_deg_ref) + 1) * 10;
	}
	else	// initial case
		I_score += 0;
}
void Hand::update_total_score() {
	float w_f0 = 3;
	float w_f1 = 3;
	int w_palm = 3;	// weight for plam
	float lambda = 20;	// for V_score

	M_score = (m_score_f[0] + m_score_f[1] + m_score_f[2] + m_score_f[3] + m_score_f[4] + w_palm*m_score_p) / (5 + w_palm);
	V_score = lambda * (w_f0*v_score_f[0] + w_f1*v_score_f[1] + v_score_f[2] + v_score_f[3] + v_score_f[4]) / (w_f0 + w_f1 + 3) - (v_collision[0] + v_collision[1] + v_collision[2] + v_collision[3] + v_collision[4]);
	//T_score = M_score + V_score + I_score;
	T_score = M_score + V_score;

}
void Hand::update_total_score2() {
	T_score = M_score + V_score;
}
float Hand::compute_score(cv::Mat &img, int n = -1, int opt = 0) {
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

	if (opt == 1) {
		if (V_score<-1)
			printf("Total score = %.2f with collision (%d)\n", M_score, int(V_score));
		else
			printf("Total score = %.2f without collision.\n", M_score);
	}
	else if (opt == 2) {
		printf("m_score_p: %.0f\n", m_score_p);
		for (int i = 0; i<5; i++)
			printf("m_score_f[%d]: %.0f\n", i, m_score_f[i]);
		for (int i = 0; i<5; i++)
			printf("v_score_f[%d]: %.0f\n", i, v_score_f[i]);
		printf("i_score: %.0f\n", I_score);
		printf("Total score: %.2f = %.2f + %.2f + %.2f\n", T_score, M_score, V_score, I_score);
	}

	return T_score;
}
int Hand::classify_pose() {
	int t0 = int(f[0].t);
	int t1 = int(f[1].t);
	int t234 = int((f[2].t + f[3].t + f[4].t) / 3);
	return t0 * 10000 + t1 * 100 + t234;
}
float Hand::compute_zoom_angle()
{
	// p f[1]
	float dy = f[1].ctr.y - p.ctr.y;
	float dx = f[1].ctr.x - p.ctr.x;
	dx = -dx;	// ccw direction = zoom in, cw direction = zoom out
	return atan2(dy, dx) * 180 / PI;
}
void Hand::export_tip_pos(cv::Point2f &x_and_y, float IM_SCALE)
{
	float x = (f[1].proj_pt[0].x + f[1].proj_pt[1].x) / 2 / IM_SCALE;
	float y = (f[1].proj_pt[0].y + f[1].proj_pt[1].y) / 2 / IM_SCALE;
	x_and_y = cv::Point2f(x, y);
}
void Hand::rotate_pt(cv::Point2f ref_p, cv::Point2f &trg_p, float deg) {
	/////////////??????????????????????????????????
	float x = trg_p.x - ref_p.x;
	float y = trg_p.y - ref_p.y;

	float x_rot = x*cos(-deg * PI / 180) - y*sin(-deg * PI / 180);
	float y_rot = x*sin(-deg * PI / 180) + y*cos(-deg * PI / 180);

	trg_p.x = x_rot + ref_p.x;
	trg_p.y = y_rot + ref_p.y;
}
void Hand::rectangle2(cv::Mat &img, cv::Point2f ctr, cv::Size2f size, cv::Scalar &color) {	// using center point and size
	rectangle(img, cv::Point(ctr.x - size.width / 2, ctr.y - size.height / 2), cv::Point(ctr.x + size.width / 2, ctr.y + size.height / 2), color);
}
void Hand::rectangle3(cv::Mat &img, cv::Point2f ctr, cv::Size2f size, float theta, cv::Scalar &color, int thickness) {
	float w = size.width;
	float h = size.height;
	float hwcos = w / 2 * cos(theta);	// half width cos theta
	float hwsin = w / 2 * sin(theta);
	float hhcos = h / 2 * cos(theta);
	float hhsin = h / 2 * sin(theta);

	cv::Point2f UL(ctr.x + hwsin - hhcos, ctr.y + hhsin + hwcos);	// upper left
	cv::Point2f UR(ctr.x - hwsin - hhcos, ctr.y + hhsin - hwcos);
	cv::Point2f LL(ctr.x + hwsin + hhcos, ctr.y - hhsin + hwcos);
	cv::Point2f LR(ctr.x - hwsin + hhcos, ctr.y - hhsin - hwcos);	// lower right

	line(img, UL, UR, color, thickness);
	line(img, UR, LR, color, thickness);
	line(img, LR, LL, color, thickness);
	line(img, LL, UL, color, thickness);
}
void Hand::rectangle_from_4pt(cv::Mat &img, cv::Point2i *proj_pt, cv::Scalar &color, int thickness) {
	line(img, proj_pt[0], proj_pt[1], color, thickness);
	line(img, proj_pt[1], proj_pt[2], color, thickness);
	line(img, proj_pt[2], proj_pt[3], color, thickness);
	line(img, proj_pt[3], proj_pt[0], color, thickness);
}


Gesture::Gesture()
{
	// Kalman filter initialization
	kf = cv::KalmanFilter(28, 14, 0);
	kf.statePre = 0;	// initial state
	kf.statePre.at<float>(0) = IM_SCALE;

	cv::Mat kf_tm = cv::Mat::zeros(28, 28, CV_32F);	//transition_matrix
	float *kf_tm_data = (float *)kf_tm.data;
	for (int i = 0; i < 28; i++) {
		kf_tm_data[i * 28 + i] = 0.1;
		if (i < 14)
			kf_tm_data[i * 28 + i + 14] = 0.1;
	}
	kf_tm_data[1 * 28 + 1 + 14] = 10;	// fast moving center
	kf_tm_data[2 * 28 + 2 + 14] = 10;
	kf.transitionMatrix = kf_tm;

	setIdentity(kf.measurementMatrix);
	//setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-4));
	//setIdentity(kf.measurementNoiseCov, cv::Scalar::all(0.1));
	//setIdentity(kf.errorCovPost, cv::Scalar::all(0.1));
	setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2));
	setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1.0));
	setIdentity(kf.errorCovPost, cv::Scalar::all(1.0));

	// gesture initialization	
	is_there_hand = false;
	bDraw = false;
	IM_SCALE = 0.25;
	sprintf(title_gesture, "gesture");

	iOpenCnt = 0; iCloseCnt = 0; iZoomCnt = 0; iRelocalCnt = 0;
	bOverview = false; bZoom = false; bRelocal = false;
	fAngle1 = -1; fAngle2 = -1; fZoomMag = 1;
	p2fTipRef = cv::Point2f(-1, -1);
	fDeltaX = 0; fDeltaY = 0;

	pose_queue.resize(5);
}
void Gesture::find_best_hand_model()
{
	is_there_hand = false;
	// only when the input image data exist
	if (im_origin.empty())
		return;

	float best_score = LONG_MIN;
	cv::Point2i ctr;
	vector<cv::Mat> contours_filled;
	double max_dist;
	float score0;
	float h0_var[14];

	// extract and analysis contour
	int n_contour = extract_contours(im_origin, contours_filled, IM_SCALE, 1.5);
	for (int i = 0; i<n_contour; i++) {
		get_ctr_pt(contours_filled[i], ctr, max_dist);
		if (max_dist < 20) continue;
		Hand h0(ctr.x, ctr.y);
		h0.scale_hand(max_dist * 2 / 100);// 비율 설정...

		score0 = analysis_contours(contours_filled[i], h0);
		if (score0 > 30 && score0 > best_score) {
			best_score = score0;

			h0.export_variables(h0_var);
			best_hand.move_hand(h0_var[1], h0_var[2]);
			best_hand.scale_hand(h0_var[0]);
			best_hand.rotate_hand(h0_var + 3);
			best_hand.bend_finger(h0_var + 9);
			is_there_hand = true;

			//imshow("contour", contours_filled[i]);
		}
	}
	contours_filled.clear();
	n_contour = extract_contours(im_origin, contours_filled, IM_SCALE, 1.3);
	for (int i = 0; i<n_contour; i++) {
		get_ctr_pt(contours_filled[i], ctr, max_dist);
		if (max_dist < 20) continue;
		Hand h0(ctr.x, ctr.y);
		h0.scale_hand(max_dist * 2 / 100);// 비율 설정...

		score0 = analysis_contours(contours_filled[i], h0);
		if (score0 > 30 && score0 > best_score+10) {
			best_score = score0-10;

			h0.export_variables(h0_var);
			best_hand.move_hand(h0_var[1], h0_var[2]);
			best_hand.scale_hand(h0_var[0]);
			best_hand.rotate_hand(h0_var + 3);
			best_hand.bend_finger(h0_var + 9);
			is_there_hand = true;

			//imshow("contour", contours_filled[i]);
		}
	}
}
void Gesture::analysis_hand_pose()
{
	// estimate pose
	int curr_pose = -1;
	if (is_there_hand)	curr_pose = best_hand.classify_pose();
	filtered_pose = estimate_pose(pose_queue, curr_pose);

	// write pose_type_str
	if (filtered_pose == POSE_OTHERS)	sprintf(pose_type_str, "POSE: ---");
	else if (filtered_pose == POSE_CLOSE)	sprintf(pose_type_str, "POSE: CLOSE");
	else if (filtered_pose == POSE_POINTING)	sprintf(pose_type_str, "POSE: POINTING");
	else if (filtered_pose == POSE_FINGER_GUN)	sprintf(pose_type_str, "POSE: FINGER GUN");
	else if (filtered_pose == POSE_OPEN)	sprintf(pose_type_str, "POSE: OPEN");

}
void Gesture::generate_output_signal()
{
	// count
	int maxCnt = 7;
	if (filtered_pose == POSE_OPEN)			iOpenCnt = iOpenCnt + 1 < maxCnt ? iOpenCnt + 1 : maxCnt;
	else									iOpenCnt = iOpenCnt - 1 > 0 ? iOpenCnt - 1 : 0;
	if (filtered_pose == POSE_CLOSE)		iCloseCnt = iCloseCnt + 1 < maxCnt ? iCloseCnt + 1 : maxCnt;
	else									iCloseCnt = iCloseCnt - 1 > 0 ? iCloseCnt - 1 : 0;
	if (filtered_pose == POSE_FINGER_GUN)	iZoomCnt = iZoomCnt + 1 < maxCnt ? iZoomCnt + 1 : maxCnt;
	else									iZoomCnt = iZoomCnt - 1 > 0 ? iZoomCnt - 1 : 0;
	if (filtered_pose == POSE_POINTING)		iRelocalCnt = iRelocalCnt + 1 < maxCnt ? iRelocalCnt + 1 : maxCnt;
	else									iRelocalCnt = iRelocalCnt - 1 > 0 ? iRelocalCnt - 1 : 0;

	// generate output signal
	if ((iOpenCnt > 2) && (filtered_pose == POSE_CLOSE)) {
		bOverview = true;
	}
	else if ((iCloseCnt > 2) && (filtered_pose == POSE_OPEN)) {
		bOverview = false;
	}
		
	if (bOverview && iCloseCnt>4)
		sprintf(gesture_overview_str, "Overview mode ON -->");
	else if (bOverview)
		sprintf(gesture_overview_str, "Overview mode ON");
	else if ((!bOverview) && iOpenCnt>4)
		sprintf(gesture_overview_str, "Overview mode OFF -->");
	else
		sprintf(gesture_overview_str, "Overview mode OFF");

	sprintf(gesture_str, "---");
	if (iZoomCnt > 3) {
		bZoom = true;
		if (is_there_hand) {	// 선택된 hand 모양이 없는 경우, zoom mode는 유지하되 값은 입력하지 않음
			if (fAngle1 == -1) {	// zoom mode로 첫 진입
				fAngle1 = best_hand.compute_zoom_angle();	// current angle (0~180)
				fAngle2 = best_hand.compute_zoom_angle();	// current angle (0~180)
			}
			else {
				fAngle2 = best_hand.compute_zoom_angle();	// current angle (0~180)
			}
			fZoomMag = (fAngle2 - fAngle1) / 10 * 6.f;

			if (fZoomMag >=0)
				sprintf(gesture_str, "Zoom in/out MODE, %+.1f", fZoomMag);
			else
				sprintf(gesture_str, "Zoom in/out MODE, %+.1f", fZoomMag);
		}
	}
	else {
		bZoom = false;
		fAngle1 = -1;
		fAngle2 = -1;
		fZoomMag = 1;
	}

	if (iRelocalCnt > 3) {
		bRelocal = true;
		if (is_there_hand) {	// 선택된 hand 모양이 없는 경우, relocalization mode는 유지하되 값은 입력하지 않음
			if (p2fTipRef.x < 0 && p2fTipRef.y < 0) {	// Relocalization mode로 첫 진입
				fDeltaX = 0;
				fDeltaY = 0;
				best_hand.bend_finger(-1, 0, -1, -1, -1);	// trick
				best_hand.export_tip_pos(p2fTipRef, IM_SCALE);	// initial guess from the hand model
				rSearchArea = cv::Rect(cv::Point2i(p2fTipRef-cv::Point2f(20,20)), cv::Size2i(41, 41));	// set the search area
				if (rSearchArea.x < 0 || rSearchArea.y < 0 || (rSearchArea.br().x>=im_origin.cols) || (rSearchArea.br().y>=im_origin.rows)) {
					p2fTipRef = cv::Point2f(-1,-1);
				}
				else {
					check_skin_color(mSearchPatch, im_origin(rSearchArea), 1.5);
					//cv::cvtColor(im_origin(rSearchArea), mSearchPatch, CV_BGR2GRAY);	// copy the 8bit image of search area
					
					vector<cv::Point2f> corners;
					// 3-th: maxCorners – The maximum number of corners to return
					// 4-th:  qualityLevel – the value of the parameter is multiplied by the by the best corner quality measure
					// 5-th: minDistance – The minimum possible Euclidean distance between the returned corners
					cv::goodFeaturesToTrack(mSearchPatch, corners, 1, 0.01, 10);	// find the real tip
					if (corners.size() < 1) {
						p2fTipRef = cv::Point2f(-1, -1);
					}
					else {
						p2fTipRef = cv::Point2f(rSearchArea.tl()) + corners[0];

						p2fTip = p2fTipRef;
						cv::cvtColor(im_origin(cv::Rect(cv::Point2i(p2fTip - cv::Point2f(3, 3)), cv::Size2i(11, 11))), mTipPatch, CV_BGR2GRAY); // register the tip patch
					}
				}
			}
			else {
				cv::Point2f p2fTipTmp;
				best_hand.bend_finger(-1, 0, -1, -1, -1);	// trick
				best_hand.export_tip_pos(p2fTipTmp, IM_SCALE);
				cv::Rect rSearchObserv = cv::Rect(cv::Point2i(p2fTipTmp - cv::Point2f(20, 20)), cv::Size2i(41, 41));
				cv::Rect rSearchPredict = cv::Rect(cv::Point2i(p2fTip - cv::Point2f(20, 20)), cv::Size2i(41, 41));
				rSearchArea = rSearchObserv | rSearchPredict;
				if (rSearchArea.x < 0 || rSearchArea.y < 0 || (rSearchArea.br().x >= im_origin.cols) || (rSearchArea.br().y >= im_origin.rows)) {
					p2fTipRef = cv::Point2f(-1, -1);
					fDeltaX = 0;
					fDeltaY = 0;
				}
				else {
					cv::cvtColor(im_origin(rSearchArea), mSearchPatch, CV_BGR2GRAY);	// copy the 8bit image of search area

					// matching mTipPatch in mSearchPatch
					cv::Mat mSearchResult;
					cv::Point2i p2iMinLoc;
					cv::matchTemplate(mSearchPatch, mTipPatch, mSearchResult, CV_TM_SQDIFF);	// matching CV_TM_CCORR
					mSearchResult.at<float>(15, 15) -= 0.1;			// center oriented
					cv::minMaxLoc(mSearchResult, NULL, NULL, &p2iMinLoc, NULL);					// matching

					// update the results
					p2fTip = cv::Point2f(rSearchArea.tl()) + cv::Point2f(3, 3) + cv::Point2f(p2iMinLoc);
					cv::cvtColor(im_origin(cv::Rect(cv::Point2i(p2fTip - cv::Point2f(3, 3)), cv::Size2i(11, 11))), mTipPatch, CV_BGR2GRAY);

					float norm_val = sqrt((p2fTip - p2fTipRef).dot(p2fTip - p2fTipRef)) + 0.0000001;
					fDeltaX = 15 * (p2fTip.x - p2fTipRef.x) / norm_val;
					fDeltaY = 15 * -(p2fTip.y - p2fTipRef.y) / norm_val;
				}
			}

			float fDir = atan2(fDeltaY, fDeltaX) * 180 / PI;
			if (fDeltaX == 0 && fDeltaY == 0)
				sprintf(gesture_str, "Relocalization MODE, -");
			else if (fDir >= 45 && fDir < 135)
				sprintf(gesture_str, "Relocalization MODE, ^");
			else if (fDir >= 135 || fDir < -135)
				sprintf(gesture_str, "Relocalization MODE, <");
			else if (fDir <= -45 && fDir > -135)
				sprintf(gesture_str, "Relocalization MODE, V");
			else
				sprintf(gesture_str, "Relocalization MODE, >");
		}
	}
	else {
		bRelocal = false;
		p2fTipRef = cv::Point2f(-1, -1);
		fDeltaX = 0;
		fDeltaY = 0;
	}
}
void Gesture::apply_kalman_filter()
{
	cv::Mat zk = cv::Mat::zeros(14, 1, CV_32F);
	

	kf.predict();		// Kalman filtering 1st step: prediction

	if (is_there_hand) {
		best_hand.export_variables(x_kf);

		float *zk_data = (float *)zk.data;
		for (int i = 0; i < 14; i++)
			zk_data[i] = x_kf[i];
		kf.correct(zk);// Kalman filtering 2nd step: update
					   //float *kf_data = (float *)kf.statePost.data;
	}
}
void Gesture::draw_filtered_gesture()
{
	if (is_there_hand) {
		// --- Applying Kalman filtered results (Applying scaled results)
		Hand hand_kf(x_kf[1] / IM_SCALE, x_kf[2] / IM_SCALE);
		hand_kf.scale_hand(x_kf[0] / IM_SCALE);
		hand_kf.rotate_hand(x_kf + 3);
		hand_kf.bend_finger(x_kf + 9);
		
		//char score_str_kf[10];
		//float kf_score = hand_kf.compute_score(im_origin);
		//sprintf(score_str_kf, "%.0f", kf_score);

		//// --- Enforced shape change
		if (filtered_pose == POSE_CLOSE)			hand_kf.bend_finger(90, 90, 90, 90, 90);
		else if (filtered_pose == POSE_POINTING)	hand_kf.bend_finger(90, 0, 90, 90, 90);
		else if (filtered_pose == POSE_FINGER_GUN)	hand_kf.bend_finger(0, 0, 90, 90, 90);
		else if (filtered_pose == POSE_OPEN)		hand_kf.bend_finger(0, 0, 0, 0, 0);

		// --- DISPLAY strings
		//putText(im_show, score_str_kf, cv::Point(x_kf[1] / IM_SCALE, x_kf[2] / IM_SCALE), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(201, 174, 255), 2);
		putText(im_show, pose_type_str, cv::Point(0, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);
		putText(im_show, gesture_overview_str, cv::Point(0, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);
		putText(im_show, gesture_str, cv::Point(0, 75), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);

		if (bRelocal) {
			cv::circle(im_show, p2fTip, 5, cv::Scalar(0, 0, 0), 2);
			cv::circle(im_show, p2fTipRef, 10, cv::Scalar(0, 0, 0), 3);
		}

		if (bDraw)
			hand_kf.draw_hand(title_gesture, im_show);
	}
	else {
		// --- DISPLAY strings
		//putText(im_show, score_str_kf, cv::Point(kf_data[1] / IM_SCALE, kf_data[2] / IM_SCALE), FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(201, 174, 255), 2);
		putText(im_show, pose_type_str, cv::Point(0, 25), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);
		putText(im_show, gesture_overview_str, cv::Point(0, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);
		putText(im_show, gesture_str, cv::Point(0, 75), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(29, 230, 181), 2);

		if (bDraw)
			cv::imshow(title_gesture, im_show);
	}

}
int Gesture::gesture_main(const cv::Mat im_origin0,
	bool &bOverview1, bool &bZoom1, bool &bRelocal1,
	float &fZoomMag1, float &fDeltaX1, float &fDeltaY1, bool bDraw1)
{
	// display original image first
	im_origin = im_origin0.clone();
	balance_white(im_origin);
	im_show = im_origin.clone();
	bDraw = bDraw1;
	if (bDraw)
		cv::imshow(title_gesture, im_show);

	// perform main functions
	find_best_hand_model();
	analysis_hand_pose();
	generate_output_signal();
	apply_kalman_filter();
	draw_filtered_gesture();

	// output generation
	bOverview1 = bOverview;
	bZoom1 = bZoom;
	bRelocal1 = bRelocal;
	fZoomMag1 = fZoomMag;
	fDeltaX1 = fDeltaX;
	fDeltaY1 = fDeltaY;

	if (bDraw)
		cv::waitKey(1);
	return 0;
}

int Gesture::extract_contours(const cv::Mat im_origin, vector<cv::Mat> &contours_filled, double im_scale, float p) {
	// --- declaring variables
	cv::Mat im_color, im_bw;	// the original image -> skin color filtering -> bw(0/255) image
	vector<vector<cv::Point> > contours_boundary;
	vector<cv::Vec4i> ignore_this_var_hierarchy;
	cv::Mat ctr_mask;	// zero for border, one for center
	cv::Mat ctr_masked_im;

	// --- scaling down & skin color filtering & erode (dilate at the last step)
	resize(im_origin, im_color, cv::Size(), im_scale, im_scale, cv::INTER_NEAREST);
	check_skin_color(im_bw, im_color, p);
	//check_skin_color_rgb(im_bw, im_color);
	erode(im_bw, im_bw, cv::Mat());
	//imshow("im_bw", im_bw);

	// --- extracting contours_boundary
	findContours(im_bw, contours_boundary, ignore_this_var_hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// --- eliminating small contours (under 5%)
	for (int i = 0; i<contours_boundary.size(); i++) {
		double area = contourArea(contours_boundary[i]);
		if (area / im_bw.rows / im_bw.cols * 100 < 5) {
			contours_boundary.erase(contours_boundary.begin() + i);
			i--;
		}
	}

	// --- allocating contours_filled
	contours_filled.resize(contours_boundary.size());
	for (int i = 0; i<contours_filled.size(); i++)
		contours_filled[i] = cv::Mat::zeros(im_bw.size(), CV_8U);

	// --- generating ctr_mask
	ctr_mask = cv::Mat::zeros(im_bw.size(), CV_8U);
	ctr_mask(cv::Rect(cvFloor(im_bw.cols*0.15), cvFloor(im_bw.rows*0.15), cvCeil(im_bw.cols*0.7), cvCeil(im_bw.rows*0.7))) = 255;

	// --- filling contours & remaining center located contours only 
	cv::Scalar tmp1, tmp2;
	for (int i = 0; i<contours_filled.size(); i++) {
		drawContours(contours_filled[i], contours_boundary, i, cv::Scalar(255), CV_FILLED);

		ctr_masked_im = contours_filled[i] & ctr_mask;
		tmp1 = sum(ctr_masked_im);
		tmp2 = sum(contours_filled[i]);
		if (tmp1[0] / tmp2[0] < 0.5) {
			contours_filled.erase(contours_filled.begin() + i);
			i--;
		}
	}

	// --- dilate
	for (int i = 0; i<contours_filled.size(); i++) {
		dilate(contours_filled[i], contours_filled[i], cv::Mat());
	}

	return contours_filled.size();
}
void Gesture::check_skin_color(cv::Mat &im_gray, const cv::Mat im_color, float p)
{
	

	// 128<=Cr<=170
	// 73<=Cb<=158

	// hand model coef를 만드는 과정은 matlab_codes 폴더의 script_hand_model_coef.m을 참조할 것
	// 아래 주석은 파일을 읽기 위해서 uncomment해야함. 최적화를 위해서 demo version의 값을 직접 입력해 둔 상태.
	//char filepath[100];
	//sprintf(filepath, "hand_model_coef.txt");

	//FILE *f;
	//f = fopen(filepath, "r");

	//double coef[6][4];
	//// row: rg, rb, gr, gb, br, bg
	//// column: 2nd coef, 1st coef, 0th coef, std
	//for (size_t i = 0; i<6; i++) {
	//	fscanf(f, "%lf\t%lf\t%lf\t%lf\n", &coef[i][0], &coef[i][1], &coef[i][2], &coef[i][3]);
	//}
	//fclose(f);
	double coef[6][4] = {
		{0.005485235, -0.759546699,	76.18973835, 41.3819875},
		{0.008187628, -1.761049365, 134.4627591, 39.35200149},
		{-0.004843374, 2.094733951, 3.235560416, 37.05600849},
		{0.002435544, 0.311895274, 13.22073297, 41.7155925},
		{-0.006532158, 2.227383584, 38.17867525, 37.20173606},
		{-0.002743394, 1.604809713, 2.256425678, 42.83449595} };
	

	cv::Mat sR, sG, sB;	// split BGR
	vector< cv::Mat > tmp_mat;
	split(im_color, tmp_mat);
	tmp_mat[0].convertTo(sB, CV_32F);
	tmp_mat[1].convertTo(sG, CV_32F);
	tmp_mat[2].convertTo(sR, CV_32F);

	vector< cv::Mat > fitmat; // 모델에 따라 예측된 값
	fitmat.resize(6);
	fitmat[0] = sR.mul(sR, coef[0][0]) + sR*coef[0][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[0][2];
	fitmat[1] = sR.mul(sR, coef[1][0]) + sR*coef[1][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[1][2];
	fitmat[2] = sG.mul(sG, coef[2][0]) + sG*coef[2][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[2][2];
	fitmat[3] = sG.mul(sG, coef[3][0]) + sG*coef[3][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[3][2];
	fitmat[4] = sB.mul(sB, coef[4][0]) + sB*coef[4][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[4][2];
	fitmat[5] = sB.mul(sB, coef[5][0]) + sB*coef[5][1] + cv::Mat::ones(sR.rows, sR.cols, CV_32F)*coef[5][2];

	vector< cv::Mat > cond;
	cond.resize(6);
	cond[0] = (fitmat[0] - (p*coef[0][3]) <= sG) & (sG <= fitmat[0] + (p*coef[0][3]));
	cond[1] = (fitmat[1] - (p*coef[1][3]) <= sB) & (sB <= fitmat[1] + (p*coef[1][3]));
	cond[2] = (fitmat[2] - (p*coef[2][3]) <= sR) & (sR <= fitmat[2] + (p*coef[2][3]));
	cond[3] = (fitmat[3] - (p*coef[3][3]) <= sB) & (sB <= fitmat[3] + (p*coef[3][3]));
	cond[4] = (fitmat[4] - (p*coef[4][3]) <= sR) & (sR <= fitmat[4] + (p*coef[4][3]));
	cond[5] = (fitmat[5] - (p*coef[5][3]) <= sG) & (sG <= fitmat[5] + (p*coef[5][3]));

	cv::Mat total_cond;
	total_cond = cond[0] & cond[1] & cond[2] & cond[3] & cond[4] & cond[5];
	im_gray = total_cond * 255;

	//imshow("im_gray1", im_gray);


	// 위의 기준으로 skin color가 아닌 pixel들을 처리하기 위하여.
	// 기존 단계에서 skin color로 판별된 pixel들과 위치가 가까운 곳에 대해서는 좀 더 완화된 (bigger p)를 이용.

	int dilation_size = 5;
	cv::Mat element = getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		cv::Point(dilation_size, dilation_size));
	cv::Mat im_dilate;
	dilate(im_gray, im_dilate, element, cv::Point(-1, -1), 3);

	p += 0.5;
	cond[0] = (fitmat[0] - (p*coef[0][3]) <= sG) & (sG <= fitmat[0] + (p*coef[0][3]));
	cond[1] = (fitmat[1] - (p*coef[1][3]) <= sB) & (sB <= fitmat[1] + (p*coef[1][3]));
	cond[2] = (fitmat[2] - (p*coef[2][3]) <= sR) & (sR <= fitmat[2] + (p*coef[2][3]));
	cond[3] = (fitmat[3] - (p*coef[3][3]) <= sB) & (sB <= fitmat[3] + (p*coef[3][3]));
	cond[4] = (fitmat[4] - (p*coef[4][3]) <= sR) & (sR <= fitmat[4] + (p*coef[4][3]));
	cond[5] = (fitmat[5] - (p*coef[5][3]) <= sG) & (sG <= fitmat[5] + (p*coef[5][3]));

	total_cond = cond[0] & cond[1] & cond[2] & cond[3] & cond[4] & cond[5];
	im_gray = (total_cond & im_dilate) * 255;

	//imshow("im_gray2", im_gray);
}
void Gesture::check_skin_color_rgb(cv::Mat &im_gray, const cv::Mat im_color)
{
	// R>95 and G>40 and B>20 and
	// max(R,G,B)-min(R,G,B)>15 and
	// |R-G|>15 and R>G and R>B

	vector< cv::Mat> im_bgr;
	cv::split(im_color, im_bgr);

	cv::Mat cond_b = im_bgr[0] > 20;
	cv::Mat cond_g = im_bgr[1] > 40;
	cv::Mat cond_r = im_bgr[2] > 95;
	cv::Mat cond_d1 = abs(im_bgr[2] - im_bgr[1]) > 15;
	cv::Mat cond_d2 = (im_bgr[2] - im_bgr[1]) > 0;
	cv::Mat cond_d3 = (im_bgr[2] - im_bgr[0]) > 0;
	cv::Mat cond_d4 = cv::max(cv::max(im_bgr[0], im_bgr[1]), im_bgr[2]) - cv::min(cv::min(im_bgr[0], im_bgr[1]), im_bgr[2]) > 15;

	im_gray = (cond_b & cond_g & cond_r) & (cond_d1 & cond_d2 & cond_d3 & cond_d4);
}
void Gesture::get_ctr_pt(const cv::Mat im_bw, cv::Point2i &ctr, double &max_dist)
{
	//cv::Mat im_dist = cv::Mat::zeros(im_bw.size(), CV_32F);
	//distanceTransform(im_bw, im_dist, CV_DIST_L2, 5);
	//minMaxLoc(im_dist, NULL, &max_dist, NULL, &ctr); // min val, max val, min loc, max loc

	// set boundary line to zero
	cv::Mat im_bw2 = im_bw.clone();
	im_bw2(cv::Range(0, 1), cv::Range(0, im_bw.cols)) = 0;
	im_bw2(cv::Range(im_bw.rows - 1, im_bw.rows), cv::Range(0, im_bw.cols)) = 0;
	im_bw2(cv::Range(0, im_bw.rows), cv::Range(0, 1)) = 0;
	im_bw2(cv::Range(0, im_bw.rows), cv::Range(im_bw.cols - 1, im_bw.cols)) = 0;
	cv::Mat im_dist = cv::Mat::zeros(im_bw.size(), CV_32F);
	distanceTransform(im_bw2, im_dist, CV_DIST_L2, 5);
	minMaxLoc(im_dist, NULL, &max_dist, NULL, &ctr); // min val, max val, min loc, max loc

													 //imshow("dist image", im_dist/50);
													 //printf("(%d, %d)\n", ctr.y, ctr.x);
}
float Gesture::analysis_contours(cv::Mat & contour, Hand &h0)
{

	int best_j[M], best_k[M];
	float best_score[M];
	float best_of_best_score = LONG_MIN;
	float x[14];

	for (int m = 0; m<M; m++)
		best_score[m] = LONG_MIN;

	//h0.bend_finger(0, 0, 60, 60, 60);
	h0.bend_finger(0, 0, 0, 0, 0);
	int k_start = -50;
	int k_end = 50;
	for (int k = k_start; k <= k_end; k += 10) {
		if (k < 0)	h0.rotate_hand(k + 360, -1, -1, -1, -1, -1);
		else		h0.rotate_hand(k, -1, -1, -1, -1, -1);
		//h0.move_hand(ctr.x, ctr.y);

		float score = h0.compute_score(contour);
		for (int m = 0; m<M; m++) {
			if (score>best_score[m]) {
				best_score[m] = score;
				best_k[m] = k;
				break;
			}
		}
	}
	for (int m = 0; m<M; m++) {
		//printf("score = %f with %d degree\n", best_score[m], best_k[m]);
		h0.rotate_hand(best_k[m], -1, -1, -1, -1, -1);
		//h0.move_hand(ctr.x,ctr.y);

		//---f[0].p
		h0.bend_finger(0, 0, 60, 60, 60);
		best_score[m] = -10000;
		for (int j = 100; j <= 180; j += 10) {
			h0.rotate_hand(-1, j, -1, -1, -1, -1);
			float score = h0.compute_score(contour);
			//printf("f[0].p = %d ==> %f\n", j, score);

			if (score>best_score[m]) {
				best_score[m] = score;
				best_j[m] = j;
			}
		}
		//printf(">> f[0].p = %d\n", best_j[m]);
		h0.rotate_hand(-1, best_j[m], -1, -1, -1, -1);
		//show_img = im_color.clone();
		//h0.draw_hand(title, show_img);
		//waitKey(10);

		//---f[1].p~f[4].p
		//float p_set[6] = {-1, -1, -1, -1, -1, -1};
		float p_set[6] = { -1, -1, 120, 60, 60, 60 };
		for (int i = 2; i <= 5; i++) {
			h0.bend_finger(0, 0, 0, 0, 0);

			best_score[m] = -10000;
			for (int j = 60; j <= 120; j += 10) {
				//printf(">>%d ", j);
				p_set[i] = j;
				h0.rotate_hand(p_set);
				float score = h0.compute_score(contour, -1);
				//printf("f[%d].p = %d ==> %f\n", i-1, j, score);

				if (score>best_score[m]) {
					best_score[m] = score;
					best_j[m] = j;
				}
			}
			//printf(">> f[%d].p = %d\n", i-1, best_j[m]);
			p_set[i] = best_j[m];
			h0.rotate_hand(p_set);
			p_set[i] = -1;

			//show_img = im_color.clone();
			//h0.draw_hand(title, show_img);
			//waitKey(0);
		}

		//---f[0].t~f[4].t
		float t_set[10] = { -1, -1, -1, -1, -1 };
		for (int i = 4; i >= 0; i--) {		// finger number

			best_score[m] = -10000;
			for (int j = 0; j <= 90; j += 18) {
				if (j == 54)	// trick
					continue;
				t_set[i] = j;
				h0.bend_finger(t_set);
				float score = h0.compute_score(contour);
				//printf("f[%d].t%d = %d ==> %f\n", i>>1, i%2, j, score);

				if (score>best_score[m]) {
					best_score[m] = score;
					best_j[m] = j;
				}
			}
			//printf("f[%d].t%d = %d\n", i>>1, i%2, best_j[m]);
			t_set[i] = best_j[m];
			h0.bend_finger(t_set);

			//show_img = im_color.clone();
			//h0.draw_hand(title, show_img);
			//waitKey(10);
		}
		// 임시 보여주기용
		// h0.export_variables(x);
		// h0.move_hand(x[1], x[2]);
		// h0.rotate_hand(x[3], x[4], x[5], x[6], x[7], x[8]);
		// h0.bend_finger(x[9], x[10], x[11], x[12], x[13]);
		// cv::Mat show_img = contours[n].clone();
		// h0.compute_score(contours[n], 1, 1);

		// char title[20];
		// sprintf(title, "abc");
		// h0.draw_hand(title, show_img);
		// waitKey(0);

		if (best_score[m]>best_of_best_score) {
			best_of_best_score = best_score[m];
			h0.export_variables(x);
		}
		//printf("Best score[%d]: %.2f\n", m, best_score[m]);

	}
	//printf("Best of best score[%d]: %.2f\n", frame, best_of_best_score);
	//h0.scale_hand(x[0]);
	h0.move_hand(x[1], x[2]);
	h0.rotate_hand(x[3], x[4], x[5], x[6], x[7], x[8]);
	h0.bend_finger(x[9], x[10], x[11], x[12], x[13]);
	//cout<<"p.p: "<<x[3]<<endl;
	return h0.compute_score(contour, -1, 0);
}
int Gesture::estimate_pose(vector<int> &pose_queue, int curr_pose)
{
	int t0, t1, t234;
	t0 = curr_pose / 10000;
	t1 = (curr_pose / 100) % 100;
	t234 = curr_pose % 100;

	int filtered_pose = POSE_OTHERS;
	//if ( (t0>=50) && (t1>=50) && (t234>=50) )
	if ((t1 >= 50) && (t234 >= 50))
		filtered_pose = POSE_CLOSE;
	if ((t0 >= 50) && (t1 <= 40) && (t234 >= 50))
		filtered_pose = POSE_POINTING;
	if ((t0 <= 40) && (t1 <= 40) && (t234 >= 50))
		filtered_pose = POSE_FINGER_GUN;
	if ((t0 <= 40) && (t1 <= 40) && (t234 <= 40))
		filtered_pose = POSE_OPEN;
	if (curr_pose < 0)
		filtered_pose = POSE_OTHERS;

	pose_queue.erase(pose_queue.begin());
	pose_queue.push_back(filtered_pose);

	float pose_hist[5] = { 0, };
	int pose_set[5] = { POSE_OTHERS, POSE_CLOSE, POSE_POINTING, POSE_FINGER_GUN, POSE_OPEN };
	for (size_t i = 0; i<pose_queue.size(); i++) {
		if (pose_queue[i] == POSE_OTHERS)
			pose_hist[0] += 1 + i*0.01f;
		if (pose_queue[i] == POSE_CLOSE)
			pose_hist[1] += 1 + i*0.01f;
		if (pose_queue[i] == POSE_POINTING)
			pose_hist[2] += 1 + i*0.01f;
		if (pose_queue[i] == POSE_FINGER_GUN)
			pose_hist[3] += 1 + i*0.01f;
		if (pose_queue[i] == POSE_OPEN)
			pose_hist[4] += 1 + i*0.01f;
	}

	for (int i = 0; i<5; i++) {
		if (pose_hist[i] >= 3)
			return pose_set[i];
	}

	return POSE_OTHERS;
}

void Gesture::balance_white(cv::Mat &mat) {
// The MIT License (MIT)
// Copyright (c) 2015 tomykaira
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// reference http://www.ipol.im/pub/art/2011/llmps-scb/
	double discard_ratio = 0.05;
	int hists[3][256];
	memset(hists, 0, 3 * 256 * sizeof(int));

	for (int y = 0; y < mat.rows; ++y) {
		uchar* ptr = mat.ptr<uchar>(y);
		for (int x = 0; x < mat.cols; ++x) {
			for (int j = 0; j < 3; ++j) {
				hists[j][ptr[x * 3 + j]] += 1;
			}
		}
	}

	// cumulative hist
	int total = mat.cols*mat.rows;
	int vmin[3], vmax[3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 255; ++j) {
			hists[i][j + 1] += hists[i][j];
		}
		vmin[i] = 0;
		vmax[i] = 255;
		while (hists[i][vmin[i]] < discard_ratio * total)
			vmin[i] += 1;
		while (hists[i][vmax[i]] > (1 - discard_ratio) * total)
			vmax[i] -= 1;
		if (vmax[i] < 255 - 1)
			vmax[i] += 1;
	}


	for (int y = 0; y < mat.rows; ++y) {
		uchar* ptr = mat.ptr<uchar>(y);
		for (int x = 0; x < mat.cols; ++x) {
			for (int j = 0; j < 3; ++j) {
				int val = ptr[x * 3 + j];
				if (val < vmin[j])
					val = vmin[j];
				if (val > vmax[j])
					val = vmax[j];
				ptr[x * 3 + j] = static_cast<uchar>((val - vmin[j]) * 255.0 / (vmax[j] - vmin[j]));
			}
		}
	}
}