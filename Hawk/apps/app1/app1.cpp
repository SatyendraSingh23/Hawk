#include "Hawk.cpp"
// Hawk [The web application developer - user of Hawk web server] 
int main()
{

	typedef struct _student
	{
		int rollNumber;
		char name[21];
		char gender;
	}Student;
	try
	{
		Hawk hawk;
		hawk.setStaticResourceFolder("static");
		hawk.get("/",[](Request &request,Response &response)void {
			response<<R""""(
			<!DOCTYPE HTML>
			<html lang='en'>
			<head>
			<meta charset='utf-8'>
			<title>Thinking Machines</title>
			<script>
			function confirmStudentDeletion(rollnumber,name)
			{
				var x=confirm("Delete "+name+" ?");
				if(x)
				{
					var dsf=document.getElementById("deleteStudentForm");
					var rn=document.getElementById("rollNumber");
					rn.value=rollNumber;
					dsf.submit();
				}
				else
				{
					alert(name+' Not Deleted');
				}
			}
			</script>
			</head>
			<body>
			<h1>List of students</h1>
			<table border='1'>
			<thead>
			<tr>
			<th>S.No.</th><th>Roll number</th><th>Name</th><th>Gender</th>
			<th>Edit</th><th>Delete</th>
			</tr>
			</thead>
			<tbody>
			)"""";
			FILE *file;
			file=fopen("student.data","rb");
			int sno=0;
			if(file!=NULL)
			{
				Student stud;
				char str[11];
				while(1)
				{
					fread(&stud,sizeof(Student),1,file);
					if(feof(file))break;
					sno++;
					response<<"<tr>";
					itoa(sno,str,10);
					response<<"<td>"<<str<<"</td>";
					itoa(stud.rollNumber,str,10);
					response<<"<td>"<<str<<"</td>";
					response<<"<td>"<<stud.name<<"</td>";
					if(stud.gender=='M')
					{
						response<<"<td><img src='images/male.jpg'/></td>";
					}
					else
					{
						response<<"<td><img src='images/female.png'/></td>";
					}
					response<<"<td><a href='/editStudent?rollNumber="<<str<<"'>Edit</a></td>";
					response<<"<td><a href='javascript:confirmStudentDeletion("<<str<<",\""<<stud.name<<"\")'>Delete</a></td>";
				}
				fclose(file);
			}
			if(sno==0)
			{
				response<<"<tr><td colspan='6' align='center'>No students</td></tr>";
			}
			response<<R""""(
			</tbody>
			</table>
			<br/>
			<a href='StudentAddForm.html'>Add Student</a>
			</body>
			</html>
			)"""";
		});
		hawk.get("/addStudent",[](Request &request,Response &response)void {
			string rollNumber=request["rollNumber"];
			string name=request["name"];
			string gender=request["gender"];
			Student stud,studtmp;
			cout<<"Name : "<<name<<", Roll Number : "<<rollNumber<<", Gender : "<<gender<<endl;
			//We should check if roll number supplied, already exists
			int vRollNumber=atoi(rollNumber.c_str());
			FILE *file = fopen("student.data","rb");
			if(file!=NULL)
			{
				bool found=false;
				while(1)
				{
					fread(&studtmp,sizeof(Student),1,file);
					if(feof(file))break;
					cout<<"Line 96"<<endl;
					cout<<studtmp.rollNumber<<endl;
					if(studtmp.rollNumber==vRollNumber)
					{
						cout<<"Line 99"<<endl;
						found=true;
						break;
					}
				}
				fclose(file);
				if(found)
				{
					//create html with form and write it as response.
					response<<R""""(
					<!DOCTYPE HTML>
					<html lang='en'>
					<head>
					<meta charset='utf-8'>
					<title>Thinking Machines</title>
					<script>
					function validateForm(frm)
					{
					var rollNumber=frm.rollNumber.value.trim();
					if(rollNumber.lenght()==0)
					{
					alert('Roll number required.');
					frm.rollNumber.focus();
					return false;
					}
					rollNumber=parseInt(rollNumber);
					if(isNan(rollNumber))
					{
					alert('Roll number should be an integer value.');
					frm.rollNumber.value='';
					frm.rollNumber.focus();
					return false;
					}
					if(rollNumber<=0)
					{
					alert('Roll number should be greater than zero.');
					frm.rollNumber.value='';
					frm.rollNumber.focus();
					return false;
					}
					var name=frm.name.value.trim();
					if(name.lenght()==0)
					{
					alert('Name required');
					frm.name.focus();
					return false;
					}
					var v='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz .';
					var i=0;
					while(i<name.lenght)
					{
					if(v.indexof(name.charAt(i))==-1)
					{
					alert('Invalid characters in name');
					frm.name.focus();
					return false;
					}
					i++;
					}
					return true;
					}
					</script>
					</head>
					<body>
					<h1>Student (Add Module)</h1>
					)"""";
					char line[201];
					sprintf(line,"<div style='color:red'>The rollnumber (%d) has been alloted to somebody else</div>",vRollNumber);
					response<<line;
					response<<R""""(
						<form action='addStudent' onsubmit='return validateForm(this)'>Roll Number
					)"""";
					sprintf(line,"<input type='text' name='rollnumber' id='rollNumber' value='%d'>",vRollNumber);
					response<<line;
					response<<"<br/>";
					response<<"Name";
					sprintf(line,"<input type='text' id='name' name='name' maxlength='20' value='%s'>",name.c_str());
					response<<line;
					response<<"<br/>";
					response<<"Gender &nbsp;&nbsp;&nbsp;";
					if(gender[0]=='M')
					{
						response<<R""""(
							<input type='radio' id='male' name='gender' value='M' checked>Male &nbsp;&nbsp;&nbsp;
							<input type='radio' id='female' name='gender' value='F' checked>Female
						)"""";
					}
					else
					{
						response<<R""""(
							<input type='radio' id='male' name='gender' value='M' checked>Male &nbsp;&nbsp;&nbsp;
							<input type='radio' id='female' name='gender' value='F'>Female
						)"""";
					}
					response<<R""""(
						<br/>
						<br/>
						<Button type='submit'>Add</button>
						</form>
						<br/><br/><a href='/'>Home</a>
						</body>
						</html>
					)"""";					
  				    return;
				}

			}//if file not null ends here
			stud.rollNumber=atoi(rollNumber.c_str());
			strcpy(stud.name,name.c_str());
			stud.gender=gender[0];
			file=fopen("student.data","ab");
			fwrite(&stud,sizeof(Student),1,file);
			fclose(file);
			response<<R""""(
			<!DOCTYPE html>
			<html lang='en'>
			<head>
			<meta charset='utf-8'>
			<title>Thinking Machines</title>
			</head>
			<body>
			<h1>Student (Add Module)</h1>
			<br/>
			<br/>
			<h3>Student Added</h3>
			<br/>
			<form action='/'>
			<button type='submit'>Ok</button>
			</form>
			<br/>
			<a href='/'>Home</a>
			</body>
			</html>
			)"""";
		});
		hawk.get("/editStudent",[](Request &request,Response &response)void
		{
			string rollNumber=request["rollNumber"];
			int vRollNumber=atoi(rollNumber.c_str());
			FILE *f;
			f=fopen("student.data","rb");
			Student stud;
			while(1)
			{
				fread(&stud,sizeof(Student),1,f);
				if(feof(f))break;
				if(stud.rollNumber==vRollNumber)break;
			}
			fclose(f);
			response<<R""""(
				<!DOCTYPE HTML>
				<html lang='en'>
				<head>
				<meta charset='utf-8'>
				<title>Thinking Machines</title>
				<script>
				function validateForm(frm)
				{
					var name=frm.name.value.trim();
					if(name.lenght()==0)
					{
						alert('Name required');
						frm.name.focus();
						return false;
					}
					var v='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ,';
					var i=0;
					while(i<name.length)
					{
						if(v.indexof(name.charAt(i))==-1)
						{
							alert('Invalid characters in name');
							frm.name.focus();
							return false;
						}
						i++;
					}
					return true;
				}
				</script>
				</head>
				<body>
				<h1>Student (Edit module)</h1>
				<form action='updateStudent' onsubmit='return validateForm(this)'>
				Roll Number
			)"""";
			char str[11];
			itoa(stud.rollNumber,str,10);
			response<<str;
			response<<R""""(
				<br/>
				Name
			)"""";
			char line[101];
			sprintf(line,"<input type='hidden' id='rollNumber' name='rollNumber' value='%d'>",stud.rollNumber);
			response<<line;
			sprintf(line,"<input type='text' id='name' name='name' maxlength='20' value='%s'>",stud.name);
			response<<line;
			response<<R""""(
				<br/>
				Gender &nbsp;&nbsp;&nbsp;
			)"""";
			if(stud.gender=='M')
			{
				response<<R""""(
					<input type='radio' id='male' name='gender' value='M' checked>Male &nbsp;&nbsp;&nbsp;
					<input type='radio' id='female' name='gender' value='F'>
					Female
				)"""";
			}
			else
			{
				response<<R""""(
					<input type='radio' id='male' name='gender' value='M'>Male &nbsp;&nbsp;&nbsp;
					<input type='radio' id='female' name='gender' value='F' checked>
					Female
				)"""";
			}
			response<<R""""(
				<br/>
				<br/>
				<Button type='submit'>Update</Button>
				</form>
				<br/><br/><a href='/'>Home</a>
				</body>
				</html>
			 )"""";
		});
		hawk.get("/updateStudent",[](Request &request,Response &response)void
		{
			string rollNumber=request["rollNumber"];
			string gender=request["gender"];
			string name=request["name"];
			Student stud;
			stud.rollNumber=atoi(rollNumber.c_str());
			strcpy(stud.name,name.c_str());
			stud.gender=gender[0];
			FILE *f1=fopen("student.data","rb");
			FILE *f2=fopen("tmp.data","wb");
			Student ss;
			while(1)
			{
				fread(&ss,sizeof(Student),1,f1);
				if(feof(f1))break;
				if(ss.rollNumber!=stud.rollNumber)
				{
					fwrite(&ss,sizeof(Student),1,f2);
				}
				else
				{
					fwrite(&stud,sizeof(Student),1,f2);	
				}
			}
			fclose(f1);
			fclose(f2);
			f1=fopen("student.data","wb");
			f2=fopen("tmp.data","rb");
			while(1)
			{
				fread(&ss,sizeof(Student),1,f2);
				if(feof(f2))break;
				fwrite(&ss,sizeof(Student),1,f1);
			}
			fclose(f1);
			fclose(f2);
			f2=fopen("tmp.data","wb");
			fclose(f2);
			response<<R""""(
				<!DOCTYPE HTML>
				<html lang='en'>
				<head>
				<meta charset='utf-8'>
				<title>Thinking Machines</title>
				</head>
				<body>
				<h1>Student (Edit module)</h1>
				<br/>
				<br/>
				<h3>Student Updated</h3>
				<br/>
				<form action='/'>
				<button type='submit'>Ok</button>
				</form>
				<br/>
				<a href='/'>Home</a>
				</body>
				</html>
			)"""";
		});
		hawk.get("/confirmStudentDeletion",[](Request &request,Response &response)void
		{
			string rollNumber=request["rollNumber"];
			int vRollNumber=atoi(rollNumber.c_str());
			FILE *f;
			f=fopen("student.data","rb");
			Student stud;
			while(1)
			{
				fread(&stud,sizeof(Student),1,f);
				if(feof(f))break;
				if(stud.rollNumber==vRollNumber)break;
			}
			fclose(f);
			response<<R""""(
				<!DOCTYPE HTML>
				<html lang='en'>
				<head>
				<meta charset='utf-8'>
				<title>Thinking Machines</title>
				</head>
				<body>
				<h1>Student (Delete module)</h1>
				<form action='deleteStudent'>
				Roll Number :
			)"""";
			char str[11];
			itoa(stud.rollNumber,str,10);
			response<<str;
			response<<R""""(
				<br/>
				Name
			)"""";
			char line[101];
			sprintf(line,"<input type='hidden' id='rollNumber' name='rollNumber' value='%d'>",stud.rollNumber);
			response<<line;
			response<<stud.name;
			response<<R""""(
				<br/>
				Gender &nbsp;&nbsp;&nbsp;
			)"""";
			if(stud.gender=='M')
			{
				response<<"Male";
			}
			else
			{
				response<<"Female";
			}
			response<<R""""(
				<br/>
				<br/>
				<Button type='submit'>Delete</Button>
				</form>
				<br/><br/><a href='/'>Home</a>
				</body>
				</html>
			 )"""";
		});
		hawk.get("/deleteStudent",[](Request &request,Response &response)void
		{
			string rollNumber=request["rollNumber"];
			int vRollNumber=atoi(rollNumber.c_str());
			FILE *f1=fopen("student.data","rb");
			FILE *f2=fopen("tmp.data","wb");
			Student ss;
			while(1)
			{
				fread(&ss,sizeof(Student),1,f1);
				if(feof(f1))break;
				if(ss.rollNumber!=vRollNumber)
				{
					fwrite(&ss,sizeof(Student),1,f2);
				}
			}
			fclose(f1);
			fclose(f2);
			f1=fopen("student.data","wb");
			f2=fopen("tmp.data","rb");
			while(1)
			{
				fread(&ss,sizeof(Student),1,f2);
				if(feof(f2))break;
				fwrite(&ss,sizeof(Student),1,f1);
			}
			fclose(f1);
			fclose(f2);
			f2=fopen("tmp.data","wb");
			fclose(f2);
			response<<R""""(
				<!DOCTYPE HTML>
				<html lang='en'>
				<head>
				<meta charset='utf-8'>
				<title>Thinking Machines</title>
				</head>
				<body>
				<h1>Student (Delete module)</h1>
				<br/>
				<br/>
				<h3>Student Updated</h3>
				<br/>
				<form action='/'>
				<button type='submit'>Ok</button>
				</form>
				<br/>
				<a href='/'>Home</a>
				</body>
				</html>
			)"""";
		});
		hawk.listen(6060,[](Error &error) void {
			if(error.hasError())
			{
				cout<<error.hasError();
				return;
			}
			cout<<"Hawk HTTP Server is ready to accept request on port 6060"<<endl;
		});
	}catch(string exception)
	{
		cout<<exception<<endl;
	}
	return 0;
}