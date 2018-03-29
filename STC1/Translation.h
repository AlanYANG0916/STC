#pragma once
#include <math.h>
 
#include "stdafx.h"
#define π 3.1415929

struct TransLateData
{
	double point1_2[3][1];//初始时，坐标系1中坐标系2的原点
	double point1_2_θ[3][1];//旋转后，坐标系1中坐标系2的原点
	double point1_IEC[3][1];//坐标系1中，IEC原点坐标


	double BaseVector1_2[3][3];//坐标系1中坐标系2的基向量
	double BaseVector1_2_θ[3][3];//旋转后，坐标系1中坐标系2的基向量
	double BaseVector1_3[3][3];//坐标系1中坐标系3的基向量
	double BaseVector1_5[3][3];//坐标系1中坐标系5的基向量

	double point2_1[3][1];//坐标系2中坐标系1的原点
	double point2_IEC[3][1];//坐标系2中坐标系3的原点

	double BaseVector2_1[3][3];//坐标系2中坐标系1的基向量

	double BaseVector2_3[3][3];//坐标系2中坐标系3的基向量
	double BaseVector3_2[3][3];//坐标系3中坐标系2的基向量

	double BaseVector2_5[3][3];//坐标系2中坐标系5的基向量
	double BaseVector5_2[3][3];//坐标系5中坐标系2的基向量

	//double BaseVector1_2_θ[3][3];//360°旋转矩阵
	int pitchangle;//pitch角度数值
	double BaseVector1_2_pitch[3][3];//pitch角矩阵
	double I[3][3];//单位阵

	double xyzOffset[3];//系统给出的需平移量，基于TTS坐标系（坐标系4）
	double Corrvector[6];//六维度校正矩阵值

	double ShowData[6];

};

struct MatrixDate{
	
};

class Matrix
{
public:
	double pitch;
	double roll;
	double rotation;
	double tx, ty, tz;
	double Vector[4][4];

	void INIT(double ax, double ay, double az,double x,double y,double z)
	{	
			pitch = ax; roll = ay; rotation = az;
			tx = x; ty = y; tz = z;
		
		double a, b, c;
		a = ax / 180 * π; b = ay / 180 * π; c = az / 180 * π;
		double cosx = cos(a);
		double cosy = cos(b);
		double cosz = cos(c);
		double sinx = sin(a);
		double siny = sin(b);
		double sinz = sin(c);
		Vector[0][0] = cosy*cosz-sinx*siny*sinz; Vector[0][1] = cosy*sinz+sinx*siny*cosz; Vector[0][2] = -siny*cosx; Vector[0][3] = x;
		Vector[1][0] = -cosx*sinz; Vector[1][1] = cosx*cosz; Vector[1][2] = sinx; Vector[1][3] = y;
		Vector[2][0] = siny*cosz+cosy*sinx*sinz; Vector[2][1] = sinx*siny-cosy*sinx*cosz; Vector[2][2] = cosx*cosy; Vector[2][3] = z;
		Vector[3][0] = 0; Vector[3][1] = 0; Vector[3][2] = 0; Vector[3][3] = 1;
	};

};

class Translation {

public:
	Translation()
	{
		memset(&m_m, 0, sizeof(TransLateData));
	};

	TransLateData m_m;
	Matrix Corr;
	Matrix Pre;
	double Offset[6];
	//定义矩阵之间的运算

