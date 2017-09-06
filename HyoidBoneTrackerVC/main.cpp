// 4~5번은 나중에 해도 됩니다!
// TODO: 1. "Result\샘플명_Variables.txt"로 계산한 변수를 저장할 파일의 이름과 저장경로를 정정
// TODO: 2. File load dialog 띄우기
// TODO: 3. 각 변수의 단위 표시
// TODO: 4. 한꺼번에 여러 파일 처리할 수 있도록 수정 (지정한 경로 내 모든 Automatic_Grayscale_DataSet_Rotation_mm를 분석)
// TODO: 5. 코드 및 주석 정리

#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>   ///getline()
#include <cmath>

/**
* @brief 재정립된 11개의 변수들과 Hyoid Bone의 최대 x, y좌표 2개의 변수를 출력 
* @date 2017/05/27
* @author 박소연
*
*
*/
using namespace std;

int main(int argc, char *argv[]) { /// 명령 프롬프트에서 실행파일과 txt파일명 입력받음
	string line;
	int frameNum = 0; /// 배열 동적할당을 위해 구해야 할 framenumber
	ifstream file_sample; /// 원본 파일
	ofstream file_new; /// 새로 생성 되는 파일
	double timeInterval = 0;  /// 파일마다 다른 time interval 초기화

	string uri = argv[1];  /// 입력받은 txt파일명 uri에 할당

	file_sample.open(uri);   /// uri에 저장된 파일명 open 
	if (file_sample.fail()) { /// 파일을 성공적으로 불러 왔는지 검사
		cout << "Failed to load sample file" << endl; /// 파일 load에 실패했다면 안내 메세지 출력
		return 0;
	}
	/** 파일마다 다른 framenumber와 time interval 얻기 */
	for (int i = 0; i < 9; i++) { /// 9개의 주석 검사
		getline(file_sample, line); /// getline() : 입력받은 stream을 한 line마다 읽어와 string으로 반환, string line에 저장
		if (strncmp(line.c_str(), "% Total frame number", 20) == 0) { /// strncmp(a, b, c) : a와 b를 c개 만큼의 문자 비교, 일치하면 0 반환
			stringstream fnst(line); /// c_str() : string을 char로 변환
			string fncontext;
			for (int j = 0; j < 6; j++) {   /// 6번째 항목이 frame number
				getline(fnst, fncontext, ' '); /// getline(a,b,c) : stream a를 문자c를 제외 한 후 string b로 저장
			}
			frameNum = stoi(fncontext);  /// getline으로 저장 된 string의 framenumber를 int로 변환
		}
		else if (strncmp(line.c_str(), "% Time interval", 8) == 0) { /// framenumber구하는 것과 동일, 8로 넣어준 이유는 비교할 문자열과 갯수가 일치하지 않아도 되는것을 보려고
		stringstream tist(line);  /// HMMD를 구하기 위한 time interval 구하기
			string ticontext;
			for (int k = 0; k < 5; k++) { /// 5번째 항목이 time interval 
				getline(tist, ticontext, ' ');
			}
			timeInterval = stod(ticontext); /// string인 time interval을 double로 변환
		}
	}

	/** 원본데이터의 framenumber, hyoid bone의 position, displacemenet 배열에 저장 */
	int *fn = new int[frameNum];   /// new, delete를 이용해 framenumber의 크기로 동적할당 fn[] : framenumber
	double *h_x = new double[frameNum]; /// h_x[] : hyoid bone의 x좌표
	double *h_y = new double[frameNum]; /// h_y[] : hyoid bone의 y좌표 
	double *dis = new double[frameNum]; /// dis[] : hyoid bone의 원점에서의 displacement

	// Calibrate 안 된 data
	for (int i = 0; i < frameNum; i++) {
		getline(file_sample, line);    // 읽지말고 지정한 줄 수만큼 뛰어넘을 수 있나?
	}

	// Calibrated data
	for (int i = 0; i < frameNum; i++) { /// 처음부터 끝(framenumber)까지 읽기
		getline(file_sample, line); /// CSV 파일을 ,로 분리해 처리하기 위해 먼저 getline으로 한 줄씩 string line에 저장
		if (strncmp(line.c_str(), "%", 1) == 0) { /// line의 첫 문자가 %라면, 주석이므로 본문이 나올 때까지 건너뜀
			i--;
			continue;
		}
		else { /// 본문
			stringstream st(line); /// getline으로 ,를 제외하여 얻어오려면 stream형태여야 하므로 line을 stream st로 변환
			string context; /// ,를 구분자로 하여 얻어낼 항목 저장할 string context

			for (int j = 0; j < 4; j++) { /// framenumber, h_x, h_y, displacement 총 4개
				getline(st, context, ',');
				switch (j) {
				case 0: /// 첫번째 항목은 framenumber 이므로 fn[]에 저장
					fn[i] = stoi(context); /// string을 int 로 변환
					break;
				case 1: /// 두번째 항목은 hyoid bone의 x좌표 이므로 h_x[]에 저장
					h_x[i] = stod(context); /// string을 double 로 변환
					break;
				case 2: /// 세번째 항목은 hyoid bone의 y좌표 이므로 h_y[]에 저장
					h_y[i] = stod(context);
					break;
				case 3: /// 네번째 항목은 displacement 이므로 dis[]에 저장
					dis[i] = stod(context);
					break;
				} /// for문을 빠져나간 후 위와 같은 과정(한 줄의 string을 getline으로 ,를 구분자로하여 각각의 배열에 저장)을 framenumber만큼 반복
			}
		}
	}

	/** 재정립된 변수들 및 정의 */
	double HMD;///hyoid movement duration = the resting position ~ Burst end position
	double HMP_X;///hyoid maximum posiition x-axis  , HMP는 원점이 기준
	double HMP_Y;///hyoid maximum posiition y-axis
	double HMP_XY;///hyoid maximum posiition x-axis, y-axis = burst end position

	double HMD_A;///hyoid maximum displacement Anterior , HMD는 resting position이 기준
	double HMD_S;///hyoid maximum displacement superior
	double HMD_H;///hyoid maximum displacement hypotenuse

	double HAV_A;///hyoid average velocity anterior = HMD_A / HMD 
	double HAV_S;///hyoid average velocity superior = HMD_S / HMD
	double HAV_H;///hyoid average velocity hypotenuse = HMD_H / HMD

	double MAX_V;///hyoid maximum velocity(velocity of each frame), 프레임별 움직인 최대 속도를 구하기 위해 vofh[]배열 생성

	/**
	* resting position은 첫 프레임의 hyoid bone의 위치에서 일정 거리를 벗어난 지점의 직전 프레임으로 설정하지만
	* 우선은 첫 프레임을 resting position이라 설정
	*/
	double restingPos; /// resting position 
	int rest_frameNum; /// resting position의 framenumber
	restingPos = h_y[0]; 
	rest_frameNum = fn[0];

	/** Burst End Positon = Maximum displacement 구하기 */
	double burstEnd;  //to get the Burst end position
	int max_frameNum;
	burstEnd = dis[0]; /// 최대값 구하기
	for (int i = 0; i < frameNum; i++) {   
		if (burstEnd < dis[i]) {
			burstEnd = dis[i];
			max_frameNum = fn[i];
		}
		else {
			continue;
		}
	}
	/** maximum anterior displacement = HMD_A 구하기 */
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
	/** maximum superior displacement = HMD_S 구하기 */
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

	/** 변수 계산 과정 */
	HMD = abs(max_frameNum - rest_frameNum) * timeInterval;

	HMP_X = h_x[max_frameNum - 1]; /// max_framenumber에 저장된 값은 index가 아닌 framenumber이므로 -1
	HMP_Y = h_y[max_frameNum - 1];
	HMP_XY = burstEnd;  //Burst end position

	HMD_A = h_x[max_frameNum - 1] - h_x[rest_frameNum - 1];
	HMD_S = h_y[max_frameNum - 1] - restingPos;  // = max_y - h_y[rest_frameNum]
	HMD_H = sqrt(pow(HMP_X - h_x[rest_frameNum - 1], 2) + (pow(HMP_Y - h_y[rest_frameNum - 1], 2)));

	HAV_A = HMD_A / HMD;
	HAV_S = HMD_S / HMD;
	HAV_H = HMD_H / HMD;

	/** MAX값 구하기 위해 각 프레임 간 hyoid bone의 속도 배열에 저장 */
	double * vofh = new double[frameNum];   //
	for (int i = 0; i < frameNum; i++) {
		if (i < (frameNum - 1)) {
			vofh[i] = sqrt(pow(h_x[i + 1] - h_x[i], 2) + pow(h_y[i + 1] - h_y[i], 2)) / timeInterval; 
		}
		else {
			vofh[i] = -9999;
		}
	}

	/** MAX값 구하기 위해 각 프레임 간 hyoid bone 속도의 최대값 구하기 */
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
	/** 새로운 파일 생성 */
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
	for (int i = 0; i < 8; i++) {/// 원본의 주석 그대로 복사 
		getline(file_sample2, line2);
		file_new << line2 << endl;
	}
	file_new << "% Resting Position Frame number : " << rest_frameNum << endl; /// 원본 주석 밑에 Resting Position Frame number 주석으로 추가
	file_new << "% Burst End Position Frame number : " << max_frameNum << endl;  /// 원본 주석 밑에 Burst End Position Frame number 주석으로 추가
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

	delete[] fn; ///동적할당 메모리 해제
	delete[] h_x;
	delete[] h_y;
	delete[] dis;
	delete[] vofh;

	return 0;
}

/*kkr first commit*/