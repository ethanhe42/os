#include <iostream>
#include <list>
#include <string>

using namespace std;

#define NADDR 3
#define PWOSIZ 32
#define NICFREE 10
#define NICINOD 10
#define DIRNUM 8
#define NOFILE 10

struct inode;

//目录索引节点
struct direct
{
	string d_name;             /*目录名*/
	inode* pointer;               /*目录号*/
};

struct dir
{
	struct direct directory[DIRNUM];
	int size;
};

//块描述符
struct inode{
	struct inode  *i_forw = NULL;
	struct inode  *i_back = NULL;
	bool I_flag;
	unsigned long i_into;      /*磁盘i节点标号*/
	unsigned int di_number; /*关联文件书，当为0时，则删除该文件*/

	unsigned int di_mode;   /*存取权限*/
	unsigned int di_uid;    /*磁盘i节点用户*/
	unsigned int di_gid;    /*磁盘i节点组*/

	//unsigned int di_addr[NADDR];          /*物理块号*/

	bool isDir;
	dir* directory;
	char data[100];
	unsigned int i_size;

	inode(long into,int uid,int gid,int mode){
		i_into = into;
		i_forw = NULL;
		i_back = NULL;
		i_size = 0;
		di_uid = uid;
		di_gid = gid;

		di_mode = mode;
	}
};

struct pwd
{
	unsigned int P_uid;
	unsigned int P_gid;
	char passward[PWOSIZ];
};

struct hinode
{
	struct inode *iforw;
};

struct file
{
	char f_flag; /*文件操作标志*/
	unsigned int f_count; /*引用计数*/
	struct inode *f_inode; /*指向内存节点*/
	unsigned long f_off; /*读/写指针*/
};

class user
{
private:
	string user_name;
	string pwd;
	bool active;
	unsigned int u_default_mode;
	unsigned int u_uid; /*用户标志*/
	unsigned int u_gid; /*用户组标志*/
	unsigned int u_ofile[NOFILE]; /*用户打开表*/
	string cur_postion;
public:
	user(string name, string pw, short uid, short gid, short mode)
	{
		user_name = name;
		pwd = pw;
		u_default_mode = mode;
		u_uid = uid;
		u_gid = gid;
		active = false;
	}

	string getName(){ return user_name; }

	int getUsrid(){ return u_uid; }

	int gerGrpid(){ return u_gid; }

	bool login_account(string password)
	{
		if (pwd == password)
		{
			active = true;
			return true;
		}
		else{
			return false;
		}
	}

	void setActive(bool act){
		active = act;
	}
	//~user();
};

class filsys
{
private:
	//unsigned int  s_isize;            /*i节点块块数*/
	//unsigned long s_fsize;             /*数据块块数*/

	//unsigned int s_nfree;             /*空闲块块数*/
	//unsigned int s_pfree;           /*空闲块指针*/
	//unsigned int s_free[NICFREE];     /*空闲块堆栈*/


	unsigned long s_ninode;            /*空闲i节点数*/

	list<inode*>::iterator s_pinode;          /*空闲i节点指针*/
	list<inode*> inode_list;

	unsigned int s_inode[NICINOD];   /*空闲i节点数组*/
	unsigned int s_rinode;           /*铭记i节点*/
	char s_fmod;                    /*超级块修改标志*/

	inode* cur_positon;

public:
	filsys(){
		s_ninode = 0;
		//s_pinode = 0;
		cur_positon = 0;
	}

	bool initial(long size,int uid,int gid,int mode){
		s_ninode = size;
		for (int i = 0; i < size; i++){
			inode_list.push_back(new inode(i,uid,gid,mode));
		}
		inode* root_dir=inode_list.front();
		root_dir->directory = new dir;
		root_dir->directory->size = 0;
		root_dir->isDir = true;

		s_ninode = size - 1;
		cur_positon = root_dir;

		list<inode*>::iterator temp =inode_list.begin();
		s_pinode = ++temp;
		return true;
	}

	inode* firstFreeInode(){
		if (s_ninode > 0){
			return *s_pinode;
		}
		else {
			cout << "The disk is full!" << endl;
			return NULL;
		}
	}

	void refresh_pinode(){
		if (s_ninode > 0)
		{
			int i = s_ninode;
			list<inode*>::iterator temp = s_pinode;
			(*temp)->I_flag = true;
			while ((*temp)->I_flag == true && i > 0){
				temp++;
				i--;
			}
			s_pinode = temp;
			s_ninode--;
		}
	}

	void retrieveInode(inode* node){
		if (*s_pinode > node){
			while ((*s_pinode) != node)
				s_pinode--;
			s_ninode++;
		}
		else{
			s_ninode++;
		}
	}

