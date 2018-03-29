#pragma once

#include <math.h>
#define L 4
#define M 3
#define N 1
#define π 3.1415926

struct MatrxData
{
	double point1_2[3][1];//坐标系1中坐标系2的原点
	double point2_1[3][1];//坐标系2中坐标系1的原点
	double point3_2[3][1];//坐标系3中坐标系2的原点

	double BaseVector1_2[3][3];//坐标系1中坐标系2的基向量
	double BaseVector1_3[3][3];//坐标系1中坐标系3的基向量

	double BaseVector2_1[3][3];//坐标系2中坐标系1的基向量
	double BaseVector2_3[3][3];//坐标系2中坐标系3的基向量
	
	double BaseVector3_2[3][3];//坐标系3中坐标系2的基向量
	double BaseVector3_1[3][3];//坐标系3中坐标系1的基向量

	double BaseVector1_2_θ[3][3];//360°旋转矩阵	
	
	//double I[3][3];//单位矩阵
	//int pitchangle;//pitch角度数值
	//double BaseVector1_2_pitch[3][3];//pitch角矩阵
	//double xyzoffset[3];//系统给出的需平移量，基于TTS坐标系（坐标系4）
	//double corrvector[6];//六维度校正矩阵值
};

class Translate {

public:
	Translate()
	{
		memset(&m_m, 0, sizeof(MatrxData));
		memset(&m_m2, 0, sizeof(MatrxData));
	};

	MatrxData m_m;
	MatrxData m_m2;
	double A[M][M];
	double B[M][M];
	double C[M][N];
	double D[M][N];

	double I[3][3];//单位矩阵
	double pitchangle;//pitch角度数值
	double BaseVector1_2_pitch[3][3];//pitch角矩阵
	double BaseVector1_2_roll[3][3];//roll角矩阵
	double BaseVector1_2_iso[3][3];//iso角矩阵
	double xyzoffset[3];//系统给出的需平移量，基于TTS坐标系（坐标系4）
	double corrvector[6];//六维度校正矩阵值

