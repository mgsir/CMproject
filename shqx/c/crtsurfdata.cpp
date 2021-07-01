#include"_shqx.h"

vector<st_stcode> vstcode;
vector<st_surfdata> vsurfdata; 


bool LoadSTCode(const char *inifile);

void CrtSurfData();

bool CrtSurfFile(const char *outpath);

CLogFile logfile;

int main(int argc, char * argv[])
{
    if(argc != 4)
    {
        printf("\nThis program is used to generate minute data of national weather stations\n");
        printf("Using: this_program sites_parameters the_direcotry_where_weather_data_is_stored logfile_name\n");
        printf("Sample: /home/mg/CProject/shqx/bin/crtsurfdata  /home/mg/CProject/shqx/ini/ini/stcode.ini "\
        "/home/mg/CProject/shqx/data/ftp/surfdata /home/mg/CProject/shqx/log/crtsurfdata.log\n");
        return -1;
    }

    CloseIOAndSignal();

    if (logfile.Open(argv[3], "a+") == false)
    {
        printf("Open logfile(%s) failed\n",argv[3]); return -1;
    }

    while (true)
    {
        if(LoadSTCode(argv[1]) == false){sleep(60); continue;}

        logfile.Write("Load inifile(%s) successfully\n",argv[1]);

        CrtSurfData();

        if(CrtSurfFile(argv[2]) == false){sleep(60); continue;}

        sleep(60);
    }
    return 0;
    
}

bool LoadSTCode(const char *inifile)
{
    vstcode.clear();

    CCmdStr CmdStr;
    struct st_stcode *stcode = new st_stcode;
    CFile File;

    if(File.Open(inifile,"r") == false)
    {
        logfile.Write("Open inifile(%s) failed \n",inifile);
        return false;
    }

    char strbuff[101];

    while(true)
    {
        memset(stcode,0,sizeof(struct st_stcode));

        if(File.Fgets(strbuff,100) == false) break;

        CmdStr.SplitToCmd(strbuff,",",true);

        CmdStr.GetValue(0, stcode->provname);
        CmdStr.GetValue(1, stcode->obtid);
        CmdStr.GetValue(2, stcode->cityname);
        CmdStr.GetValue(3,&stcode->lat);
        CmdStr.GetValue(4,&stcode->lon);
        CmdStr.GetValue(5,&stcode->height);

        vstcode.push_back(*stcode);
    }

    return true;
}


void CrtSurfData()
{
    vsurfdata.clear();

    struct st_surfdata stsurfdata;

    char strLocalTime[21];
    LocalTime(strLocalTime,"yyyy-mm-dd hh24:mi");
    strcat(strLocalTime,":00");


    for(int i = 0; i < vstcode.size(); ++i)
    {
        memset(&stsurfdata,0,sizeof(struct st_surfdata));

        STRCPY(stsurfdata.obtid, 10, vstcode[i].obtid); // 站点代码

        STRCPY(stsurfdata.ddatetime, 20, strLocalTime); // 数据时间采用当前时间

        stsurfdata.t = rand() % 351;             // 气温：单位，0.1摄氏度
        stsurfdata.p = rand() % 265 + 10000;     // 气压：0.1百帕
        stsurfdata.u = rand() % 100 + 1;         // 相对湿度，0-100之间的值。
        stsurfdata.wd = rand() % 360;            // 风向，0-360之间的值。
        stsurfdata.wf = rand() % 150;            // 风速：单位0.1m/s
        stsurfdata.r = rand() % 16;              // 降雨量：0.1mm
        stsurfdata.vis = rand() % 5001 + 100000; // 能见度：0.1米

        vsurfdata.push_back(stsurfdata);
    }


}

bool CrtSurfFile(const char *outpath)
{
    CFile File;

    char strLocalTime[21];
    LocalTime(strLocalTime,"yyyymmddhh24miss");

    char strFileName[301];
    SNPRINTF(strFileName,300,"%s/SURF_ZH_%s_%d.txt",outpath,strLocalTime,getpid());

    if(File.OpenForRename(strFileName,"w") == false){logfile.Write("Open/Create surfile(%s) failed\n",strFileName);  return false;};

    for(int ii = 0; ii < vsurfdata.size(); ++ii)
    {
         File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    }

    logfile.Write("Create datafile(%s) successfully，datatime=%s，recordcount=%d！\n\n", strFileName, vsurfdata[0].ddatetime, vsurfdata.size());

    File.CloseAndRename();

    vsurfdata.clear();
    vstcode.clear();
    return true;
}