	void mkdir(string dir_name){
		if (cur_positon->isDir){
			int& addpoint = cur_positon->directory->size;
			if (addpoint == DIRNUM) cout << "The directory is full!" << endl;
			else{
				cur_positon->directory->directory[addpoint].d_name = dir_name;
				inode* temp=firstFreeInode();
				if (temp){
					temp->isDir = true;
					temp->i_forw = cur_positon;
					temp->I_flag = true;
					temp->directory = new dir;
					temp->directory->size = 0;
					cur_positon->directory->directory[addpoint].pointer = temp;
					addpoint++;
					refresh_pinode();
				}
				else cout << "Can not creat a directory!" << endl;
			}
		}
		else cout << "Can not creat a directory!" << endl;
	}

	void cd(string dir_name){
		if (cur_positon->isDir){
			direct* temp;
			int i = 0;
			for (i = 0; i<cur_positon->directory->size; ){
				temp =&( cur_positon->directory->directory[i]);
				if (temp->d_name == dir_name)
				{
					cur_positon = temp->pointer;
					return;
				}
				else if (temp->pointer != NULL){ i++; }
			}
			if (i == cur_positon->directory->size)
				cout << "Can not find " << dir_name << endl;
			else cout << "current position: " << dir_name << endl;
		}
		else{
			cout << "Failed!" << endl;
		}
	}

	void chdir(){
		if (cur_positon != inode_list.front()){
			cur_positon = cur_positon->i_forw;
		}
	}

	void showDir(){
		if (cur_positon->isDir){
			int i = 0;
			direct temp;
			for (i = 0; i < DIRNUM; i++){
				temp = cur_positon->directory->directory[i];
				if (temp.d_name=="") continue;
				if (temp.pointer->isDir)
				{
					cout << "directory  " << temp.pointer->di_uid << "  " << temp.pointer->di_gid << "  " << temp.d_name << endl;
				}
				else {
					cout << "file      " << temp.pointer->di_uid << "  " << temp.pointer->di_gid << "  " << temp.d_name << endl;
				}

			}
			if (i == 0) cout << "The directory is empty!";
		}
		else cout << "This is not a directory!";
	}

	void creatFile(string file_name,int size){
		if (cur_positon->isDir && s_ninode > size){
			int& addpoint = cur_positon->directory->size;
			if (addpoint == DIRNUM) cout << "The directory is full!" << endl;
			else{
				cur_positon->directory->directory[addpoint].d_name = file_name;
				inode* temp = firstFreeInode();
				if (temp){
					temp->isDir = false;
					//cur_positon->i_back = temp;
					temp->i_forw = cur_positon;
					temp->I_flag = true;
					temp->data[0] = '\0';
					cur_positon->directory->directory[addpoint].pointer = temp;
					addpoint++;
					refresh_pinode();
					size--;
					for (size; size > 0; size--){
						temp->i_back = firstFreeInode();
						temp->isDir = false;
						temp->i_forw->i_forw = cur_positon;
						temp->I_flag = true;
						temp->i_back->data[0] = '\0';
						refresh_pinode();
						temp = temp->i_back;
					}
				}
				else cout << "Can not creat a file!" << endl;
			}
		}
		else cout << "Can not creat a file!" << endl;
	}

	void closeFile(){
		cur_positon = cur_positon->i_forw;
	}

	void extendFile(inode* filenode){
		if (s_ninode > 0){
			filenode->i_back = firstFreeInode();
			filenode->i_back->isDir = false;
			filenode->i_back->i_forw = filenode->i_forw;
			filenode->i_back->I_flag = true;
			filenode->i_back->data[0] = '\0';
			refresh_pinode();
		}
		else{ cout << "Disk is full!" << endl; }
	}

	void editFile(inode* node){
		inode* filenode = node;
		cout << endl;
		string context;
		cin >> context;
		int count = context.size() / 100 ;
		for (int i = 0; i < count+1; i++)
		{
			for (int k = 0; k < 100; k++){
				if (100 * i + k<context.size()){
					filenode->data[k] = context.at(100 * i + k);
					filenode->i_size++;
				}
				else{
					filenode->data[k] = '\0';
					break;
				}
			}
			if (filenode->i_back != NULL) filenode = filenode->i_back;
			else{
				extendFile(filenode);
				filenode = filenode->i_back;
			}
		}
	}

	void printFileContext(inode* filenode){
		inode* temp = filenode;
		while (temp){
			cout << temp->data;
			temp = temp->i_back;
		}
	}

