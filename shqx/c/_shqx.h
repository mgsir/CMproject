#ifndef _SHQX_H
#define _SHQX_H

#include "_public.h"

// ȫ������վ��������ݽṹ
struct st_stcode
{
  char provname[31];   // ʡ����
  char obtid[11];      // վ�����
  char cityname[31];   // ������
  double lat;          // γ��
  double lon;          // ����
  double height;       // ���θ߶�
};

// ȫ������վ����ӹ۲����ݽṹ
struct st_surfdata
{
  char obtid[11];      // վ�����
  char ddatetime[21];  // ����ʱ�䣺��ʽyyyy-mm-dd hh:mi:ss��
  int  t;              // ���£���λ��0.1���϶�
  int  p;              // ��ѹ��0.1����
  int  u;              // ���ʪ�ȣ�0-100֮���ֵ��
  int  wd;             // ����0-360֮���ֵ��
  int  wf;             // ���٣���λ0.1m/s
  int  r;              // ��������0.1mm
  int  vis;            // �ܼ��ȣ�0.1��
};
#endif
