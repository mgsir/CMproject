#include "_public.h"
#include "_ftp.h"

struct st_arg
{
  char remoteip[51];
  int port;
  char mode[11];
  char username[31];
  char password[31];
  char localpath[301];
  char remotepath[301];
  char matchname[301];
  char listfilename[301];
  char remotepathbak[301];
  char okfilename[301];
  int delremote;
  int ptype;
} starg;

struct st_fileinfo1
{
  char filename[301];
  char m_time[21];
};

CLogFile logfile;
Cftp ftp;

vector<st_fileinfo1> vlistfile;
vector<st_fileinfo1> voklistfile;
vector<st_fileinfo1> voklistfile1;
vector<st_fileinfo1> vlistfile1;

void _help();
void _xml2arg(const char *);
bool _ftpgetfile();
bool LoadListFile();
bool LoadOkFile();
void CompareFile();
void UpdateListFile();
bool WriteOkFileXml();

int main(int argc, char *argv[])
{

  if (argc != 3)
  {
    _help();
    return -1;
  }

  if (logfile.Open(argv[1], "a+") == false)
  {
    printf("logfile.open(%s,%s) failed\n", argv[1], "a+");
    return -1;
  }
  char strXmlBuffer[3001];
  strncpy(strXmlBuffer, argv[2], 3000);

  _xml2arg(strXmlBuffer);

  int mode = strcmp(starg.mode, "port") == 0 ? FTPLIB_PORT : FTPLIB_PASSIVE;
  if (ftp.login(starg.remoteip, starg.username, starg.password, mode) == false)
  {
    logfile.Write("ftp.login(%s,%s,%s)failed\n", starg.remoteip, starg.username, starg.password);
  }

  if (_ftpgetfile() == false)
  {
    logfile.Write("_ftpgetfile() failed\n");
    return -1;
  }

  return 0;
}

bool _ftpgetfile()
{
  if (ftp.chdir(starg.remotepath) == false)
  {
    logfile.Write("ftp.chdir(%s)failed\n", starg.remotepath);
    return false;
  }

  if (ftp.nlist(".", starg.listfilename) == false)
  {
    logfile.Write("ftp.nlist(%s,%s) failed!\n", ".", starg.listfilename);
    return false;
  }

  logfile.Write("ftp.login ok\n");

  if(LoadListFile() == false) return false;

  if (chdir(starg.localpath) == -1)
  {
    MKDIR(starg.localpath, false);
    if (chdir(starg.localpath) == -1)
    {
      logfile.Write("chdir(%s) failed!\n", starg.localpath);
      return false;
    }
  }

  CFile file;
  if(starg.ptype == 1)
  {
    LoadOkFile();

    // Add content to vlistfile1 and vokfile1
    CompareFile();

    UpdateListFile();

    WriteOkFileXml();

    if (file.Open(starg.okfilename, "a") == false)
    {
      logfile.Write("file.OPen(%s) failed\n", starg.okfilename);
      return false;
    }
  }

  char strRemotePath[301];
 
  for (int i = 0; i < vlistfile.size(); ++i) // Write file to local path
  {
    SNPRINTF(strRemotePath, 300, starg.remotepath, vlistfile[i].filename);
    logfile.Write("get %s ...\n", vlistfile[i].filename);
    if (ftp.get(vlistfile[i].filename, vlistfile[i].filename) == false)
    {
      logfile.WriteEx("failed\n");
      return false;
    }

    logfile.WriteEx("ok\n");

    if(starg.ptype == 1)
    {
      file.Fprintf("<filename>%s</filename><m_time>%s</m_time>\n",vlistfile[i].filename,vlistfile[i].m_time);
    }
    if (starg.ptype == 2) // Delete remote file
    {
      ftp.ftpdelete(strRemotePath);
    }

    if (starg.ptype == 3)
    {
      char strRemoteFileBackup[301];
      SNPRINTF(strRemoteFileBackup, 300, starg.remotepathbak, vlistfile[i].filename);
      ftp.ftprename(strRemotePath, vlistfile[i].filename);
    }
  }

  return true;
}
void _help()
{
  printf("\n");
  printf("Using:/htidc/public/bin/ftpgetfile logfilename xmlbuffer\n\n");
  printf("Sample: /htidc/public/bin/ftpgetfile  /log/shqx/ftpgetfile_surfdata.log \"<remoteip>39.108.135.217:21</remoteip><port>21</port><mode>pasv</mode><username>oracle</username><password>642517235</password><localpath>/data/ftp/surfdata</localpath><remotepath>/oracle/data/ftp/surfdata</remotepath><remotepathbak>/oracle/data/ftp/surfdatabackup</remotepathbak><matchname>SURF_*.TXT</matchname><listfilename>/data/shqx/ftplist/ftpgetfile_surfdate.list</listfilename><deleteremote>2</deleteremote><timeout>1800</timeout><timetvl>-8</timetvl><mtime>24</mtime><indexid>10001</indexid><ptype>1</ptype><okfilename>/data/ftp/ftplist/ftpgetfile_surfdata.xml</okfilename>\"\n\n\n");
}