	void appendFileContext(inode* filenode){
		inode* temp = filenode;
		cout << endl;
		string context;
		cin >> context;
		int num = context.size();
		for (int i = 0; i < num; ){
			if (temp->i_size < 100){
				temp->data[temp->i_size++] = context.at(i);
				i++;
			}
			else if (temp->i_back != NULL){
				temp = temp->i_back;
			}
			else{
				extendFile(temp);
				temp = temp->i_back;
			}
		}
		if (temp->i_size<100) temp->data[temp->i_size++] ='\0';
	}

	void fileOpMenu(inode* filenode)
	{
		bool flag = true;
		while (flag)
		{
			cout << endl << "choose one of the actions below:(1 to 4)" << endl;
			int i = 0;
			i++;
			cout << i << ".\t" << "correct content" << endl;
			i++;
			cout << i << ".\t" << "show content" << endl;
			i++;
			cout << i << ".\t" << "add content" << endl;
			i++;
			cout << i << ".\t" << "exit file" << endl;
			int select;
			cin >> select;
			switch (select)
			{
			case 1:
				editFile(filenode);
				break;
			case 2:
				printFileContext(filenode);
				break;
			case 3:
				appendFileContext(filenode);
				break;
			case 4:
				flag = false;
				cur_positon = cur_positon->i_forw;
				break;
			default:
				break;
			}
		}
	}

	inode* findFileorDir(string name){
		if (cur_positon->isDir){
			direct* temp;
			int i = 0;
			int num = cur_positon->directory->size;
			for (i; i < DIRNUM;i++){
				temp = &(cur_positon->directory->directory[i]);
				if (temp->d_name == name){
					return temp->pointer;
				}
				/*else if (temp->pointer != NULL){
					i++;
				}*/
			}
		}
		else return NULL;
	}

	void delFileorDirinDir(string name){
		if (cur_positon->isDir){
			direct* temp;
			int i = 0;
			int num = cur_positon->directory->size;
			for (i; i < DIRNUM;i++){
				temp = &(cur_positon->directory->directory[i]);
				if (temp->d_name == name){
					cur_positon->directory->directory[i].d_name = "";
					cur_positon->directory->directory[i].pointer = NULL;
					cur_positon->directory->size--;
					return;
				}
				/*else if (temp->pointer != NULL){
				}*/
			}
		}
		else return;
	}

	void openFile(string file_name){
		if (cur_positon->isDir){
			direct* temp;
			int i = 0;
			int num = cur_positon->directory->size;
			for (i = 0; i<DIRNUM;i++){
				temp = &(cur_positon->directory->directory[i]);
				if (temp->d_name == file_name){
					cur_positon = temp->pointer;
					fileOpMenu(cur_positon);
					return;
				}
				//else if (temp->pointer != NULL){ i++; }
			}
			if (i == cur_positon->directory->size-1)
				cout << "Can not find " << file_name << endl;
			else cout << "current position: " << file_name << endl;
		}
		else{
			cout << "Failed!" << endl;
		}
	}

	void deleteFile(){
		cout << endl << "input file name:";
		string fname;
		cin >> fname;
		inode* fnode=findFileorDir(fname);
		if (fnode){
			while (fnode){
				fnode->data[0] = '\0';
				fnode->I_flag = false;
				fnode->i_forw = NULL;
				inode* temp = fnode->i_back;
				fnode->i_back = NULL;
				fnode->i_size = 0;
				retrieveInode(fnode);
				fnode = temp;
				delFileorDirinDir(fname);
			}
		}
		else{
			cout <<endl<< fname << " can not found." << endl;
		}
	}

	void deleteDir(){
		cout << endl << "input directory name:";
		string dname;
		cin >> dname;
		inode* fnode = findFileorDir(dname);
		if (fnode && fnode->di_number!=0){
			delete fnode->directory;
			fnode->isDir = false;
			fnode->i_back = NULL;
			fnode->I_flag = false;
			delFileorDirinDir(dname);
		}
		else{
			cout << "failed" << endl;
		}
	}
};

struct dinode
{
	unsigned int di_number;        /*关联文件数*/
	unsigned int di_mode;          /*存取权限*/

	unsigned int di_uid;
	unsigned int di_gid;
	unsigned long di_size;                /*文件大小*/
	unsigned int di_addr[NADDR];          /*物理块号*/
};

class disk:public filsys
{
private:
	dinode disk_discription;
	bool raw;
public:
	disk(long size, int uid, int gid, int mode) :filsys()
	{
		disk_discription.di_size = size;
		disk_discription.di_number = 0;

		disk_discription.di_uid = uid;
		disk_discription.di_gid = gid;

		disk_discription.di_mode = mode;

		raw = true;
	}
	void creatFileSystem(){
		initial(disk_discription.di_size,disk_discription.di_uid,disk_discription.di_gid,disk_discription.di_mode);
		raw = false;
	}

	int getUid(){ return disk_discription.di_uid; }

	bool getRaw(){ return raw; }
};


