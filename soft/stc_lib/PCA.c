
#include	"PCA.h"

bit		B_Capture0,B_Capture1,B_Capture2;
u8		PCA0_mode,PCA1_mode,PCA2_mode;
u16		CCAP0_tmp,PCA_Timer0;
u16		CCAP1_tmp,PCA_Timer1;
u16		CCAP2_tmp,PCA_Timer2;


//========================================================================
// ����: UpdatePwm(u8 PCA_id, u8 pwm_value)
// ����: ����PWMֵ. 
// ����: PCA_id: PCA���. ȡֵ PCA0,PCA1,PCA2,PCA_Counter
//		 pwm_value: pwmֵ, ���ֵ������͵�ƽ��ʱ��.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void	PWMn_Update(u8 PCA_id, u8 pwm_value)
{
	if(PCA_id == PCA0)		CCAP0H = pwm_value;
	else if(PCA_id == PCA1)	CCAP1H = pwm_value;
	else if(PCA_id == PCA2)	CCAP2H = pwm_value;
}

//========================================================================
// ����: void	PCA_Init(PCA_id, PCA_InitTypeDef *PCAx)
// ����: PCA��ʼ������.
// ����: PCA_id: PCA���. ȡֵ PCA0,PCA1,PCA2,PCA_Counter
//		 PCAx: �ṹ����,��ο�PCA.h��Ķ���.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void	PCA_Init(u8 PCA_id, PCA_InitTypeDef *PCAx)
{
	if(PCA_id > PCA_Counter)	return;		//id����

	if(PCA_id == PCA_Counter)			//���ù���Counter
	{
		CR = 0;
		CH = 0;
		CL = 0;
		AUXR1 = (AUXR1 & ~(3<<4)) | PCAx->PCA_IoUse;			//�л�IO��
		CMOD  = (CMOD  & ~(7<<1)) | PCAx->PCA_Clock;			//ѡ��ʱ��Դ
		CMOD  = (CMOD  & ~1) | (PCAx->PCA_Interrupt_Mode & 1);	//ECF
		if(PCAx->PCA_Polity == PolityHigh)		PPCA = 1;	//�����ȼ��ж�
		else									PPCA = 0;	//�����ȼ��ж�
		CR = 1;
		return;
	}

	PCAx->PCA_Interrupt_Mode &= (3<<4) + 1;
	if(PCAx->PCA_Mode >= PCA_Mode_SoftTimer)	PCAx->PCA_Interrupt_Mode &= ~(3<<4);

	if(PCA_id == PCA0)
	{
		CCAPM0    = PCAx->PCA_Mode | PCAx->PCA_Interrupt_Mode;	//����ģʽ, �ж�ģʽ
		PCA_PWM0  = (PCA_PWM0 & ~(3<<6)) | PCAx->PCA_PWM_Wide;	//PWM���

		PCA_Timer0 = PCAx->PCA_Value;
		B_Capture0 = 0;
		PCA0_mode = PCAx->PCA_Mode;
		CCAP0_tmp = PCA_Timer0;
		CCAP0L = (u8)CCAP0_tmp;			//��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
		CCAP0H = (u8)(CCAP0_tmp >> 8);	//��дCCAP0H
	}
	if(PCA_id == PCA1)
	{
		CCAPM1    = PCAx->PCA_Mode | PCAx->PCA_Interrupt_Mode;
		PCA_PWM1  = (PCA_PWM1 & ~(3<<6)) | PCAx->PCA_PWM_Wide;

		PCA_Timer1 = PCAx->PCA_Value;
		B_Capture1 = 0;
		PCA1_mode = PCAx->PCA_Mode;
		CCAP1_tmp = PCA_Timer1;
		CCAP1L = (u8)CCAP1_tmp;			//��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
		CCAP1H = (u8)(CCAP1_tmp >> 8);	//��дCCAP0H
	}
	if(PCA_id == PCA2)
	{
		CCAPM2    = PCAx->PCA_Mode | PCAx->PCA_Interrupt_Mode;
		PCA_PWM2  = (PCA_PWM2 & ~(3<<6)) | PCAx->PCA_PWM_Wide;

		PCA_Timer2 = PCAx->PCA_Value;
		B_Capture2 = 0;
		PCA2_mode = PCAx->PCA_Mode;
		CCAP2_tmp = PCA_Timer2;
		CCAP2L = (u8)CCAP2_tmp;			//��Ӱ��Ĵ���д�벶��Ĵ�������дCCAP0L
		CCAP2H = (u8)(CCAP2_tmp >> 8);	//��дCCAP0H
	}
}

