#ifndef _SHQX_H
#define _SHQX_H

#include "_public.h"

// 全国气象站点参数数据结构
struct st_stcode
{
  char provname[31];   // 省名称
  char obtid[11];      // 站点代码
  char cityname[31];   // 城市名
  double lat;          // 纬度
  double lon;          // 经度
  double height;       // 海拔高度
};

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码
  char ddatetime[21];  // 数据时间：格式yyyy-mm-dd hh:mi:ss。
  int  t;              // 气温：单位，0.1摄氏度
  int  p;              // 气压：0.1百帕
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm
  int  vis;            // 能见度：0.1米
};
#endif
