
#include "_public.h"
#include "_ooci.h"

struct st_surfdata
{
  char obtid[11];     
  char ddatetime[21];  
  int  t;             
  int  p;              
  int  u;             
  int  wd;             
  int  wf;             
  int  r;             
  int  vis;           
};



CDir Dir;
CLogFile logfile;
connection conn;

void _help();
bool _psurfdata();

int main(int argc, char *argv[])
{
    if(argc != 5){ _help(); return -1;}


    if(logfile.Open(argv[2],"a+") == false)
    {
        printf("Open log file failed\n");
        return false;
    }

    logfile.Write("The procedure start\n");

    int  gap = atoi(argv[4]);
    
     while(true)
     {
        if(Dir.OpenDir(argv[1],"SURF_ZH*.txt",1000,true,true) == false)
        {
            logfile.Write("DirOpenDir(%s) failed\n",argv[1]); 
            sleep(gap);
            continue;
        }

        while(true) // deal with all files where it are read by Dir
        {
            if(Dir.ReadDir() == false) break; // all files are handled

            if (conn.m_state == 0)
            {
                if (conn.connecttodb(argv[3], "AMERICAN_AMERICA.ZHS16GBK") != 0)
                {
                    logfile.Write("conn.connectotdb(%s) failed\n %s\n", argv[3], conn.m_cda.message);
                    break;
                }
            }

            logfile.Write("start to write single file(%s) to database\n",Dir.m_FullFileName);

            if(_psurfdata() == false)
            {
                logfile.WriteEx("_psurfdate() failure\n");
                break;
            }
            logfile.WriteEx("success\n");
        }


        if(conn.m_state == 1) conn.disconnect();
        sleep(gap);
    }
}   


void _help()
{
    printf("\nThis program is used to produce to  database \"T_SURFDATA\"table from local datafile\n");
    printf("Using: /htidc/shqx/bin/psurfdata datafilename logfilename database_args procedure_gap\n");
    printf("/htidc/shqx/bin/psurfdata /data/ftp/surfdata /log/shqx/psurfdata.log  scott/tiger@snorcl11g_217 10\n");
}

bool _psurfdata() // handle single data file
{
    CFile file;
    if(file.Open(Dir.m_FullFileName,"r") == false)
    {
        logfile.Write("file.Open(%s) failed\n",Dir.m_FileName);
        return false;
    }

    struct  st_surfdata stsurfdata;
    int iccount = 0;
    // bind input and ouput variable
    sqlstatement stmtsel(&conn);
    sleep(2);
    stmtsel.prepare("select count(*) from T_SURFDATA where obtid=:1 and ddatetime=to_date(:2,'yyyy-mm-dd hh24:mi:ss')");
    stmtsel.bindin(1, stsurfdata.obtid, 5);
    stmtsel.bindin(2, stsurfdata.ddatetime, 19);
    stmtsel.bindout(1, &iccount);

    sqlstatement stmtins(&conn);
    sleep(2);
    stmtins.prepare("insert into T_SURFDATA(obtid,ddatetime,t,p,u,wd,wf,r,vis,crttime,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,:5,:6,:7,:8,:9,sysdate,SEQ_SURFDATA.nextval)");
    stmtins.bindin(1,stsurfdata.obtid,5);
    stmtins.bindin(2,stsurfdata.ddatetime,19);
    stmtins.bindin(3,&stsurfdata.t);
    stmtins.bindin(4,&stsurfdata.p);
    stmtins.bindin(5, &stsurfdata.u);
    stmtins.bindin(6, &stsurfdata.wd);
    stmtins.bindin(7, &stsurfdata.wf);
    stmtins.bindin(8, &stsurfdata.r);
    stmtins.bindin(9, &stsurfdata.vis);

    CCmdStr CmdStr;
    char strBuffer[301];

    while(true)
    {
        memset(strBuffer,0,sizeof(strBuffer));
        memset(&stsurfdata,0,sizeof(struct st_surfdata));
        
        if(file.Fgets(strBuffer,300,true) == false)  break;
        CmdStr.SplitToCmd(strBuffer,",",true);
        if(CmdStr.CmdCount() != 9)
        {
            logfile.Write("%s\n",strBuffer);
            continue;
        }

        CmdStr.GetValue(0,stsurfdata.obtid,5);
        CmdStr.GetValue(1,stsurfdata.ddatetime,19);
        double dtmp = 0;
        CmdStr.GetValue(2, &dtmp);
        stsurfdata.t = (int)(dtmp * 10);
        CmdStr.GetValue(3, &dtmp);
        stsurfdata.p = (int)(dtmp * 10);
        CmdStr.GetValue(4, &stsurfdata.u); 
        CmdStr.GetValue(5, &stsurfdata.wd); 
        CmdStr.GetValue(6, &dtmp);
        stsurfdata.wf = (int)(dtmp * 10); 
        CmdStr.GetValue(7, &dtmp);
        stsurfdata.r = (int)(dtmp * 10);
        CmdStr.GetValue(8, &dtmp);
        stsurfdata.vis = (int)(dtmp * 10); 

        if(stmtsel.execute() != 0)
        {
            if(stmtsel.m_cda.rc >= 3113 && stmtsel.m_cda.rc <= 3115) return false;
            logfile.Write("stmtsel.execute() failed\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
            continue;
        }

        iccount = 0;
        stmtsel.next();
        if(iccount > 0) continue;


        if(stmtins.execute() != 0)
        {
            if(stmtins.m_cda.rc != 1)
            {
                if(stmtins.m_cda.rc >= 3113 && stmtins.m_cda.rc <= 3115) return false;
                logfile.Write("stmtins.execute() failed\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message); 
                logfile.WriteEx("inserted strData: %s\n",strBuffer);
                continue;
            }
        }
    }

    conn.commit();
    file.CloseAndRemove(); // close file and remove it
    return true;
}