void _xml2arg(const char * strXmlBuffer)
{
  memset(&starg, 0, sizeof(struct st_arg));
  GetXMLBuffer(strXmlBuffer, "remoteip", starg.remoteip, 50);
  GetXMLBuffer(strXmlBuffer, "port", &starg.port);
  GetXMLBuffer(strXmlBuffer, "mode", starg.mode, 10);
  GetXMLBuffer(strXmlBuffer, "username", starg.username, 30);
  GetXMLBuffer(strXmlBuffer, "password", starg.password, 30);
  GetXMLBuffer(strXmlBuffer, "localpath", starg.localpath, 300);
  GetXMLBuffer(strXmlBuffer, "remotepath", starg.remotepath, 300);
  GetXMLBuffer(strXmlBuffer, "listfilename", starg.listfilename, 300);
  GetXMLBuffer(strXmlBuffer, "matchname", starg.matchname, 300);
  GetXMLBuffer(strXmlBuffer, "deleteremote", &starg.delremote);
  GetXMLBuffer(strXmlBuffer, "ptype", &starg.ptype);
  GetXMLBuffer(strXmlBuffer, "remotepathbak", starg.remotepathbak, 300);
  GetXMLBuffer(strXmlBuffer,"okfilename",starg.okfilename,300);
}

bool LoadListFile()
{
  vlistfile.clear();
  CFile file;

  if(file.Open(starg.listfilename,"r") == false)
  {
    logfile.Write("file.Open(%s)",starg.listfilename); return false;
  }

  st_fileinfo1 fileinfo;
  
  char path[301];
  while(true)
  {
    memset(path,0,sizeof(path));
    memset(&fileinfo,0,sizeof(struct st_fileinfo1));
    if(file.Fgets(fileinfo.filename,300,true) == false) break;
    if(MatchFileName(fileinfo.filename,starg.matchname) == false)  continue;

    if(starg.ptype == 1)
    {
      if(ftp.mtime(fileinfo.filename) == false) 
      {
        logfile.Write("ftp.mtime(%s)failed %s\n",fileinfo.filename);
        return false;
      }
      strcpy(fileinfo.m_time,ftp.m_mtime);
    }

    vlistfile.push_back(fileinfo);
  }

  return true;
}

bool LoadOkFile()
{
  voklistfile.clear();

  CFile file;
  // in the begin, okfile don't exist 
  if(file.Open(starg.okfilename,"r") == false) return true;

  char strBuffer[301];
  st_fileinfo1 oklistfile;
  while(true)
  {
    memset(strBuffer,0,sizeof(strBuffer));
    memset(&oklistfile,0,sizeof(struct st_fileinfo1));
    if(file.Fgets(strBuffer,300,true) == false) break;
    GetXMLBuffer(strBuffer,"filename",oklistfile.filename);
    GetXMLBuffer(strBuffer,"m_time",oklistfile.m_time);
    voklistfile.push_back(oklistfile);
  }
  return true;
}

void CompareFile()
{
  voklistfile1.clear();
  vlistfile1.clear();

  for(int i = 0; i < vlistfile.size(); ++i)
  {
    int j = 0;
    for(; j < voklistfile.size(); ++j)
    {
      if(strcmp(vlistfile[i].filename,voklistfile[j].filename) == 0 && strcmp(vlistfile[i].m_time,voklistfile[j].m_time) == 0)  
      {
        voklistfile1.push_back(vlistfile[i]);
        break;
      }
    }

    if(j == voklistfile.size())
    {
      vlistfile1.push_back(vlistfile[i]);
    }
  }
}

  void UpdateListFile()
  {
    vlistfile = vlistfile1;
    voklistfile = voklistfile1;
    vlistfile1.clear();
    voklistfile1.clear();
  }

bool WriteOkFileXml()
{
  CFile file;
  if(file.Open(starg.okfilename,"w") == false)
  {
    logfile.Write("file.Open(%s) failed\n",starg.okfilename);
    return false;
  }

  for(int i = 0; i < voklistfile.size(); ++i)
  {
    file.Fprintf("<filename>%s</filename><m_time>%s</m_time>\n",voklistfile[i].filename,voklistfile[i].m_time);
  }
}