	void Mul44(double x[4][4], double y[4][4], double z[4][4])//44*44乘法
	{
		double A[4][4],B[4][4];
		int i, j, k;
		for (i = 0; i < 4; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 4; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				z[i][j] = 0;
				for (k = 0; k < 4; k++)
				{
					z[i][j] += A[i][k] * B[k][j];
				}
			}
		}
	};
	void Mul41(double x[4][4], double y[4][1], double z[4][1])//44*41乘法
	{
		double A[4][4], B[4][4];
		int i, j, k;
		for (i = 0; i < 4; i++)//防止数据空间串扰,
		{
			B[i][0] = y[i][0];
			for (j = 0; j < 4; j++)
			{
				A[i][j] = x[i][j];
			}
		}

		for (i = 0; i < 4; i++)
		{
			for (k = 0; k < 4; k++)
			{
				z[i][0] += A[i][k] * B[k][0];
			}
		}
	};
	void Mul33(double x[3][3], double y[3][3], double z[3][3])//33*33乘法
	{
		double A[3][3], B[3][3];
		int i, j, k;
		for (i = 0; i < 3; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 3; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				z[i][j] = 0;
				for (k = 0; k < 3; k++)
				{
					z[i][j] += A[i][k] * B[k][j];
				}
			}
		}
	};
	void Mul31(double x[3][3], double y[3][1], double z[3][1])//33*31乘法
	{
		double A[3][3], B[3][3];
		int i, j, k;
		for (i = 0; i < 3; i++)//防止数据空间串扰,
		{
			B[i][0] = y[i][0];
			for (j = 0; j < 3; j++)
			{
				A[i][j] = x[i][j];
			}
		}
		for (i = 0; i < 3; i++)
		{
			for (k = 0; k < 3; k++)
			{
				z[i][0] += A[i][k] * B[k][0];
			}
		}
	};
	void Sub44(double x[4][4], double y[4][4], double z[4][4])//44*44减法
	{
		double A[4][4], B[4][4];
		int i, j, k;
		for (i = 0; i < 4; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 4; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				z[i][j] = A[i][j]-B[i][j];
			}
		}
	};
	void Sub33(double x[3][3], double y[3][3], double z[3][3])//33*33减法
	{
		double A[3][3], B[3][3];
		int i, j, k;
		for (i = 0; i < 3; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 3; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				z[i][j] = A[i][j] - B[i][j];
			}
		}
	};
	void Add44(double x[4][4], double y[4][4], double z[4][4])//44*44加法
	{
		double A[4][4], B[4][4];
		int i, j, k;
		for (i = 0; i < 4; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 4; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				z[i][j] = A[i][j] + B[i][j];
			}
		}
	};
	void Add33(double x[3][3], double y[3][3], double z[3][3])//33*33加法
	{
		double A[3][3], B[3][3];
		int i, j, k;
		for (i = 0; i < 3; i++)//防止数据空间串扰,
		{
			for (j = 0; j < 3; j++)
			{
				A[i][j] = x[i][j];
				B[i][j] = y[i][j];
			}
		}

		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				z[i][j] = A[i][j] + B[i][j];
			}
		}
	};

	////构造44*44旋转矩阵
	//void INITMatri44(int angle, int axis,double Vector[4][4])
	//{
	//	switch (axis)
	//	{
	//	case 1://x

	//			Vector[0][0] = 1; Vector[0][1] = 0; Vector[0][2] = 0; Vector[0][3] = 0;
	//			Vector[1][0] = 0; Vector[1][1] = cos(angle / 180 * π); Vector[1][2] = sin(angle / 180 * π); Vector[1][3] = 0;
	//			Vector[2][0] = 0; Vector[2][1] = -sin(angle / 180 * π); Vector[2][2] = cos(angle / 180 * π); Vector[2][3] = 0;
	//			Vector[3][0] = 0; Vector[3][1] = 0; Vector[3][2] = 0; Vector[3][3] = 1;

	//		break;
	//	case 2://y

	//			Vector[0][0] = cos(angle / 180 * π); Vector[0][1] = 0; Vector[0][2] = -sin(angle / 180 * π); Vector[0][3] = 0;
	//			Vector[1][0] = 0; Vector[1][1] = 1; Vector[1][2] = 0; Vector[1][3] = 0;
	//			Vector[2][0] = sin(angle / 180 * π); Vector[2][1] = 0; Vector[2][2] = cos(angle / 180 * π); Vector[2][3] = 0;
	//			Vector[3][0] = 0; Vector[3][1] = 0; Vector[3][2] = 0; Vector[3][3] = 1;

	//		break;
	//	case 3://z

	//			Vector[0][0] = cos(angle / 180 * π); Vector[0][1] = sin(angle / 180 * π); Vector[0][2] = 0; Vector[0][3] = 0;
	//			Vector[1][0] = -sin(angle / 180 * π); Vector[1][1] = cos(angle / 180 * π); Vector[1][2] = 0; Vector[1][3] = 0;
	//			Vector[2][0] = 0; Vector[2][1] = 0; Vector[2][2] = 1; Vector[2][3] = 0;
	//			Vector[3][0] = 0; Vector[3][1] = 0; Vector[3][2] = 0; Vector[3][3] = 1;

	//		break;
	//	}
	//};
	//void InitransMatri44(double x, double y, double z,double Vector[4][4])
	//{
	//	Vector[0][0] = 1; Vector[0][1] = 0; Vector[0][2] = 0; Vector[0][3] = x;
	//	Vector[1][0] = 0; Vector[1][1] = 1; Vector[1][2] = 0; Vector[1][3] = y;
	//	Vector[2][0] = 0; Vector[2][1] = 0; Vector[2][2] = 1; Vector[2][3] = z;
	//	Vector[3][0] = 0; Vector[3][1] = 0; Vector[3][2] = 0; Vector[3][3] = 1;
	//};
	void PREMatrix(double ax, double ay, double az, double x, double y, double z)
	{
		Pre.INIT(ax,ay,az,x,y,z);
	};
	void CORRMatrix(double ax, double ay, double az, double x, double y, double z)
	{
		Corr.INIT(ax, ay, az, x, y, z);
	};
	void solucorr(double vector[6])//根据西门子的算法解出的矫正后的六维度数据
	{
		//根据验证，这里的平移量应当是没有没有负号的
		vector[3] = (Corr.Vector[0][0] * Pre.Vector[0][3] + Corr.Vector[0][1] * Pre.Vector[1][3] + Corr.Vector[0][2] * Pre.Vector[2][3] + Corr.Vector[0][3]);
		vector[4] = (Corr.Vector[1][0] * Pre.Vector[0][3] + Corr.Vector[1][1] * Pre.Vector[1][3] + Corr.Vector[1][2] * Pre.Vector[2][3] + Corr.Vector[1][3]);
		vector[5] = (Corr.Vector[2][0] * Pre.Vector[0][3] + Corr.Vector[2][1] * Pre.Vector[1][3] + Corr.Vector[2][2] * Pre.Vector[2][3] + Corr.Vector[2][3]);
		vector[2] = atan(-(Corr.Vector[1][0] * Pre.Vector[0][0] + Corr.Vector[1][1] * Pre.Vector[1][0] + Corr.Vector[1][2] * Pre.Vector[2][0] )
			/(Corr.Vector[1][0] * Pre.Vector[0][1] + Corr.Vector[1][1] * Pre.Vector[1][1] + Corr.Vector[1][2] * Pre.Vector[2][1] ))*180/π + 360;

		vector[0] = asin(Corr.Vector[1][0] * Pre.Vector[0][2] + Corr.Vector[1][1] * Pre.Vector[1][2] + Corr.Vector[1][2] * Pre.Vector[2][2]) * 180 / π ;

		vector[1] = atan(-(Corr.Vector[0][0] * Pre.Vector[0][2] + Corr.Vector[0][1] * Pre.Vector[1][2] + Corr.Vector[0][2] * Pre.Vector[2][2])
			/ (Corr.Vector[2][0] * Pre.Vector[0][2] + Corr.Vector[2][1] * Pre.Vector[1][2] + Corr.Vector[2][2] * Pre.Vector[2][2])) * 180 / π;
	};
	void solu(double b[4],double x[4])
	{
		//double a[4][4], b[4], x[4];                             //变量数组的定义
		int n = 4;//阶数
		xiaoyuan(Pre.Vector, b, n);                                     //调用函数消元 
		huidai(Pre.Vector,b,x,n);                                     //调用函数回带
	};
	void solu2(double b[4], double x[4])
	{
		double bb[4][1];
		double xx[4][1];
		for (size_t i = 0; i < 4; i++)
		{
			bb[i][0] = b[i];
		}
		Mul41(Pre.Vector,bb, xx);
		for (size_t i = 0; i < 4; i++)
		{
			x[i] = xx[i][0];
		}
	};
	void xiaoyuan(double a[4][4], double b[4], int n) 
	{
		int i, j, l;
		double p, q, max;
		for (l = 0; l < n-1; l++)//函数消元的定义 
		{
			max = fabs(a[l][1]);//调用绝对值函数，选出绝对值最大者
			j = l;
			for ( i = l; i < n; i++)
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
	void huidai(double a[4][4], double b[4], double x[4],int n)//函数回带的定义 
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


	void MatrixToSolve(double MoveDataInTTS[7], double MoveDataIn6DOF[6],double A360angle)
	{
		double x = MoveDataInTTS[0] * π/180;
		double y = MoveDataInTTS[1] * π / 180;
		double z = (MoveDataInTTS[2]-270) * π / 180;
		double zz= A360angle * π / 180;
		double PitchMatrix[4][4], RollMatrix[4][4], IsoMatrix[4][4], TranslationMatrix[4][4];
		PitchMatrix[0][0]=1; PitchMatrix[0][1] = 0; PitchMatrix[0][2] = 0; PitchMatrix[0][3] = 0;
		PitchMatrix[1][0] = 0; PitchMatrix[1][1] = cos(x); PitchMatrix[1][2] = sin(x); PitchMatrix[1][3] = 0;
		PitchMatrix[2][0] = 0; PitchMatrix[2][1] = -sin(x); PitchMatrix[2][2] = cos(x); PitchMatrix[2][3] = 0;
		PitchMatrix[3][0] = 0; PitchMatrix[3][1] = 0; PitchMatrix[3][2] = 0; PitchMatrix[3][3] = 1;

		RollMatrix[0][0] = cos(y); RollMatrix[0][1] = 0; RollMatrix[0][2] = -sin(y); RollMatrix[0][3] = 0;
		RollMatrix[1][0] = 0; RollMatrix[1][1] = 1; RollMatrix[1][2] = 0; RollMatrix[1][3] = 0;
		RollMatrix[2][0] = sin(y); RollMatrix[2][1] = 0; RollMatrix[2][2] = cos(y); RollMatrix[2][3] = 0;
		RollMatrix[3][0] = 0; RollMatrix[3][1] = 0; RollMatrix[3][2] = 0; RollMatrix[3][3] = 1;

		IsoMatrix[0][0] = cos(z); IsoMatrix[0][1] = sin(z); IsoMatrix[0][2] = 0; IsoMatrix[0][3] = 0;
		IsoMatrix[1][0] = -sin(z); IsoMatrix[1][1] = cos(z); IsoMatrix[1][2] = 0; IsoMatrix[1][3] = 0;
		IsoMatrix[2][0] = 0; IsoMatrix[2][1] = 0; IsoMatrix[2][2] = 1; IsoMatrix[2][3] = 0;
		IsoMatrix[3][0] = 0; IsoMatrix[3][1] = 0; IsoMatrix[3][2] = 0; IsoMatrix[3][3] = 1;

		TranslationMatrix[0][0] = 1; TranslationMatrix[0][1] = 0; TranslationMatrix[0][2] = 0; TranslationMatrix[0][3] = -MoveDataInTTS[3];
		TranslationMatrix[1][0] = 0; TranslationMatrix[1][1] = 1; TranslationMatrix[1][2] = 0; TranslationMatrix[1][3] = -MoveDataInTTS[4];
		TranslationMatrix[2][0] = 0; TranslationMatrix[2][1] = 0; TranslationMatrix[2][2] = 1; TranslationMatrix[2][3] = -MoveDataInTTS[5];
		TranslationMatrix[3][0] = 0; TranslationMatrix[3][1] = 0; TranslationMatrix[3][2] = 0; TranslationMatrix[3][3] = 1;
		double TTSMatrix[4][4], InvTTSMatrix[4][4];
		Mul44(TranslationMatrix, RollMatrix, TTSMatrix);
		Mul44(TTSMatrix, PitchMatrix, TTSMatrix);
		Mul44(TTSMatrix, IsoMatrix, TTSMatrix);
		GetMatrixInverse(TTSMatrix,4, InvTTSMatrix);

		double OriginMatrix[4][4], InvOriginMatrix[4][4];
		OriginMatrix[0][0] = 1; OriginMatrix[0][1] = 0; OriginMatrix[0][2] = 0; OriginMatrix[0][3] = Offset[3];;// -3;
		OriginMatrix[1][0] = 0; OriginMatrix[1][1] = 1; OriginMatrix[1][2] = 0; OriginMatrix[1][3] = Offset[4];;// 0.5;
		OriginMatrix[2][0] = 0; OriginMatrix[2][1] = 0; OriginMatrix[2][2] = 1; OriginMatrix[2][3] = Offset[5];// -1028;
		OriginMatrix[3][0] = 0; OriginMatrix[3][1] = 0; OriginMatrix[3][2] = 0; OriginMatrix[3][3] = 1;
		GetMatrixInverse(OriginMatrix, 4, InvOriginMatrix);

		double AxisMatrix[4][4],InvAxisMatrix[4][4];
		AxisMatrix[0][0] = cos(zz); AxisMatrix[0][1] = sin(zz); AxisMatrix[0][2] = 0; AxisMatrix[0][3] = 0;
		AxisMatrix[1][0] = -sin(zz); AxisMatrix[1][1] = cos(zz); AxisMatrix[1][2] = 0; AxisMatrix[1][3] = 0;
		AxisMatrix[2][0] = 0; AxisMatrix[2][1] = 0; AxisMatrix[2][2] = 1; AxisMatrix[2][3] = 0;
		AxisMatrix[3][0] = 0; AxisMatrix[3][1] = 0; AxisMatrix[3][2] = 0; AxisMatrix[3][3] = 1;
		GetMatrixInverse(AxisMatrix, 4, InvAxisMatrix);

		double SoluMatrix[4][4];
		Mul44(InvAxisMatrix, InvOriginMatrix,SoluMatrix);
		Mul44(SoluMatrix, InvTTSMatrix, SoluMatrix);
		Mul44(SoluMatrix, OriginMatrix, SoluMatrix);
		Mul44(SoluMatrix, AxisMatrix, SoluMatrix);

			//MoveDataIn6DOF[0] = MoveDataInTTS[0];
			//MoveDataIn6DOF[1] = MoveDataInTTS[1];
		//order is zxy
			MoveDataIn6DOF[0] = asin(SoluMatrix[2][1]) / π * 180;
			if (SoluMatrix[2][0] / cos(MoveDataIn6DOF[0] * π / 180)>1)
			{
				MoveDataIn6DOF[1] = 90;
			}
			else if (SoluMatrix[2][0] / cos(MoveDataIn6DOF[0] * π / 180)<-1)
			{
				MoveDataIn6DOF[1] = -90;
			}
			else
			{
				MoveDataIn6DOF[1] = -asin(SoluMatrix[2][0] / cos(MoveDataIn6DOF[0] * π / 180)) / π * 180;
			}

			//MoveDataIn6DOF[2] = MoveDataInTTS[2];

			if ((SoluMatrix[0][1] / cos(MoveDataIn6DOF[0] * π / 180))>1)
			{
				MoveDataIn6DOF[2] = 90;
			}
			else if (SoluMatrix[0][1] / cos(MoveDataIn6DOF[0] * π / 180)<-1)
			{
				MoveDataIn6DOF[2] = -90;
			}
			else
			{
				MoveDataIn6DOF[2] = -asin(SoluMatrix[0][1] / cos(MoveDataIn6DOF[0] * π / 180)) / π * 180+270;
			}

			//order is zyx
			/*MoveDataIn6DOF[1] = -asin(SoluMatrix[2][0]) / π * 180;
			if (SoluMatrix[2][1] / cos(MoveDataIn6DOF[1] * π / 180)>1)
			{
				MoveDataIn6DOF[0] = 90;
			}
			else if (SoluMatrix[2][1] / cos(MoveDataIn6DOF[1] * π / 180)<-1)
			{
				MoveDataIn6DOF[0] = -90;
			}
			else
			{
				MoveDataIn6DOF[0] = asin(SoluMatrix[2][1] / cos(MoveDataIn6DOF[1] * π / 180)) / π * 180;
			}

			if ((SoluMatrix[1][0] / cos(MoveDataIn6DOF[0] * π / 180))>1)
			{
				MoveDataIn6DOF[2] = 90;
			}
			else if (SoluMatrix[1][0] / cos(MoveDataIn6DOF[0] * π / 180)<-1)
			{
				MoveDataIn6DOF[2] = -90;
			}
			else
			{
				MoveDataIn6DOF[2] = asin(SoluMatrix[1][0] / cos(MoveDataIn6DOF[1] * π / 180)) / π * 180 + 270;
			}*/

			MoveDataIn6DOF[3] = SoluMatrix[0][3];
			MoveDataIn6DOF[4] = SoluMatrix[1][3];
			MoveDataIn6DOF[5] = SoluMatrix[2][3];
	};

	void ReMatrixToSolve(float MoveDataIn6DOF[6], double MoveDataInTTS[6] )
	{
		double x = MoveDataIn6DOF[0] * π / 180;
		double y = MoveDataIn6DOF[1] * π / 180;
		double z = (MoveDataIn6DOF[2]) * π / 180;
		double PitchMatrix[4][4], RollMatrix[4][4], IsoMatrix[4][4], TranslationMatrix[4][4];
		PitchMatrix[0][0] = 1; PitchMatrix[0][1] = 0; PitchMatrix[0][2] = 0; PitchMatrix[0][3] = 0;
		PitchMatrix[1][0] = 0; PitchMatrix[1][1] = cos(x); PitchMatrix[1][2] = -sin(x); PitchMatrix[1][3] = 0;
		PitchMatrix[2][0] = 0; PitchMatrix[2][1] = sin(x); PitchMatrix[2][2] = cos(x); PitchMatrix[2][3] = 0;
		PitchMatrix[3][0] = 0; PitchMatrix[3][1] = 0; PitchMatrix[3][2] = 0; PitchMatrix[3][3] = 1;

		RollMatrix[0][0] = cos(y); RollMatrix[0][1] = 0; RollMatrix[0][2] = sin(y); RollMatrix[0][3] = 0;
		RollMatrix[1][0] = 0; RollMatrix[1][1] = 1; RollMatrix[1][2] = 0; RollMatrix[1][3] = 0;
		RollMatrix[2][0] = -sin(y); RollMatrix[2][1] = 0; RollMatrix[2][2] = cos(y); RollMatrix[2][3] = 0;
		RollMatrix[3][0] = 0; RollMatrix[3][1] = 0; RollMatrix[3][2] = 0; RollMatrix[3][3] = 1;

		IsoMatrix[0][0] = cos(z); IsoMatrix[0][1] = -sin(z); IsoMatrix[0][2] = 0; IsoMatrix[0][3] = 0;
		IsoMatrix[1][0] = sin(z); IsoMatrix[1][1] = cos(z); IsoMatrix[1][2] = 0; IsoMatrix[1][3] = 0;
		IsoMatrix[2][0] = 0; IsoMatrix[2][1] = 0; IsoMatrix[2][2] = 1; IsoMatrix[2][3] = 0;
		IsoMatrix[3][0] = 0; IsoMatrix[3][1] = 0; IsoMatrix[3][2] = 0; IsoMatrix[3][3] = 1;

		TranslationMatrix[0][0] = 1; TranslationMatrix[0][1] = 0; TranslationMatrix[0][2] = 0; TranslationMatrix[0][3] = MoveDataIn6DOF[3];
		TranslationMatrix[1][0] = 0; TranslationMatrix[1][1] = 1; TranslationMatrix[1][2] = 0; TranslationMatrix[1][3] = MoveDataIn6DOF[4];
		TranslationMatrix[2][0] = 0; TranslationMatrix[2][1] = 0; TranslationMatrix[2][2] = 1; TranslationMatrix[2][3] = MoveDataIn6DOF[5];
		TranslationMatrix[3][0] = 0; TranslationMatrix[3][1] = 0; TranslationMatrix[3][2] = 0; TranslationMatrix[3][3] = 1;
		double DOFMatrix[4][4], InvDOFMatrix[4][4];
		Mul44(TranslationMatrix, IsoMatrix, DOFMatrix);
		Mul44(DOFMatrix, PitchMatrix, DOFMatrix);
		Mul44(DOFMatrix, RollMatrix, DOFMatrix);
		GetMatrixInverse(DOFMatrix, 4, InvDOFMatrix);

		double OriginMatrix[4][4], InvOriginMatrix[4][4];
		OriginMatrix[0][0] = 1; OriginMatrix[0][1] = 0; OriginMatrix[0][2] = 0; OriginMatrix[0][3] = Offset[3];
		OriginMatrix[1][0] = 0; OriginMatrix[1][1] = 1; OriginMatrix[1][2] = 0; OriginMatrix[1][3] = Offset[4];
		OriginMatrix[2][0] = 0; OriginMatrix[2][1] = 0; OriginMatrix[2][2] = 1; OriginMatrix[2][3] = Offset[5];
		OriginMatrix[3][0] = 0; OriginMatrix[3][1] = 0; OriginMatrix[3][2] = 0; OriginMatrix[3][3] = 1;
		GetMatrixInverse(OriginMatrix, 4, InvOriginMatrix);

		double AxisMatrix[4][4], InvAxisMatrix[4][4];
		AxisMatrix[0][0] = 1; AxisMatrix[0][1] = 0; AxisMatrix[0][2] = 0; AxisMatrix[0][3] = 0;
		AxisMatrix[1][0] = 0; AxisMatrix[1][1] = 1; AxisMatrix[1][2] = 0; AxisMatrix[1][3] = 0;
		AxisMatrix[2][0] = 0; AxisMatrix[2][1] = 0; AxisMatrix[2][2] = 1; AxisMatrix[2][3] = 0;
		AxisMatrix[3][0] = 0; AxisMatrix[3][1] = 0; AxisMatrix[3][2] = 0; AxisMatrix[3][3] = 1;
		GetMatrixInverse(AxisMatrix, 4, InvAxisMatrix);

		double SoluMatrix[4][4];
		Mul44(OriginMatrix,AxisMatrix, SoluMatrix);
		Mul44(SoluMatrix, DOFMatrix, SoluMatrix);
		Mul44(SoluMatrix, InvAxisMatrix, SoluMatrix);
		Mul44(SoluMatrix, InvOriginMatrix, SoluMatrix);

		GetMatrixInverse(SoluMatrix,4, SoluMatrix);


		MoveDataInTTS[0] = asin(SoluMatrix[1][2]) / π * 180;
		if (SoluMatrix[0][2] / cos(MoveDataInTTS[0] * π / 180)>1)
		{
			MoveDataInTTS[1] = 90;
		}
		else if (SoluMatrix[0][2] / cos(MoveDataInTTS[0] * π / 180)<-1)
		{
			MoveDataInTTS[1] = -90;
		}
		else
		{
			MoveDataInTTS[1] = -asin(SoluMatrix[0][2] / cos(MoveDataInTTS[0] * π / 180)) / π * 180;
		};

		if ((SoluMatrix[1][0] / cos(MoveDataInTTS[0] * π / 180))>1)
		{
			MoveDataInTTS[2] = 90;
		}
		else if (SoluMatrix[1][0] / cos(MoveDataInTTS[0] * π / 180)<-1)
		{
			MoveDataInTTS[2] = -90;
		}
		else
		{
			MoveDataInTTS[2] = -asin(SoluMatrix[1][0] / cos(MoveDataInTTS[0] * π / 180)) / π * 180;
		}
		MoveDataInTTS[3] = -SoluMatrix[0][3];
		MoveDataInTTS[4] = -SoluMatrix[1][3];
		MoveDataInTTS[5] = -SoluMatrix[2][3];
	};

	double getA(double arcs[L][L], int n)
	{
		if (n == 1)
		{
			return arcs[0][0];
		}
		double ans = 0;
		double temp[L][L] = { 0.0 };
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
	void  getAStart(double arcs[L][L], int n, double ans[L][L])
	{
		if (n == 1)
		{
			ans[0][0] = 1;
			return;
		}
		int i, j, k, t;
		double temp[L][L];
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
	bool GetMatrixInverse(double src[L][L], int n, double des[L][L])
	{
		double flag = getA(src, n);
		double t[L][L];
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

	//对于360°旋转台与房间Z轴偏移的校正
	void CorrectionFor360Zaxis(double input[6],double output[6])
	{
		CString strFilePath;
		CFile file;
		
			strFilePath = L"./360Z_Axis_Corr.csv";
		if (!file.Open(strFilePath, CFile::modeRead))
			 {
			return;
			}
		double correctdata2[36][3];
		
			CString result;
		result = "";
		char buffer[4096];
		int row = -1;
		int line = 0;
		file.Read(buffer, file.GetLength());
		
			for (int i = 0; i<(file.GetLength()); i++)
			 {
			if (buffer[i] == 0x0A)//换行
				 {
				if (row != -1)
					 {
					correctdata2[row][line] = _wtof(result.GetBuffer());
					row++;
					result = L"";
					line = -1;
					}
				else
					 {
					row = 0;
					}
				
					}
			else if (buffer[i] == 0x2C)//逗号
				 {
				if (row != -1)
					 {
					if (line == -1)
						{
						line = 0;
						}
					correctdata2[row][line] = _wtof(result.GetBuffer());
					result = L"";
					line++;
					}
				
					}
			else if (buffer[i] == 0x00)
				 {
				
					}
			else
				 {
				result += buffer[i];
				}
			}
		for (size_t i = 0; i < 6; i++)
			 {
			output[i] = 0;
			}
		int nnn = 270 - (int)input[2];
		if (nnn >= 360)
			 {
			nnn -= 360;
			}
		double a = input[2] * π / 180;
		double MATIRC[3][3]; double QQ[3][1]; double QQQ[3][1];
		MATIRC[0][0] = cos(-a); MATIRC[0][1] = sin(-a); MATIRC[0][2] = 0;
		MATIRC[1][0] = -sin(-a); MATIRC[1][1] = cos(-a); MATIRC[1][2] = 0;
		MATIRC[2][0] = 0; MATIRC[2][1] = 0; MATIRC[2][2] = 1;
		
			QQ[0][0] = correctdata2[nnn / 10][1]; QQ[0][1] = correctdata2[nnn / 10][2]; QQ[0][2] = 0;
		Mul31(MATIRC, QQ, QQQ);
		output[3] = -QQQ[0][0];
		output[4] = -QQQ[0][1];
	};

	void ReCorrectionFor360Zaxis(double input[6], double output[6])
	{
		//input为270°时xy方向的差值
		double NUM[6];
		for (size_t i = 0; i < 6; i++)
		{
			NUM[i] = input[i];
		}
		if (NUM[2]>0)
		{
			NUM[2] =input[2];
		}
		else if (NUM[2]<0)
		{
			NUM[2] = 360 +input[2];
		}
		//input[3] = 90;
		double a = NUM[2] * π / 180;

		output[3] = input[3] * cos(a) - input[4] * sin(a) - input[3];
		output[4] = input[3] * sin(a) + input[4] * cos(a) - input[4];
		output[5] = 0;

		output[0] = 0;
		output[1] = 0;
		output[2] = 0;
	};


};