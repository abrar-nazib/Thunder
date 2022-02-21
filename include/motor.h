#ifndef _MOTOR_CONTROL
#define _MOTOR_CONTROL

void setupMotors();
void motorTestAuto();
void motorCalibrate();
void Forward(double del, int vel);
void Backward(double del, int vel);
void Right(double del, int vel);
void Left(double del, int vel);
void Stop(double del);
void Tleft();
void Tright();
void BreakR();
void BreakL();
void BreakF();

#endif // _MOTOR_CONTROL