	//矩阵的乘法33*33
	void mulMatri33(double x[M][M], double y[M][M], double z[M][M])
	{
		int i, j, k;
		for (i = 0; i < M; i++)//防止数据空间串扰
		{
			for (j = 0; j < M; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < M; i++)
		{
			for (j = 0; j < M; j++)
			{
				z[i][j] = 0;
				for (k = 0; k < M; k++)
				{
					z[i][j] += A[i][k] * B[k][j];
				}
			}
		}
	};
	//矩阵的乘法33*31
	void mulMatri31(double x[M][M], double y[M][N], double z[M][N])
	{
		int i, k;
		for (i = 0; i < M; i++)//防止数据空间串扰
		{
			C[i][0] = y[i][0];
			for (k = 0; k < M; k++)
			{
				A[i][k] = x[i][k];
			}
		}
		for (i = 0; i < M; i++)
		{
				z[i][0] = 0;
				for (k = 0; k < M; k++)
				{
					z[i][0] += A[i][k] * C[k][0];
				}
		}
	};
	//矩阵减法31-31
	void subMatri31(double x[M][N], double y[M][N], double z[M][N])
	{
		int i;
		for (i = 0; i < M; i++)//防止数据空间串扰
		{
				C[i][0] = x[i][0];
				D[i][0] = y[i][0];

		}
		for (i = 0; i < M; i++)
		{
				z[i][0] = C[i][0] - D[i][0];
		}
	};
	//矩阵减法33-33
	void subMatri33(double x[M][M], double y[M][M], double z[M][M])
	{
		int i, j;

		for (i = 0; i < M; i++)//防止数据空间串扰
		{
			for (j = 0; j < M; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < M; j++)
			{
				z[i][j] = A[i][j] - B[i][j];
			}
		}
	};
	//矩阵加法31+31
	void addMatri31(double x[M][N], double y[M][N], double z[M][N])
	{
		int i;
		for (i = 0; i < M; i++)//防止数据空间串扰
		{
			C[i][0] = x[i][0];
			D[i][0] = y[i][0];

		}
		for (i = 0; i < M; i++)
		{
			z[i][0] = C[i][0] + D[i][0];
		}
	};
	//矩阵加法33+33
	void addMatri33(double x[M][M], double y[M][M], double z[M][M])
	{
		int i, j;
		for (i = 0; i < M; i++)//防止数据空间串扰
		{
			for (j = 0; j < M; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}
		for (i = 0; i < M; i++)
		{
			for (j = 0; j < M; j++)
			{
				z[i][j] = A[i][j] + B[i][j];
			}
		}
	};

	//按第一行展开计算|A|
	double getA(double arcs[M][M], int n)
	{
		if (n == 1)
		{
			return arcs[0][0];
		}
		double ans = 0;
		double temp[M][M] = { 0.0 };
		int i, j, k;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n - 1; j++)
			{
				for (k = 0; k < n - 1; k++)
				{
					temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];

				}
			}
			double t = getA(temp, n - 1);
			if (i % 2 == 0)
			{
				ans += arcs[0][i] * t;
			}
			else
			{
				ans -= arcs[0][i] * t;
			}
		}
		return ans;
	};

	//计算每一行每一列的每个元素所对应的余子式，组成A*
	void  getAStart(double arcs[M][M], int n, double ans[M][M])
	{
		if (n == 1)
		{
			ans[0][0] = 1;
			return;
		}
		int i, j, k, t;
		double temp[M][M];
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				for (k = 0; k < n - 1; k++)
				{
					for (t = 0; t < n - 1; t++)
					{
						temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
					}
				}


				ans[j][i] = getA(temp, n - 1);  //此处顺便进行了转置
				if ((i + j) % 2 == 1)
				{
					ans[j][i] = -ans[j][i];
				}
			}
		}
	};

	//得到给定矩阵src的逆矩阵保存到des中。逆矩阵的求解
	bool GetMatrixInverse(double src[M][M], int n, double des[M][M])
	{
		double flag = getA(src, n);
		double t[M][M];
		if (0 == flag)
		{
			//cout << "原矩阵行列式为0，无法求逆。请重新运行" << endl;
			return false;//如果算出矩阵的行列式为0，则不往下进行
		}
		else
		{
			getAStart(src, n, t);
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					des[i][j] = t[i][j] / flag;
				}

			}
		}

		return true;
	};
	//为矩阵赋值
	void INITMatri(int data[9], int Vector[M][N])
	{
		Vector[0][0] = data[0];
		Vector[1][0] = data[1];
		Vector[2][0] = data[2];
		Vector[0][1] = data[3];
		Vector[1][1] = data[4];
		Vector[2][1] = data[5];
		Vector[0][2] = data[6];
		Vector[1][2] = data[7];
		Vector[2][2] = data[8];
	};
	//构造绕Z轴的旋转矩阵
	void INITZMatri(double Zangle, double Vector[M][M])
	{
		Vector[0][0] = cos(Zangle / 180 * π);
		Vector[1][0] = sin(Zangle / 180 * π);
		Vector[2][0] = 0;
		Vector[0][1] = -sin(Zangle / 180 * π);
		Vector[1][1] = cos(Zangle / 180 * π);
		Vector[2][1] = 0;
		Vector[0][2] = 0;
		Vector[1][2] = 0;
		Vector[2][2] = 1;
	};

	//构造绕Y轴的旋转矩阵
	void INITYMatri(double Yangle, double Vector[M][M])
	{
		Vector[0][0] = cos(Yangle / 180 * π);
		Vector[1][0] = 0;
		Vector[2][0] = -sin(Yangle / 180 * π);
		Vector[0][1] = 0;
		Vector[1][1] = 1;
		Vector[2][1] = 0;
		Vector[0][2] = sin(Yangle / 180 * π);
		Vector[1][2] = 0;
		Vector[2][2] = cos(Yangle / 180 * π);
	};

	//构造绕X轴的旋转矩阵,PITCH用
	void INITXMatri(double Xangle, double Vector[M][M])
	{
		Vector[0][0] = 1;
		Vector[1][0] = 0;
		Vector[2][0] = 0;
		Vector[0][1] = 0;
		Vector[1][1] = cos(Xangle / 180 * π);
		Vector[2][1] = sin(Xangle / 180 * π);
		Vector[0][2] = 0;
		Vector[1][2] = -sin(Xangle / 180 * π);
		Vector[2][2] = cos(Xangle / 180 * π);
	};

	void init()
	{
		m_m.point1_2[0][0] = 0;// -3;
		m_m.point1_2[1][0] = 0;// 0.5;
		m_m.point1_2[2][0] = -1026.5;


		m_m.BaseVector1_2[0][0] = 1;// 0.954;
		m_m.BaseVector1_2[1][0] = 0;
		m_m.BaseVector1_2[2][0] = 0;// 0.3;
		m_m.BaseVector1_2[0][1] = 0;
		m_m.BaseVector1_2[1][1] = 1;//0.995
		m_m.BaseVector1_2[2][1] = 0;//0.1
		m_m.BaseVector1_2[0][2] = 0;// -0.3;
		m_m.BaseVector1_2[1][2] = 0;
		m_m.BaseVector1_2[2][2] = 1;// 0.954;

		m_m.BaseVector1_3[0][0] = 1;
		m_m.BaseVector1_3[1][0] = 0;
		m_m.BaseVector1_3[2][0] = 0;
		m_m.BaseVector1_3[0][1] = 0;
		m_m.BaseVector1_3[1][1] = 1;
		m_m.BaseVector1_3[2][1] = 0;
		m_m.BaseVector1_3[0][2] = 0;
		m_m.BaseVector1_3[1][2] = 0;
		m_m.BaseVector1_3[2][2] = 1;

		I[0][0] = 1;
		I[1][0] = 0;
		I[2][0] = 0;
		I[0][1] = 0;
		I[1][1] = 1;
		I[2][1] = 0;
		I[0][2] = 0;
		I[1][2] = 0;
		I[2][2] = 1;
		//INITXMatri(m_m2.pitchangle, m_m2.BaseVector1_2_pitch);
		//mulMatri33(m_m2.BaseVector1_2_pitch, m_m2.BaseVector1_5, m_m2.BaseVector1_4);//构造table坐标系
		m_m2.point1_2[0][0] = 0;// -3;
		m_m2.point1_2[1][0] = 0;// 0.5;
		m_m2.point1_2[2][0] = -1028;

		//m_m2.point2_1[0][0] = 3;
		//m_m2.point2_1[1][0] = -0.5;
		//m_m2.point2_1[2][0] = 1028;

		//m_m2.BaseVector1_2[0][0] = 0.979795897;
		//m_m2.BaseVector1_2[1][0] = 0;
		//m_m2.BaseVector1_2[2][0] = 0.2;
		//m_m2.BaseVector1_2[0][1] = 0;
		//m_m2.BaseVector1_2[1][1] = 1;//0.995
		//m_m2.BaseVector1_2[2][1] = 0;//0.1
		//m_m2.BaseVector1_2[0][2] = -0.2;
		//m_m2.BaseVector1_2[1][2] = 0;
		//m_m2.BaseVector1_2[2][2] = 0.979795897;

		//m_m2.BaseVector2_1[0][0] = 0.979795897;
		//m_m2.BaseVector2_1[1][0] = 0;
		//m_m2.BaseVector2_1[2][0] = -0.2;
		//m_m2.BaseVector2_1[0][1] = 0;
		//m_m2.BaseVector2_1[1][1] = 1;//0.995
		//m_m2.BaseVector2_1[2][1] = 0;//0.1
		//m_m2.BaseVector2_1[0][2] = 0.2;
		//m_m2.BaseVector2_1[1][2] = 0;
		//m_m2.BaseVector2_1[2][2] = 0.979795897;

		m_m2.BaseVector1_2[0][0] = 1;//0.953939201416946;
		m_m2.BaseVector1_2[0][1] = 0;
		m_m2.BaseVector1_2[0][2] = 0;// 0.3;
		m_m2.BaseVector1_2[1][0] = 0;
		m_m2.BaseVector1_2[1][1] = 1;
		m_m2.BaseVector1_2[1][2] = 0;
		m_m2.BaseVector1_2[2][0] = 0;// -0.3;
		m_m2.BaseVector1_2[2][1] = 0;
		m_m2.BaseVector1_2[2][2] = 1;// 0.953939201416946;

		m_m2.BaseVector2_1[0][0] = 1;// 0.953939201416946;
		m_m2.BaseVector2_1[0][1] = 0;
		m_m2.BaseVector2_1[0][2] = 0;// -0.3;
		m_m2.BaseVector2_1[1][0] = 0;
		m_m2.BaseVector2_1[1][1] = 1;//0.995
		m_m2.BaseVector2_1[1][2] = 0;//0.1
		m_m2.BaseVector2_1[2][0] = 0;// 0.3;
		m_m2.BaseVector2_1[2][1] = 0;
		m_m2.BaseVector2_1[2][2] = 1;// 0.953939201416946;
		mulMatri31(m_m2.BaseVector1_2, m_m2.point1_2, m_m2.point2_1);
		m_m2.point2_1[0][0] = -m_m2.point2_1[0][0];
		m_m2.point2_1[1][0] = -m_m2.point2_1[1][0];
		m_m2.point2_1[2][0] = -m_m2.point2_1[2][0];

		m_m2.BaseVector1_3[0][0] = 1;
		m_m2.BaseVector1_3[1][0] = 0;
		m_m2.BaseVector1_3[2][0] = 0;
		m_m2.BaseVector1_3[0][1] = 0;
		m_m2.BaseVector1_3[1][1] = 1;
		m_m2.BaseVector1_3[2][1] = 0;
		m_m2.BaseVector1_3[0][2] = 0;
		m_m2.BaseVector1_3[1][2] = 0;
		m_m2.BaseVector1_3[2][2] = 1;

		mulMatri33(m_m2.BaseVector1_2, m_m2.BaseVector1_3, m_m2.BaseVector2_3);//在坐标系2中的表示坐标系3的基向量
		//GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆
	};

	void UpDataMatrix(double tableangle,double x,double y,double z)//(在360°平台角度改变后,或者出现平移使得坐标系2的原点变化以后，在坐标系2中表示其他的坐标系;或者平台角度未改变，也要算出相应的矩阵数据
	{
		INITZMatri(tableangle, m_m2.BaseVector1_2_θ);//构建360°z轴旋转角矩阵
		//INITXMatri(m_m2.pitchangle, m_m2.BaseVector1_2_pitch);//构造pitch角矩阵
		mulMatri33(m_m2.BaseVector1_2_θ, m_m2.BaseVector1_2, m_m2.BaseVector1_2);//旋转后，在坐标系1中的表示坐标系2的基向量
		GetMatrixInverse(m_m2.BaseVector1_2, 3,m_m2.BaseVector2_1);//求逆

		mulMatri33(m_m2.BaseVector1_2_θ, m_m2.BaseVector1_3, m_m2.BaseVector1_3);//旋转后，在坐标系1中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector1_3, 3, m_m2.BaseVector3_1);//求逆

		//更新坐标系原点数据坐标
		m_m2.point1_2[0][0] = m_m.point1_2[0][0] + x;
		m_m2.point1_2[1][0] = m_m.point1_2[1][0] + y;
		m_m2.point1_2[2][0] = m_m.point1_2[2][0] + z;
		mulMatri31(m_m2.BaseVector1_2, m_m2.point1_2, m_m2.point2_1);
		m_m2.point2_1[0][0] = -m_m2.point2_1[0][0];
		m_m2.point2_1[1][0] = -m_m2.point2_1[1][0];
		m_m2.point2_1[2][0] = -m_m2.point2_1[2][0];
		mulMatri31(m_m2.BaseVector2_3, m_m2.point2_1, m_m2.point3_2);
		m_m2.point3_2[0][0] = -m_m2.point3_2[0][0];
		m_m2.point3_2[1][0] = -m_m2.point3_2[1][0];
		m_m2.point3_2[2][0] = -m_m2.point3_2[2][0];
	};

	void DoPitch(double pitch,double Vector[6])
	{
		pitchangle = pitch;
		INITXMatri(pitchangle, BaseVector1_2_pitch);//构造pitch角矩阵
		mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//pitch后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
		//UpDataMatrix(0);
		double num[3][3];
		double num2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1,2中的坐标
		double B1[3][1], B2[3][1], _A2[3][1];//点B在坐标系1,2中的坐标
		A1[0][0] = 1;
		A1[1][0] = 1;
		A1[2][0] = 1;
		mulMatri31(BaseVector1_2_pitch, A1, B1);//获得点B在坐标系1中的坐标
		mulMatri31(BaseVector1_2_pitch, A2, _A2);//获得点B在坐标系1中的坐标
		mulMatri31(m_m2.BaseVector1_2, A1, A2);
		mulMatri31(m_m2.BaseVector1_2, B1, B2);//获得A、B两点在坐标系2中的坐标值

		subMatri33(BaseVector1_2_pitch, I, num);
		//mulMatri33(m_m2.BaseVector3_2, num, num);
		mulMatri31(num, m_m2.point1_2, num2);//解出平移
		Vector[3] = num2[0][0];
		Vector[4] = num2[1][0];
		Vector[5] = num2[2][0];

		m_m2.point1_2[0][0] += num2[0][0];
		m_m2.point1_2[1][0] += num2[1][0];
		m_m2.point1_2[2][0] += num2[2][0];

		//（1）	点A绕坐标系2的x轴旋转α角，使A运动到Y=0平面上
		Vector[0] = asin(_A2[1][0] / sqrt(_A2[1][0] * _A2[1][0] + _A2[2][0] * _A2[2][0])) / π * 180;
		double AA[3][1], RA[3][3];
		INITXMatri(Vector[0], RA);
		mulMatri31(RA, _A2, AA);
		//然后绕Y轴转动β角度，使之转动到和目标位点B相同的Z坐标
		double a, b, c;
		a = B2[2][0], b = -AA[0][0]; c = AA[2][0];
		Vector[1] = acos((2 * a*c + sqrt(pow(2 * c*b, 2) + 4 * pow(b, 4) - 4 * pow(a*b, 2))) / (2 * c*c + 2 * b*b)) / π * 180;
		double AAA[3][1], RAA[3][3];
		INITYMatri(Vector[1], RAA);
		mulMatri31(RAA, AA, AAA);
		//
		a = B2[1][0], b = AAA[0][0]; c = -AAA[2][0];
		Vector[2] = acos((2 * a*c + sqrt(pow(2 * c*b, 2) + 4 * pow(b, 4) - 4 * pow(a*b, 2))) / (2 * c*c + 2 * b*b)) / π * 180;

	};

	void DoPitch2(double pitch, double Vector[6])
	{
		pitchangle = pitch;
		INITXMatri(pitchangle, BaseVector1_2_pitch);
		mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在pitch后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
																	//UpDataMatrix(0);
		mulMatri33(BaseVector1_2_pitch, I, m_m2.BaseVector1_3);//在坐标系1中构造table坐标系（坐标系3）

		double num[3][3];
		double num2[3][1];
		double numA[3][1], numA2[3][1];
		double numB[3][1], numB2[3][1];
		double numC[3][1], numC2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1中的坐标
		double B1[3][1], B2[3][1];//点B在坐标系1中的坐标
		double C1[3][1], C2[3][1];

		A1[0][0] = 10; A1[1][0] = 20; A1[2][0] = 30;
		B1[0][0] = 100; B1[1][0] = 50; B1[2][0] = 87;
		C1[0][0] = 1; C1[1][0] = 1; C1[2][0] = 1;

		subMatri33(BaseVector1_2_pitch, I, num);
		//mulMatri33(m_m2.BaseVector3_2, num, num);
		mulMatri31(num, m_m2.point1_2, num2);//解出平移
		mulMatri31(m_m2.BaseVector1_2, num2, num2);
		Vector[3] = num2[0][0];
		Vector[4] = num2[1][0];
		Vector[5] = num2[2][0];

		//m_m2.point1_2[0][0] = +num2[0][0];
		//m_m2.point1_2[1][0] = +num2[1][0];
		//m_m2.point1_2[2][0] = +num2[2][0];
		mulMatri33(m_m2.BaseVector1_2, BaseVector1_2_pitch, num);
		mulMatri31(num, A1, numA); mulMatri31(num, B1, numB); mulMatri31(num, C1, numC);
		addMatri31(numA, m_m2.point2_1, numA); addMatri31(numB, m_m2.point2_1, numB); addMatri31(numC, m_m2.point2_1, numC);
		subMatri31(numA, num2, numA); subMatri31(numB, num2, numB); subMatri31(numC, num2, numC);

		mulMatri31(m_m2.BaseVector1_2, A1, numA2); addMatri31(numA2, m_m2.point2_1, numA2);
		mulMatri31(m_m2.BaseVector1_2, B1, numB2); addMatri31(numB2, m_m2.point2_1, numB2);
		mulMatri31(m_m2.BaseVector1_2, C1, numC2); addMatri31(numC2, m_m2.point2_1, numC2);


		double VECTOR[3][3]; double VECTOR2[3], VECTOR3[3];
		VECTOR[0][0] = numA2[0][0]; VECTOR[0][1] = numA2[1][0]; VECTOR[0][2] = numA2[2][0];
		VECTOR[1][0] = numB2[0][0]; VECTOR[1][1] = numB2[1][0]; VECTOR[1][2] = numB2[2][0];
		VECTOR[2][0] = numC2[0][0]; VECTOR[2][1] = numC2[1][0]; VECTOR[2][2] = numC2[2][0];
		VECTOR2[0] = numA[2][0]; VECTOR2[1] = numB[2][0]; VECTOR2[2] = numC[2][0];
		solu(VECTOR, VECTOR2, VECTOR3);
		//Vector[1] = -asin( VECTOR3[0]) / π * 180;
		//Vector[0] = asin(VECTOR3[1]/cos(Vector[1]*π/180)) / π * 180;
		Vector[0] = asin(VECTOR3[1]) / π * 180;
		if (VECTOR3[0] / cos(Vector[0] * π / 180)>1)
		{
			Vector[1] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[1] = -90;
		}
		else
		{
			Vector[1] = -asin(VECTOR3[0] / cos(Vector[0] * π / 180)) / π * 180;
		}


		double VECTOr[3][3]; double VECTOr2[3], VECTOr3[3];
		VECTOr[0][0] = numA2[0][0]; VECTOr[0][1] = numA2[1][0]; VECTOr[0][2] = numA2[2][0];
		VECTOr[1][0] = numB2[0][0]; VECTOr[1][1] = numB2[1][0]; VECTOr[1][2] = numB2[2][0];
		VECTOr[2][0] = numC2[0][0]; VECTOr[2][1] = numC2[1][0]; VECTOr[2][2] = numC2[2][0];
		VECTOr2[0] = numA[0][0]; VECTOr2[1] = numB[0][0]; VECTOr2[2] = numC[0][0];
		solu(VECTOr, VECTOr2, VECTOr3);
		//Vector[2] = asin(VECTOr3[0] / cos(Vector[1] * π / 180)) / π * 180; //此处有修改Vector[0]改为Vector[1]
		if ((VECTOr3[1] / cos(Vector[0] * π / 180))>1)
		{
			Vector[2] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[2] = -90;
		}
		else
		{
			Vector[2] = -asin(VECTOr3[1] / cos(Vector[0] * π / 180)) / π * 180;
		}

		//UpDataMatrix(0, num2[0][0], num2[1][0], num2[2][0]);
	};

	void DoRoll(double pitch,double roll, double Vector[6])
	{
		INITYMatri(roll, BaseVector1_2_roll);//构造roll角的旋转矩阵
		INITXMatri(pitch, BaseVector1_2_pitch);//构造roll角的旋转矩阵

		mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在pitch后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量

		mulMatri31(m_m2.BaseVector2_3, m_m2.point2_1, m_m2.point3_2);
		m_m2.point3_2[0][0] = -m_m2.point3_2[0][0];
		m_m2.point3_2[1][0] = -m_m2.point3_2[1][0];
		m_m2.point3_2[2][0] = -m_m2.point3_2[2][0];

		//mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在pitch后，在坐标系2中的表示坐标系3的基向量
		//GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
																	//UpDataMatrix(0);
		mulMatri33(BaseVector1_2_roll, BaseVector1_2_pitch, BaseVector1_2_roll);//在坐标系3中先做pitch角，后做roll角，这里的pitch是从-pitch°旋转到0°
		//mulMatri33(BaseVector1_2_roll, I, m_m2.BaseVector1_3);//在坐标系1中构造table坐标系（坐标系3）

		double num[3][3];
		double num2[3][1];
		double numA[3][1], numA2[3][1];
		double numB[3][1], numB2[3][1];
		double numC[3][1], numC2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1中的坐标
		double B1[3][1], B2[3][1];//点B在坐标系1中的坐标
		double C1[3][1], C2[3][1];

		A1[0][0] = 10; A1[1][0] = 20; A1[2][0] = 30;
		B1[0][0] = 100; B1[1][0] = 50; B1[2][0] = 87;
		C1[0][0] = 1; C1[1][0] = 1; C1[2][0] = 1;

		subMatri33(BaseVector1_2_roll, I, num);
		//mulMatri33(m_m2.BaseVector3_2, num, num);
		mulMatri31(num, m_m2.point3_2, num2);//解出平移
		mulMatri31(m_m2.BaseVector3_2, num2, num2);
		Vector[3] = num2[0][0];
		Vector[4] = num2[1][0];
		Vector[5] = num2[2][0];

		//m_m2.point1_2[0][0] = +num2[0][0];
		//m_m2.point1_2[1][0] = +num2[1][0];
		//m_m2.point1_2[2][0] = +num2[2][0];
		mulMatri33(m_m2.BaseVector3_2, BaseVector1_2_roll, num);
		mulMatri31(num, A1, numA); mulMatri31(num, B1, numB); mulMatri31(num, C1, numC);
		addMatri31(numA, m_m2.point2_1, numA); addMatri31(numB, m_m2.point2_1, numB); addMatri31(numC, m_m2.point2_1, numC);
		subMatri31(numA, num2, numA); subMatri31(numB, num2, numB); subMatri31(numC, num2, numC);

		mulMatri31(m_m2.BaseVector3_2, A1, numA2); addMatri31(numA2, m_m2.point2_1, numA2);
		mulMatri31(m_m2.BaseVector3_2, B1, numB2); addMatri31(numB2, m_m2.point2_1, numB2);
		mulMatri31(m_m2.BaseVector3_2, C1, numC2); addMatri31(numC2, m_m2.point2_1, numC2);


		double VECTOR[3][3]; double VECTOR2[3], VECTOR3[3];
		VECTOR[0][0] = numA2[0][0]; VECTOR[0][1] = numA2[1][0]; VECTOR[0][2] = numA2[2][0];
		VECTOR[1][0] = numB2[0][0]; VECTOR[1][1] = numB2[1][0]; VECTOR[1][2] = numB2[2][0];
		VECTOR[2][0] = numC2[0][0]; VECTOR[2][1] = numC2[1][0]; VECTOR[2][2] = numC2[2][0];
		VECTOR2[0] = numA[2][0]; VECTOR2[1] = numB[2][0]; VECTOR2[2] = numC[2][0];
		solu(VECTOR, VECTOR2, VECTOR3);
		//Vector[1] = -asin( VECTOR3[0]) / π * 180;
		//Vector[0] = asin(VECTOR3[1]/cos(Vector[1]*π/180)) / π * 180;
		Vector[0] = asin(VECTOR3[1]) / π * 180;
		if (VECTOR3[0] / cos(Vector[0] * π / 180)>1)
		{
			Vector[1] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[1] = -90;
		}
		else
		{
			Vector[1] = -asin(VECTOR3[0] / cos(Vector[0] * π / 180)) / π * 180;
		}


		double VECTOr[3][3]; double VECTOr2[3], VECTOr3[3];
		VECTOr[0][0] = numA2[0][0]; VECTOr[0][1] = numA2[1][0]; VECTOr[0][2] = numA2[2][0];
		VECTOr[1][0] = numB2[0][0]; VECTOr[1][1] = numB2[1][0]; VECTOr[1][2] = numB2[2][0];
		VECTOr[2][0] = numC2[0][0]; VECTOr[2][1] = numC2[1][0]; VECTOr[2][2] = numC2[2][0];
		VECTOr2[0] = numA[0][0]; VECTOr2[1] = numB[0][0]; VECTOr2[2] = numC[0][0];
		solu(VECTOr, VECTOr2, VECTOr3);
		//Vector[2] = asin(VECTOr3[0] / cos(Vector[1] * π / 180)) / π * 180; //此处有修改Vector[0]改为Vector[1]
		if ((VECTOr3[1] / cos(Vector[0] * π / 180))>1)
		{
			Vector[2] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[2] = -90;
		}
		else
		{
			Vector[2] = -asin(VECTOr3[1] / cos(Vector[0] * π / 180)) / π * 180;
		}

		//UpDataMatrix(0, num2[0][0], num2[1][0], num2[2][0]);
		mulMatri33(BaseVector1_2_roll, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在pitch后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
	};

	void xyztranslation(double x, double y, double z, double Vector[6])//在坐标系4中平移的函数分解
	{
		double num[3][1];
		double num2[3][1];
		num[0][0] = x;
		num[1][0] = y;
		num[2][0] = z;
		mulMatri31(m_m2.BaseVector3_2, num, num2);
		Vector[0] = num2[0][0];
		Vector[1] = num2[1][0];
		Vector[2] = num2[2][0];

		m_m2.point1_2[0][0] += num2[0][0];
		m_m2.point1_2[1][0] += num2[1][0];
		m_m2.point1_2[2][0] += num2[2][0];

	};

	void xyztranslation2(double x, double y, double z, double Vector[6])//在坐标系3中分解平移的函数
	{	
		//mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector3_1, m_m2.BaseVector1_3);
		
		double num[3][1];
		double num2[3][1];
		num[0][0] = x;
		num[1][0] = y;
		num[2][0] = z;
		mulMatri31(m_m2.BaseVector2_3, num, num2);//在坐标系2中分解运动矢量
		Vector[3] = num2[0][0];
		Vector[4] = num2[1][0];
		Vector[5] = num2[2][0];
		Vector[0] = 0;
		Vector[1] = 0;
		Vector[2] = 0;
		//UpDataMatrix(0, num2[0][0], num2[1][0], num2[2][0]);
	};

	void Correct(double correct[6],double Vector[6])
	{
		double x[3][3];
		double y[3][3];
		double z[3][3];
		//double xyz[3][3];
		double rotationVector[3][3];
		INITXMatri(correct[0],x); INITYMatri(correct[1], y); INITZMatri(correct[2]-270, z);
		mulMatri33(y, x, rotationVector); 
		mulMatri33(rotationVector, z, rotationVector);
		mulMatri33(rotationVector, BaseVector1_2_pitch, rotationVector);

		double num[3][3];
		double num2[3][1];
		double numA[3][1], numA2[3][1];
		double numB[3][1], numB2[3][1];
		double numC[3][1], numC2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1中的坐标
		double B1[3][1], B2[3][1];//点B在坐标系1中的坐标
		double C1[3][1], C2[3][1];

		A1[0][0] = 10; A1[1][0] = 20; A1[2][0] = 30;
		B1[0][0] = 100; B1[1][0] = 50; B1[2][0] = 87;
		C1[0][0] = 1; C1[1][0] = 1; C1[2][0] = 1;

		subMatri33(rotationVector, I, num);

		mulMatri31(m_m2.BaseVector2_3, m_m2.point2_1, m_m2.point3_2);
		m_m2.point3_2[0][0] = -m_m2.point3_2[0][0];
		m_m2.point3_2[1][0] = -m_m2.point3_2[1][0];
		m_m2.point3_2[2][0] = -m_m2.point3_2[2][0];

		xyztranslation2(correct[3], correct[4], correct[5], Vector);
		mulMatri31(num, m_m2.point3_2, num2);//解出平移
		mulMatri31(m_m2.BaseVector3_2, num2, num2);


		//subMatri33(BaseVector1_2_pitch, I, num);
		//mulMatri31(num, m_m2.point1_2, num2);//解出平移
		//mulMatri31(m_m2.BaseVector1_2, num2, num2);

		Vector[3] += num2[0][0];
		Vector[4] += num2[1][0];
		Vector[5] += num2[2][0];
		

		mulMatri33(m_m2.BaseVector3_2, rotationVector, num);
		mulMatri31(num, A1, numA); mulMatri31(num, B1, numB); mulMatri31(num, C1, numC);
		addMatri31(numA, m_m2.point2_1, numA); addMatri31(numB, m_m2.point2_1, numB); addMatri31(numC, m_m2.point2_1, numC);
		subMatri31(numA, num2, numA); subMatri31(numB, num2, numB); subMatri31(numC, num2, numC);

		mulMatri31(m_m2.BaseVector3_2, A1, numA2); addMatri31(numA2, m_m2.point2_1, numA2);
		mulMatri31(m_m2.BaseVector3_2, B1, numB2); addMatri31(numB2, m_m2.point2_1, numB2);
		mulMatri31(m_m2.BaseVector3_2, C1, numC2); addMatri31(numC2, m_m2.point2_1, numC2);


		double VECTOR[3][3]; double VECTOR2[3], VECTOR3[3];
		VECTOR[0][0] = numA2[0][0]; VECTOR[0][1] = numA2[1][0]; VECTOR[0][2] = numA2[2][0];
		VECTOR[1][0] = numB2[0][0]; VECTOR[1][1] = numB2[1][0]; VECTOR[1][2] = numB2[2][0];
		VECTOR[2][0] = numC2[0][0]; VECTOR[2][1] = numC2[1][0]; VECTOR[2][2] = numC2[2][0];
		VECTOR2[0] = numA[2][0]; VECTOR2[1] = numB[2][0]; VECTOR2[2] = numC[2][0];
		solu(VECTOR, VECTOR2, VECTOR3);

		Vector[0] = asin(VECTOR3[1]) / π * 180;
		if (VECTOR3[0] / cos(Vector[0] * π / 180)>1)
		{
			Vector[1] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[1] = -90;
		}
		else
		{
			Vector[1] = -asin(VECTOR3[0] / cos(Vector[0] * π / 180)) / π * 180;
		}

		double VECTOr[3][3]; double VECTOr2[3], VECTOr3[3];
		VECTOr[0][0] = numA2[0][0]; VECTOr[0][1] = numA2[1][0]; VECTOr[0][2] = numA2[2][0];
		VECTOr[1][0] = numB2[0][0]; VECTOr[1][1] = numB2[1][0]; VECTOr[1][2] = numB2[2][0];
		VECTOr[2][0] = numC2[0][0]; VECTOr[2][1] = numC2[1][0]; VECTOr[2][2] = numC2[2][0];
		VECTOr2[0] = numA[0][0]; VECTOr2[1] = numB[0][0]; VECTOr2[2] = numC[0][0];
		solu(VECTOr, VECTOr2, VECTOr3);
		if ((VECTOr3[1] / cos(Vector[0] * π / 180))>1)
		{
			Vector[2] = 0;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[2] = 180;
		}
		else
		{
			Vector[2] = -asin(VECTOr3[1] / cos(Vector[0] * π / 180)) / π * 180;
		}

		//UpDataMatrix(0, num2[0][0], num2[1][0], num2[2][0]);
	};

	void Correct2(double correct[6], double Vector[6])
	{
		for (size_t i = 0; i < 6; i++)//先初始化
		{
			Vector[i] = 0;
		}
		//绕Z轴旋转
		double correct2[6];
		double Vector2[6];
		double ANGLEVECTOR[3][3];
		for (size_t i = 0; i < 6; i++)
		{
			correct2[i] = 0;
		}
		correct2[2] = correct[2];
		Correct(correct2,Vector2);
		for (size_t i = 0; i < 6; i++)
		{
			Vector[i] += Vector2[i];
		}
		INITZMatri(correct[2], ANGLEVECTOR);
		mulMatri33(ANGLEVECTOR, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在旋转后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
		////绕x轴旋转
		for (size_t i = 0; i < 6; i++)
		{
			correct2[i] = 0;
		}
		correct2[0] = correct[0];
		Correct(correct2, Vector2);
		for (size_t i = 0; i < 6; i++)
		{
			Vector[i] +=Vector2[i];
		}
		//Vector[0] = m_m2.BaseVector2_3[1][1];
		//Vector[1] = m_m2.BaseVector3_2[1][2];
		INITXMatri(correct[0], ANGLEVECTOR);
		mulMatri33(ANGLEVECTOR, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在旋转后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
		//绕y轴旋转
		for (size_t i = 0; i < 6; i++)
		{
			correct2[i] = 0;
		}
		correct2[1] = correct[1];
		Correct(correct2, Vector2);
		for (size_t i = 0; i < 6; i++)
		{
			Vector[i] += Vector2[i];
		}
		INITYMatri(correct[1], ANGLEVECTOR);
		mulMatri33(ANGLEVECTOR, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在旋转后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量
		//平移
		xyztranslation2(correct[3], correct[4], correct[5], Vector2);
		for (size_t i = 0; i < 6; i++)
		{
			Vector[i] += Vector2[i];
		}
	}

	void Correct3(double correct[6], double Vector[6])
	{
		double x[3][3];
		double y[3][3];
		double z[3][3];

		double rotationVector[3][3];
		INITXMatri(correct[0], x); INITYMatri(correct[1], y); INITZMatri(correct[2]-270, z);
		//fix coord order
		//mulMatri33(y, x, rotationVector);
		//mulMatri33(rotationVector, z, rotationVector);
		//moving coord order
		mulMatri33(z, x, rotationVector);
		mulMatri33(rotationVector, y, rotationVector);
		//mulMatri33(rotationVector, BaseVector1_2_pitch, rotationVector);

		double num[3][3];
		double num2[3][1];
		double numA[3][1], numA2[3][1];
		double numB[3][1], numB2[3][1];
		double numC[3][1], numC2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1中的坐标
		double B1[3][1], B2[3][1];//点B在坐标系1中的坐标
		double C1[3][1], C2[3][1];

		A1[0][0] = 10; A1[1][0] = 20; A1[2][0] = 30;
		B1[0][0] = 100; B1[1][0] = 50; B1[2][0] = 87;
		C1[0][0] = 1; C1[1][0] = 1; C1[2][0] = 1;

		subMatri33(rotationVector, I, num);
		//mulMatri33(m_m2.BaseVector3_2, num, num);
		mulMatri31(num, m_m2.point1_2, num2);//解出平移
		mulMatri31(m_m2.BaseVector1_2, num2, num2);
		Vector[3] = num2[0][0]+ correct[3];
		Vector[4] = num2[1][0] + correct[4];
		Vector[5] = num2[2][0] + correct[5];

		//m_m2.point1_2[0][0] = +num2[0][0];
		//m_m2.point1_2[1][0] = +num2[1][0];
		//m_m2.point1_2[2][0] = +num2[2][0];
		mulMatri33(m_m2.BaseVector1_2, rotationVector, num);
		mulMatri31(num, A1, numA); mulMatri31(num, B1, numB); mulMatri31(num, C1, numC);
		addMatri31(numA, m_m2.point2_1, numA); addMatri31(numB, m_m2.point2_1, numB); addMatri31(numC, m_m2.point2_1, numC);
		subMatri31(numA, num2, numA); subMatri31(numB, num2, numB); subMatri31(numC, num2, numC);

		mulMatri31(m_m2.BaseVector1_2, A1, numA2); addMatri31(numA2, m_m2.point2_1, numA2);
		mulMatri31(m_m2.BaseVector1_2, B1, numB2); addMatri31(numB2, m_m2.point2_1, numB2);
		mulMatri31(m_m2.BaseVector1_2, C1, numC2); addMatri31(numC2, m_m2.point2_1, numC2);


		double VECTOR[3][3]; double VECTOR2[3], VECTOR3[3];
		VECTOR[0][0] = numA2[0][0]; VECTOR[0][1] = numA2[1][0]; VECTOR[0][2] = numA2[2][0];
		VECTOR[1][0] = numB2[0][0]; VECTOR[1][1] = numB2[1][0]; VECTOR[1][2] = numB2[2][0];
		VECTOR[2][0] = numC2[0][0]; VECTOR[2][1] = numC2[1][0]; VECTOR[2][2] = numC2[2][0];
		VECTOR2[0] = numA[2][0]; VECTOR2[1] = numB[2][0]; VECTOR2[2] = numC[2][0];
		solu(VECTOR, VECTOR2, VECTOR3);
		//Vector[1] = -asin( VECTOR3[0]) / π * 180;
		//Vector[0] = asin(VECTOR3[1]/cos(Vector[1]*π/180)) / π * 180;
		Vector[0] = asin(VECTOR3[1]) / π * 180;
		if (VECTOR3[0] / cos(Vector[0] * π / 180)>1)
		{
			Vector[1] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[1] = -90;
		}
		else
		{
			Vector[1] = -asin(VECTOR3[0] / cos(Vector[0] * π / 180)) / π * 180;
		}


		double VECTOr[3][3]; double VECTOr2[3], VECTOr3[3];
		VECTOr[0][0] = numA2[0][0]; VECTOr[0][1] = numA2[1][0]; VECTOr[0][2] = numA2[2][0];
		VECTOr[1][0] = numB2[0][0]; VECTOr[1][1] = numB2[1][0]; VECTOr[1][2] = numB2[2][0];
		VECTOr[2][0] = numC2[0][0]; VECTOr[2][1] = numC2[1][0]; VECTOr[2][2] = numC2[2][0];
		VECTOr2[0] = numA[0][0]; VECTOr2[1] = numB[0][0]; VECTOr2[2] = numC[0][0];
		solu(VECTOr, VECTOr2, VECTOr3);
		if ((VECTOr3[1] / cos(Vector[0] * π / 180))>1)
		{
			Vector[2] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[2] = -90;
		}
		else
		{
			Vector[2] = -asin(VECTOr3[1] / cos(Vector[0] * π / 180)) / π * 180;
		}

	};

	void CorrectinTTS(double correct[6], double Vector[6])
	{
		double num[6], num2[6], num3[6];
		double correct2[6];
		for (size_t i = 0; i < 3; i++)
		{
			correct2[i] = correct[i];
			correct2[i + 3] = 0;
		}
		//correct2[2] = 270;
		init();
		Correct3(correct2, num2);

		INITYMatri(correct[1], BaseVector1_2_roll);//构造roll角的旋转矩阵
		INITXMatri(correct[0], BaseVector1_2_pitch);//构造picth角的旋转矩阵
		INITZMatri(correct[2]-270, BaseVector1_2_iso);//构造iso角的旋转矩阵

		mulMatri33(BaseVector1_2_roll, m_m2.BaseVector1_2, m_m2.BaseVector2_3);//在iso后，在坐标系2中的表示坐标系3的基向量
		mulMatri33(BaseVector1_2_pitch, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在pitch后，在坐标系2中的表示坐标系3的基向量
		mulMatri33(BaseVector1_2_iso, m_m2.BaseVector2_3, m_m2.BaseVector2_3);//在roll后，在坐标系2中的表示坐标系3的基向量
		GetMatrixInverse(m_m2.BaseVector2_3, 3, m_m2.BaseVector3_2);//求逆,在坐标系3中的表示坐标系2的基向量

		xyztranslation2(correct[3], correct[4], correct[5], num3);
		for (size_t i = 0; i < 6; i++)
		{
			Vector[i] = num2[i] + num3[i];
		}
	}


	void resolutoIEC(float motodata[6], double Vector[6])
	{
		double x[3][3];
		double y[3][3];
		double z[3][3];

		double rotationVector[3][3];
		INITXMatri(motodata[0], x); INITYMatri(motodata[1], y); INITZMatri(motodata[2], z);
		mulMatri33(z, x, rotationVector);
		mulMatri33(rotationVector, y, rotationVector);

		double num[3][3];
		double num2[3][1];
		double numA[3][1], numA2[3][1];
		double numB[3][1], numB2[3][1];
		double numC[3][1], numC2[3][1];
		double A1[3][1], A2[3][1];//点A在坐标系1中的坐标
		double B1[3][1], B2[3][1];//点B在坐标系1中的坐标
		double C1[3][1], C2[3][1];

		A1[0][0] = 10; A1[1][0] = 20; A1[2][0] = 30;
		B1[0][0] = 100; B1[1][0] = 50; B1[2][0] = 87;
		C1[0][0] = 1; C1[1][0] = 1; C1[2][0] = 1;

		subMatri33(rotationVector, I, num);
		//mulMatri33(m_m2.BaseVector3_2, num, num);
		mulMatri31(num, m_m2.point2_1, num2);//解出平移
		mulMatri31(m_m2.BaseVector2_1, num2, num2);
		Vector[3] = num2[0][0] + motodata[3];
		Vector[4] = num2[1][0] + motodata[4];
		Vector[5] = num2[2][0] + motodata[5];

		//m_m2.point1_2[0][0] = +num2[0][0];
		//m_m2.point1_2[1][0] = +num2[1][0];
		//m_m2.point1_2[2][0] = +num2[2][0];
		mulMatri33(m_m2.BaseVector2_1, rotationVector, num);
		mulMatri31(num, A1, numA); mulMatri31(num, B1, numB); mulMatri31(num, C1, numC);
		addMatri31(numA, m_m2.point1_2, numA); addMatri31(numB, m_m2.point1_2, numB); addMatri31(numC, m_m2.point1_2, numC);
		subMatri31(numA, num2, numA); subMatri31(numB, num2, numB); subMatri31(numC, num2, numC);

		mulMatri31(m_m2.BaseVector2_1, A1, numA2); addMatri31(numA2, m_m2.point1_2, numA2);
		mulMatri31(m_m2.BaseVector2_1, B1, numB2); addMatri31(numB2, m_m2.point1_2, numB2);
		mulMatri31(m_m2.BaseVector2_1, C1, numC2); addMatri31(numC2, m_m2.point1_2, numC2);


		double VECTOR[3][3]; double VECTOR2[3], VECTOR3[3];
		VECTOR[0][0] = numA2[0][0]; VECTOR[0][1] = numA2[1][0]; VECTOR[0][2] = numA2[2][0];
		VECTOR[1][0] = numB2[0][0]; VECTOR[1][1] = numB2[1][0]; VECTOR[1][2] = numB2[2][0];
		VECTOR[2][0] = numC2[0][0]; VECTOR[2][1] = numC2[1][0]; VECTOR[2][2] = numC2[2][0];
		VECTOR2[0] = numA[1][0]; VECTOR2[1] = numB[1][0]; VECTOR2[2] = numC[1][0];
		solu(VECTOR, VECTOR2, VECTOR3);
		//Vector[1] = -asin( VECTOR3[0]) / π * 180;
		//Vector[0] = asin(VECTOR3[1]/cos(Vector[1]*π/180)) / π * 180;
		Vector[0] = -asin(VECTOR3[2]) / π * 180;
		if (VECTOR3[0] / cos(Vector[0] * π / 180)>1)
		{
			Vector[2] = 90+270;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[2] = -90+270;
		}
		else
		{
			Vector[2] = asin(VECTOR3[0] / cos(Vector[0] * π / 180)) / π * 180+270;
		}


		double VECTOr[3][3]; double VECTOr2[3], VECTOr3[3];
		VECTOr[0][0] = numA2[0][0]; VECTOr[0][1] = numA2[1][0]; VECTOr[0][2] = numA2[2][0];
		VECTOr[1][0] = numB2[0][0]; VECTOr[1][1] = numB2[1][0]; VECTOr[1][2] = numB2[2][0];
		VECTOr[2][0] = numC2[0][0]; VECTOr[2][1] = numC2[1][0]; VECTOr[2][2] = numC2[2][0];
		VECTOr2[0] = numA[0][0]; VECTOr2[1] = numB[0][0]; VECTOr2[2] = numC[0][0];
		solu(VECTOr, VECTOr2, VECTOr3);
		if ((VECTOr3[2] / cos(Vector[0] * π / 180))>1)
		{
			Vector[1] = 90;
		}
		else if (VECTOR3[0] / cos(Vector[0] * π / 180)<-1)
		{
			Vector[1] = -90;
		}
		else
		{
			Vector[1] = asin(VECTOr3[2] / cos(Vector[0] * π / 180)) / π * 180;
		}
	}

	void solu(double Vector[3][3],double b[3], double x[3])
	{
		//double a[4][4], b[4], x[4];                             //变量数组的定义
		int n = 3;//阶数
		xiaoyuan(Vector, b, n);                                     //调用函数消元 
		huidai(Vector, b, x, n);                                     //调用函数回带
	};
	void xiaoyuan(double a[3][3], double b[3], int n)
	{
		int i, j, l;
		double p, q, max;
		for (l = 0; l < n - 1; l++)//函数消元的定义 
		{
			max = fabs(a[l][1]);//调用绝对值函数，选出绝对值最大者
			j = l;
			for (i = l; i < n; i++)
			{
				if (max < fabs(a[i][l]))
				{
					max = fabs(a[i][l]);
					j = i;
				}
			}
			for (i = l; i < n; i++)
			{
				p = a[l][i];
				a[l][i] = a[j][i];
				a[j][i] = p;
			}
			p = b[l];//交换常数行
			b[l] = b[j];


			b[j] = p;
			for (i = l + 1; i < n; i++)//消元计算   
			{
				q = a[i][l] / a[l][l];
				for (j = l; j < n; j++)
					a[i][j] = double(a[i][j] - q*a[l][j]);//消元后的系数矩阵 
				b[i] -= q*b[l];//消元后的常数矩阵 
			}
		}
	};
	void huidai(double a[3][3], double b[3], double x[3], int n)//函数回带的定义 
	{
		double p;
		int i, j;
		x[n - 1] = b[n - 1] / a[n - 1][n - 1];
		i = n - 2;
		while (i >= 0)
		{
			p = 0;
			j = i + 1;
			while (j<n)
			{
				p += a[i][j] * x[j];
				x[i] = (b[i] - p) / a[i][i];
				j++;
			}
			i--;
		}
	};
};