class sys_process
{
private:
	int user_num;
	int group_num;
	list<user*> user_list;

	int disk_num;
	list<disk*> disk_list;
public:
	sys_process()
	{
		user_num = 0;
		group_num = 0;
	}
	//~sys_process();
	user* creatUser(string name,string pwd,short mode=0)
	{
		user_num++;
		group_num++;
		user* temp = new user(name, pwd, user_num, group_num, mode);
		user_list.push_back(temp);
		return temp;
	}

	user* findUser(string name)
	{
		list<user*>::iterator temp;
		for (temp = user_list.begin(); temp != user_list.end();temp++)
		{
			if ( (*temp)->getName() == name) return *temp;
		}
		if (temp == user_list.end()) return NULL;
		return NULL;
	}

	disk* creatDisk(long size,user usr,int mode=0){
		disk_num++;
		disk* temp = new disk(size, usr.getUsrid(), usr.gerGrpid(), mode);
		disk_list.push_back(temp);
		return temp;
	}

	disk* findDisk(user* usr)
	{
		list<disk*>::iterator temp;
		for (temp = disk_list.begin(); temp != disk_list.end(); temp++)
		{
			if ((*temp)->getUid() == usr->getUsrid()) return *temp;
		}
		if (temp == disk_list.end()) return NULL;
		return NULL;
	}

	void menu(user* usr)
	{
		disk* di = findDisk(usr);
		if (di == NULL){
			cout << "no disk found!you have to creat a new partition!" << endl;
			cout << "input disk size:";
			long size;
			cin >> size;
			di=creatDisk(size, *usr);
		}
		bool flag = true;
		while (flag)
		{
			cout << endl<<"choose one of the actions below:(1 to 11)" << endl;
			int i = 0;
			i++;
			cout << i << ".\t" << "create dir" << endl;
			i++;
			cout << i << ".\t" << "entry dir" << endl;
			i++;
			cout << i << ".\t" << "return" << endl;
			i++;
			cout << i << ".\t" << "show file" << endl;
			i++;
			cout << i << ".\t" << "create file" << endl;
			i++;
			cout << i << ".\t" << "open file" << endl;
			i++;
			cout << i << ".\t" << "delete file" << endl;
			i++;
			cout << i << ".\t" << "delete dir" << endl;
			i++;
			cout << i << ".\t" << "create system file" << endl;
			i++;
			cout << i << ".\t" << "create disk partition" << endl;
			i++;
			cout << i << ".\t" << "log out" << endl;
			int select;
			string name;
			cin >> select;
			if (di->getRaw() && select!=9){
				cout << "The disk is raw,you have to creat a filesystem first!" << endl;
				continue;
			}
			switch (select)
			{
			case 1:
				cout << endl << "input directory name:";
				cin >> name;
				di->mkdir(name);
				break;
			case 2:
				cout << endl << "input directory name:";
				cin >> name;
				di->cd(name);
				break;
			case 3:
				di->chdir();
				break;
			case 4:
				di->showDir();
				break;
			case 5:
				cout << endl << "input file name:";
				cin >> name;
				di->creatFile(name, 2);
				break;
			case 6:
				cout << endl<<"input file name:";
				cin >> name;
				di->openFile(name);
				break;
			case 7:
				di->deleteFile();
				break;
			case 8:
				di->deleteDir();
				break;
			case 9:
				cout << endl << "creat filesysytem" << endl;
				di->creatFileSystem();
				break;
			case 10:
				cout << endl << "input disk size:";
				long size;
				cin >> size;
				di = creatDisk(size, *usr);
				break;
			case 11:
				usr->setActive(false);
				flag = false;
				break;
			default:
				select = 0;
				break;
			}
		}
	}

	void daemon()
	{
		string log_name, log_pwd;
		cout << "login:";
		cin >> log_name;
		cout << endl;
		user* login=findUser(log_name);
		if (login==NULL)
		{
			cout << log_name << " is not exist,do you want to creat an user with this name?(Y or N)" << endl;
			char c;
			cin >> c;
			if (c == 'y' || c == 'Y')
			{
				cout << "Please input password:";
				cin >> log_pwd;
				cout << "mode:(0 or 1)";
				int mode = 0;
				cin >> mode;
				login=creatUser(log_name, log_pwd, mode);
			}
		}
		else
		{
			while (1){
				cout << "password:";
				cin >> log_pwd;
				if (log_pwd == "exit") break;
				else if (login->login_account(log_pwd)){
					cout << "Welcome, " << log_name << endl;
					login->setActive(true);
					break;
				}
				else{
					cout << "password failed!" << endl;
				}
			}
		}
		menu(login);
	}
};

int main()
{
	sys_process sys;
	sys.daemon();
	return 0;
}