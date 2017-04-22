#ifndef INVERTED_PENDULUM
#define INVERTED_PENDULUM

#include "encoder_timer.h"
#include "encoder_motor.h"
#include "PID.hpp"

#define PI		3.14159265358979323846
#define INF_FLOAT 3.402823466e+38F


class EncoderPendulum :public EncoderTimer
{
	unsigned int npr;//每圈pos的增量
	short oldDiff;
	short ddiff;
public:

	//将增量旋转编码器作为绝对编码器使用
	//numPerRound: 每圈pos的增量
	EncoderPendulum(TIM_TypeDef *TIMx,
		unsigned int numPerRound = 2000);

	//隐藏父类refresh()函数，对角加速度进行计算
	void refresh();

	//获取绝对弧度值，以初始化点为+-pi点，范围-pi~pi
	float getRadian();

	//获取角速度
	float getRadianDiff();

	//获取角加速度
	float getRadianDDiff();
};

class MotorBeam :public EncoderMotor
{
	unsigned int npr;//每圈pos的增量

public:

	//横梁电机，可以获取以弧度为单位的相对角度值
	//numPerRound: 每圈pos的增量
	MotorBeam(TIM_TypeDef *TIMx,
		Gpio *motorPinA, Gpio *motorPinB, Gpio *motorPinPwm,
		unsigned int numPerRound = 1560,
		Encoder_Motor_Target_Typedef controlTarget = Encoder_Motor_Target_Position,
		float refreshInterval = 0.005);

	//获取弧度值，以初始化点为0弧度点，范围-nan~+nan
	float getRadian();

	//获取角速度
	float getRadianDiff();

	//设置目标角度弧度差
	void setRadianDiff(float radian);
};

typedef enum
{
	Inverted_Pendulum_Mode_Disabled,
	Inverted_Pendulum_Mode_Invert,
	Inverted_Pendulum_Mode_Swing,
	Inverted_Pendulum_Mode_Swing_Invert
}Inverted_Pendulum_Mode_Typedef;

class InvertedPendulum
{
	float refreshInt;
	float enRadThres;//进行pid反馈的角度范围，单方向，单位弧度。初始pi/3
	int mode;
public:
	greg::PID pendulumRadianPID, beamRadianPID,//角度PID
		pendulumPalstancePID, beamPalstancePID;//角速度PID
	EncoderPendulum encoder;
	MotorBeam motor;

	InvertedPendulum(TIM_TypeDef *TIMpendulum, TIM_TypeDef *TIMmotor,
		Gpio *motorPinA, Gpio *motorPinB, Gpio *motorPinPwm,
		unsigned int nprPendulum = 2000, unsigned int nprMotor = 1560, float refreshInterval = 0.005);

	//初始化编码器、电机、PID控制，设置初始化PID值
	void begin();

	//对编码器、电机PID、倒立PID进行刷新
	void refresh();

	//设置倒立摆模式
	void setMode(Inverted_Pendulum_Mode_Typedef m) {
		mode = m;
	}

	//设置进行pid反馈的角度范围
	void setEnRadThres(float t);

	//获取摆杆角度（弧度）
	float getPendulumRadian();

	//获取摆杆角速度（弧度/秒）
	float getPendulumPalstance();

	//获取摆杆角加速度（弧度/秒^2）
	float getPendulumAcceleration();

	//获取横梁角度（弧度）
	float getBeamRadian();

	//获取横梁角速度（弧度/秒）
	float getBeamPalstance();

	//重置PID累计值
	void resetInvertPID();

	//设置横梁目标角度增量，倒立状态有效
	void setBeamRadianDiff(float d);
};

#endif