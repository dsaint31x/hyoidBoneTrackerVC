// 4~5���� ���߿� �ص� �˴ϴ�!
// TODO: 1. "Result\���ø�_Variables.txt"�� ����� ������ ������ ������ �̸��� �����θ� ����
// TODO: 2. File load dialog ����
// TODO: 3. �� ������ ���� ǥ��
// TODO: 4. �Ѳ����� ���� ���� ó���� �� �ֵ��� ���� (������ ��� �� ��� Automatic_Grayscale_DataSet_Rotation_mm�� �м�)
// TODO: 5. �ڵ� �� �ּ� ����

#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>   ///getline()
#include <cmath>

/**
* @brief �������� 11���� ������� Hyoid Bone�� �ִ� x, y��ǥ 2���� ������ ��� 
* @date 2017/05/27
* @author �ڼҿ�
*
*
*/
using namespace std;

int main(int argc, char *argv[]) { /// ��� ������Ʈ���� �������ϰ� txt���ϸ� �Է¹���
	string line;
	int frameNum = 0; /// �迭 �����Ҵ��� ���� ���ؾ� �� framenumber
	ifstream file_sample; /// ���� ����
	ofstream file_new; /// ���� ���� �Ǵ� ����
	double timeInterval = 0;  /// ���ϸ��� �ٸ� time interval �ʱ�ȭ

	string uri = argv[1];  /// �Է¹��� txt���ϸ� uri�� �Ҵ�

	file_sample.open(uri);   /// uri�� ����� ���ϸ� open 
	if (file_sample.fail()) { /// ������ ���������� �ҷ� �Դ��� �˻�
		cout << "Failed to load sample file" << endl; /// ���� load�� �����ߴٸ� �ȳ� �޼��� ���
		return 0;
	}
	/** ���ϸ��� �ٸ� framenumber�� time interval ��� */
	for (int i = 0; i < 9; i++) { /// 9���� �ּ� �˻�
		getline(file_sample, line); /// getline() : �Է¹��� stream�� �� line���� �о�� string���� ��ȯ, string line�� ����
		if (strncmp(line.c_str(), "% Total frame number", 20) == 0) { /// strncmp(a, b, c) : a�� b�� c�� ��ŭ�� ���� ��, ��ġ�ϸ� 0 ��ȯ
			stringstream fnst(line); /// c_str() : string�� char�� ��ȯ
			string fncontext;
			for (int j = 0; j < 6; j++) {   /// 6��° �׸��� frame number
				getline(fnst, fncontext, ' '); /// getline(a,b,c) : stream a�� ����c�� ���� �� �� string b�� ����
			}
			frameNum = stoi(fncontext);  /// getline���� ���� �� string�� framenumber�� int�� ��ȯ
		}
		else if (strncmp(line.c_str(), "% Time interval", 8) == 0) { /// framenumber���ϴ� �Ͱ� ����, 8�� �־��� ������ ���� ���ڿ��� ������ ��ġ���� �ʾƵ� �Ǵ°��� ������
		stringstream tist(line);  /// HMMD�� ���ϱ� ���� time interval ���ϱ�
			string ticontext;
			for (int k = 0; k < 5; k++) { /// 5��° �׸��� time interval 
				getline(tist, ticontext, ' ');
			}
			timeInterval = stod(ticontext); /// string�� time interval�� double�� ��ȯ
		}
	}

	/** ������������ framenumber, hyoid bone�� position, displacemenet �迭�� ���� */
	int *fn = new int[frameNum];   /// new, delete�� �̿��� framenumber�� ũ��� �����Ҵ� fn[] : framenumber
	double *h_x = new double[frameNum]; /// h_x[] : hyoid bone�� x��ǥ
	double *h_y = new double[frameNum]; /// h_y[] : hyoid bone�� y��ǥ 
	double *dis = new double[frameNum]; /// dis[] : hyoid bone�� ���������� displacement

	// Calibrate �� �� data
	for (int i = 0; i < frameNum; i++) {
		getline(file_sample, line);    // �������� ������ �� ����ŭ �پ���� �� �ֳ�?
	}

	// Calibrated data
	for (int i = 0; i < frameNum; i++) { /// ó������ ��(framenumber)���� �б�
		getline(file_sample, line); /// CSV ������ ,�� �и��� ó���ϱ� ���� ���� getline���� �� �پ� string line�� ����
		if (strncmp(line.c_str(), "%", 1) == 0) { /// line�� ù ���ڰ� %���, �ּ��̹Ƿ� ������ ���� ������ �ǳʶ�
			i--;
			continue;
		}
		else { /// ����
			stringstream st(line); /// getline���� ,�� �����Ͽ� �������� stream���¿��� �ϹǷ� line�� stream st�� ��ȯ
			string context; /// ,�� �����ڷ� �Ͽ� �� �׸� ������ string context

			for (int j = 0; j < 4; j++) { /// framenumber, h_x, h_y, displacement �� 4��
				getline(st, context, ',');
				switch (j) {
				case 0: /// ù��° �׸��� framenumber �̹Ƿ� fn[]�� ����
					fn[i] = stoi(context); /// string�� int �� ��ȯ
					break;
				case 1: /// �ι�° �׸��� hyoid bone�� x��ǥ �̹Ƿ� h_x[]�� ����
					h_x[i] = stod(context); /// string�� double �� ��ȯ
					break;
				case 2: /// ����° �׸��� hyoid bone�� y��ǥ �̹Ƿ� h_y[]�� ����
					h_y[i] = stod(context);
					break;
				case 3: /// �׹�° �׸��� displacement �̹Ƿ� dis[]�� ����
					dis[i] = stod(context);
					break;
				} /// for���� �������� �� ���� ���� ����(�� ���� string�� getline���� ,�� �����ڷ��Ͽ� ������ �迭�� ����)�� framenumber��ŭ �ݺ�
			}
		}
	}

	/** �������� ������ �� ���� */
	double HMD;///hyoid movement duration = the resting position ~ Burst end position
	double HMP_X;///hyoid maximum posiition x-axis  , HMP�� ������ ����
	double HMP_Y;///hyoid maximum posiition y-axis
	double HMP_XY;///hyoid maximum posiition x-axis, y-axis = burst end position

	double HMD_A;///hyoid maximum displacement Anterior , HMD�� resting position�� ����
	double HMD_S;///hyoid maximum displacement superior
	double HMD_H;///hyoid maximum displacement hypotenuse

	double HAV_A;///hyoid average velocity anterior = HMD_A / HMD 
	double HAV_S;///hyoid average velocity superior = HMD_S / HMD
	double HAV_H;///hyoid average velocity hypotenuse = HMD_H / HMD

	double MAX_V;///hyoid maximum velocity(velocity of each frame), �����Ӻ� ������ �ִ� �ӵ��� ���ϱ� ���� vofh[]�迭 ����

	/**
	* resting position�� ù �������� hyoid bone�� ��ġ���� ���� �Ÿ��� ��� ������ ���� ���������� ����������
	* �켱�� ù �������� resting position�̶� ����
	*/
	double restingPos; /// resting position 
	int rest_frameNum; /// resting position�� framenumber
	restingPos = h_y[0]; 
	rest_frameNum = fn[0];

	/** Burst End Positon = Maximum displacement ���ϱ� */
	double burstEnd;  //to get the Burst end position
	int max_frameNum;
	burstEnd = dis[0]; /// �ִ밪 ���ϱ�
	for (int i = 0; i < frameNum; i++) {   
		if (burstEnd < dis[i]) {
			burstEnd = dis[i];
			max_frameNum = fn[i];
		}
		else {
			continue;
		}
	}
	/** maximum anterior displacement = HMD_A ���ϱ� */
	double max_x; 
	int max_frameNum2;
	max_x = h_x[0];
	for (int i = 0; i < frameNum; i++) {
		if (max_x < h_x[i]) {
			max_x = h_x[i];
			max_frameNum2 = fn[i];
		}
		else {
			continue;
		}
	}
	/** maximum superior displacement = HMD_S ���ϱ� */
	double max_y;  
	int max_frameNum3;
	max_y = h_y[0];
	for (int i = 0; i < frameNum; i++) {
		if (max_y < h_y[i]) {
			max_y = h_y[i];
			max_frameNum3 = fn[i];
		}
		else {
			continue;
		}
	}

	/** ���� ��� ���� */
	HMD = abs(max_frameNum - rest_frameNum) * timeInterval;

	HMP_X = h_x[max_frameNum - 1]; /// max_framenumber�� ����� ���� index�� �ƴ� framenumber�̹Ƿ� -1
	HMP_Y = h_y[max_frameNum - 1];
	HMP_XY = burstEnd;  //Burst end position

	HMD_A = h_x[max_frameNum - 1] - h_x[rest_frameNum - 1];
	HMD_S = h_y[max_frameNum - 1] - restingPos;  // = max_y - h_y[rest_frameNum]
	HMD_H = sqrt(pow(HMP_X - h_x[rest_frameNum - 1], 2) + (pow(HMP_Y - h_y[rest_frameNum - 1], 2)));

	HAV_A = HMD_A / HMD;
	HAV_S = HMD_S / HMD;
	HAV_H = HMD_H / HMD;

	/** MAX�� ���ϱ� ���� �� ������ �� hyoid bone�� �ӵ� �迭�� ���� */
	double * vofh = new double[frameNum];   //
	for (int i = 0; i < frameNum; i++) {
		if (i < (frameNum - 1)) {
			vofh[i] = sqrt(pow(h_x[i + 1] - h_x[i], 2) + pow(h_y[i + 1] - h_y[i], 2)) / timeInterval; 
		}
		else {
			vofh[i] = -9999;
		}
	}

	/** MAX�� ���ϱ� ���� �� ������ �� hyoid bone �ӵ��� �ִ밪 ���ϱ� */
	int max_frameNumV;
	MAX_V = vofh[0];
	for (int i = 0; i < frameNum; i++) {
		if (MAX_V < dis[i]) {
			MAX_V = dis[i];
			max_frameNumV = fn[i];
		}
		else {
			continue;
		}
	}
	/** ���ο� ���� ���� */
	file_new.open("Variables.txt");
	if (!file_new.is_open()) {     
		cout << "open() error" << endl;
		return -1;
	}
	ifstream file_sample2;
	string line2;
	file_sample2.open(uri);     
	if (file_sample2.fail()) {
		cout << "Failed to load sample file" << endl;
		return 0;
	}
	for (int i = 0; i < 8; i++) {/// ������ �ּ� �״�� ���� 
		getline(file_sample2, line2);
		file_new << line2 << endl;
	}
	file_new << "% Resting Position Frame number : " << rest_frameNum << endl; /// ���� �ּ� �ؿ� Resting Position Frame number �ּ����� �߰�
	file_new << "% Burst End Position Frame number : " << max_frameNum << endl;  /// ���� �ּ� �ؿ� Burst End Position Frame number �ּ����� �߰�
	// file_new << "% Data order : Frame number / Velocity of HyoidBone / velocity of Hyoid Bone (x-axis) / average velocity of 3 Hyoid Bone (x-axis)" << endl;
	file_new << "1,Hyoid Movement Duration," << HMD << endl;
	file_new << "2,Hyoid Maximum Position (X)," << HMP_X << endl;
	file_new << "3,Hyoid Maximum Position (Y)," << HMP_Y << endl;
	file_new << "4,Hyoid Maximum Position (XY)," << HMP_XY << endl;
	file_new << "4.1,Hyoid Maximum position (X-axis)," << max_x << endl;
	file_new << "4.2,Hyoid Maximum position (Y-axis)," << max_y << endl;
	file_new << "5,Hyoid Maximum Displacement (Anterior)," << HMD_A << endl;
	file_new << "6,Hyoid Maximum Displacement (Superior)," << HMD_S << endl;
	file_new << "7,Hyoid Maximum Displacement (Hypotenuse)," << HMD_H << endl;
	file_new << "8,Hyoid Average Velocity (Anterior)," << HAV_A << endl;
	file_new << "9,Hyoid Average Velocity (Superior)," << HAV_S << endl;
	file_new << "10,Hyoid Average Velocity (Hypotenuse)," << HAV_H << endl;
	file_new << "11,Hyoid Maximum Velocity = Max (Velocity_of_eachframe)," << MAX_V << endl;

	file_new.close();

	delete[] fn; ///�����Ҵ� �޸� ����
	delete[] h_x;
	delete[] h_y;
	delete[] dis;
	delete[] vofh;

	return 0;
}

/*kkr first commit*/