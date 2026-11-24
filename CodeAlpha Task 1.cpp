#include <iostream>
using namespace std;
int main()
{
    int courses;
    float grade, creditHours;
    float totalCredits = 0, totalGradePoints = 0;
    float semesterGPA, previousCGPA, previousCredits;
    float overallCGPA;
    cout<<"===== GPA & CGPA Calculator ====="<<endl;
    cout<<"Enter number of courses: ";
    cin>>courses;
    cout<<"Enter previous CGPA (if first semester enter 0): ";
    cin>>previousCGPA;
    cout<<"Enter previous total credit hours (if first semester enter 0): ";
    cin>>previousCredits;
    for(int i=1;i<=courses;i++)
    {
        cout<<"\nCourse "<<i<<endl;
        cout<<"Enter grade points (0 to 4): ";
        cin>>grade;
        cout<<"Enter credit hours: ";
        cin>>creditHours;
        totalCredits=totalCredits+creditHours;
        totalGradePoints=totalGradePoints+(grade*creditHours);
    }
    semesterGPA=totalGradePoints/totalCredits;
    overallCGPA=((previousCGPA*previousCredits)+totalGradePoints)
        /(previousCredits+totalCredits);
    cout<<"\n===== Result ====="<<endl;
    cout<<"Total Credit Hours of Current Semester: "<<totalCredits<<endl;
    cout<<"Total Grade Points: "<<totalGradePoints<<endl;
    cout<<"Semester GPA: "<<semesterGPA<<endl;
    cout<<"Overall CGPA: "<<overallCGPA<<endl;
    return 0;
}