#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sstream>

using namespace std;


struct process
{
    string name_process;
    int arrival_t;
    int service_t;
    int finish_t;
    int wait_t;
    int turnaround;
    int remaining;
    float norm_turnaround;
    int active[20];
    int ready=0;

};

void display_stats(vector<process> processes, float tmean, float ntmean);
void display_trace(vector<process> processes);
void prem_trace(vector<process> processes);

bool compareProcesses(process p1, process p2)
{
    return (p1.arrival_t < p2.arrival_t);
}

void rr_analysis(vector<process> processes, string mode, int time_slot, int quantum)
{
    int c = processes.size();
    queue<process> start;
    queue<process> q;
    queue<process> finish;
    float turn_mean=0;
    float Nturn_mean=0.0;
    process temp;

    for(int i=0; i<c; i++)
    {
        processes[i].remaining=processes[i].service_t;//to keep track of remaining service time
        start.push(processes[i]);
    }


    for(int i = 0; i < time_slot;)
    {
        //cout<<i<<" that's one iteration"<<endl;
        for(int j = 0; j < c; j++)
        {
            if(start.size()!=0)
                if(start.front().arrival_t <= i)
                {
                    q.push(start.front());
                    start.pop();
                }
        }

        if(!q.empty())
        {
            process x = q.front();
            q.pop();
            if(x.remaining<quantum)
            {   for(int act=0;act<x.remaining;act++){
                    x.active[i+act]=2;

                }
                x.finish_t=i+x.remaining;
                i+=x.remaining;
                x.remaining=0;
                //cout<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
                continue;
            }
            x.remaining -= quantum;
            if(x.remaining != 0)
            {   for(int act=0;act<quantum;act++){
                    x.active[i+act]=2;

                }
                //cout<<"there is still some more to go current i= "<<i<<x.name_process<<x.remaining<<endl;
                for(int j = 0; j < c; j++)
                {
                    if(start.size()!=0)
                        if(start.front().arrival_t <= i+quantum)
                        {
                            q.push(start.front());
                            start.pop();
                        }
                }
                q.push(x);
            }
            else
            {    for(int act=0;act<quantum;act++){
                    x.active[i+act]=2;

                }
                //cout<<"process finished the last quantum current i= "<<i<<x.name_process<<x.remaining<<endl;
                x.finish_t=i+quantum;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
            //cout<<i<<endl;
            i+=quantum;
            //cout<<i<<endl;
        }

    }
    for(int i=0; i<c; i++)
    {
        if(finish.empty())
        {
        }
        else
        {
            process x =finish.front();
            processes[i]=x;
            finish.pop();
        }
    }
    sort(processes.begin(), processes.end(), compareProcesses);
    turn_mean = turn_mean/(float)c;
    Nturn_mean = Nturn_mean/ (float)c;
    if(mode=="stats")
    {
        cout<<"RR-"<<quantum<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }
    else if(mode=="trace")
    {  for(int timer=0;timer<time_slot;timer++){
            for(int k=0;k<c;k++){

                if(processes[k].arrival_t<=timer && processes[k].active[timer]!=2 && processes[k].finish_t>timer){
                    processes[k].active[timer]=1;
                }
            }}
        cout<<"RR-"<<quantum;

        int lstinst=time_slot;
        int n=c;
        for(int k=0; k<=lstinst; k++)
        {
            int m=k;
            if(m>9)
            {
                m=m%10;
            }
            cout<<" "<<m;
        }cout<<" "<<endl;
        cout<<"\n------------------------------------------------"<<endl;
        for(int i=0;i<n;i++) {
            cout<<"  \n"<<processes[i].name_process<<"     |";
            for(int j=0;j<lstinst;j++){

                if(processes[i].active[j]==0){
                    cout<<" |";

                }
                else if(processes[i].active[j]==1){
                    cout<<".|";

                }
                else if(processes[i].active[j]==2){
                    cout<<"*|";

                }
            }cout<<" "<<endl;


        }
        cout<<"\n------------------------------------------------"<<endl;
        cout<<""<<endl;

    }

}

void srn_analysis(vector<process> processes, string mode,int lstinst)
{
    float turn_mean=0;
    float Nturn_mean=0.0;
    int n = processes.size();
    for (int i = 0; i < n; i++)
    {
        processes[i].remaining = processes[i].service_t;
    }
    int complete = 0, t = 0, minm = INT_MAX;
    int shortest = 0, finish_time;
    bool check = false;


    while (complete != n)
    {

        // Find process with minimum
        // remaining time among the
        // processes that arrives till the
        // current time`
        for (int j = 0; j < n; j++)
        {
            if ((processes[j].arrival_t <= t) &&(processes[j].remaining < minm) && processes[j].remaining > 0)
            {
                minm = processes[j].remaining;
                shortest = j;
                check = true;
            }
        }

        if (check == false)
        {
            t++;
            continue;
        }

        // Reduce remaining time by one
        processes[shortest].remaining--;

        // Update minimum
        minm = processes[shortest].remaining;
        if (minm == 0)
            minm = INT_MAX;

        // If a process gets completely
        // executed
        if (processes[shortest].remaining == 0)
        {

            // Increment complete
            complete++;
            check = false;

            // Find finish time of current
            // process
            processes[shortest].finish_t = t + 1;



            // Calculate waiting time
            processes[shortest].wait_t = processes[shortest].finish_t -processes[shortest].service_t -processes[shortest].arrival_t;

            if (processes[shortest].wait_t < 0)
                processes[shortest].wait_t = 0;
        }
        processes[shortest].active[t]=2;

        // Increment time
        t++;
    }
    for(int timer=0;timer<lstinst;timer++){
        for(int k=0;k<n;k++){

            if(processes[k].arrival_t<=timer && processes[k].active[timer]!=2 && processes[k].finish_t>timer){
                processes[k].active[timer]=1;
            }
        }}

    for (int i = 0; i < n; i++){
        processes[i].turnaround = processes[i].service_t + processes[i].wait_t;
        turn_mean += processes[i].turnaround;

        // Calculation of Normalized Turn Around Time
        processes[i].norm_turnaround = ((float)processes[i].turnaround / processes[i].service_t);
        // Sum Normalized Turn Around Time
        Nturn_mean += processes[i].norm_turnaround;
    }
    turn_mean = turn_mean/(float)n;
    Nturn_mean = Nturn_mean/ (float)n;
    if(mode=="stats")
    {
        cout<<"SRT"<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }


    else if(mode=="trace")
    {
        cout<<"SRT   ";


        for(int k=0; k<=lstinst; k++)
        {
            int m=k;
            if(m>9)
            {
                m=m%10;
            }
            cout<<" "<<m;
        }
        cout<<"\n------------------------------------------------"<<endl;
        for(int i=0;i<n;i++) {
            cout<<"  \n"<<processes[i].name_process<<"      |";
            for(int j=0;j<lstinst;j++){

                if(processes[i].active[j]==0){
                    cout<<" |";

                }
                else if(processes[i].active[j]==1){
                    cout<<".|";

                }
                else if(processes[i].active[j]==2){
                    cout<<"*|";

                }
            }


        }
        cout<<"\n------------------------------------------------"<<endl;
        cout<<""<<endl;


    }

}

void feedback(vector<process> processes,int lstinst, string mode)
{
    float turn_mean=0;
    float Nturn_mean=0.0;
    queue<process> finish;
    queue<process>q0;
    queue<process>q1;
    queue<process>q2;
    queue<process>q3;
    queue<process>q4;
    queue<process>q5;
    int i,timer;
    int n = processes.size();
    int completed[n];

    for(i=0; i<n; i++)
    {
        processes[i].remaining=processes[i].service_t;//to keep track of remaining service time
        q0.push(processes[i]);//storing for checking about arrival time

    }

    for(i=0; i<lstinst; i++)
    {
        for(int j=0; j<n&&!q0.empty(); j++)//check for new arrival
        {
            if(q0.front().arrival_t==i)
            {
                q1.push(q0.front()); //q0=E  q1= q2=D q3=BC
                //cout << "at i= " <<i<<" process "<<q0.front().name_process<<" arrived"<<endl;
                q0.pop();
            }
            else
                break;
        }
        if(!q1.empty())
        {
            process x = q1.front();
            //cout<<"process "<<x.name_process<<"in front of q1 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q1.pop();
            x.remaining --;
            x.active[i]=2;
            if(x.remaining != 0 && q0.front().arrival_t!=i+1 && q2.empty() )
            {
                q1.push(x);
            }
            else if(x.remaining != 0)
            {
                q2.push(x);
            }
            else
            {
                x.finish_t=i+1;
                //cout<<"process "<<x.name_process<<" dispatched from q1 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }

        }
        else if(!q2.empty())
        {
            process x = q2.front();
            x.active[i]=2;
            //cout<<"process "<<x.name_process<<"in front of q2 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q2.pop();
            x.remaining --;
            if(x.remaining != 0)
            {
                q3.push(x);
            }
            else
            {
                x.finish_t=i+1;
                //cout<<"process "<<x.name_process<<" dispatched from q2 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
        }
        else if(!q3.empty())
        {
            process x = q3.front();
            x.active[i]=2;
            //cout<<"process "<<x.name_process<<"in front of q3 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q3.pop();
            x.remaining --;
            if(x.remaining != 0)
            {
                q4.push(x);
            }
            else
            {
                x.finish_t=i+1;
                //cout<<"process "<<x.name_process<<" dispatched from q3 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
        }
        else if(!q4.empty())
        {
            process x = q4.front();
            x.active[i]=2;
            //cout<<"process "<<x.name_process<<"in front of q4 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q4.pop();
            x.remaining --;
            if(x.remaining != 0)
            {
                q5.push(x);
            }
            else
            {
                x.finish_t=i+1;
                //cout<<"process "<<x.name_process<<" dispatched from q4 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
        }
        else if(!q5.empty())
        {

            q5.front().remaining--;
            process x = q5.front();
            x.active[i]=2;
            q5.pop();
            //cout<<"process "<<x.name_process<<"in front of q5 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            if(x.remaining==0)
            {
                //cout<<"process "<< x.name_process<<" dispatched from q5 at i = "<<i<<endl;
                x.finish_t=i+1;
                x.wait_t=x.finish_t-x.arrival_t-x.service_t;
                x.turnaround=x.finish_t-x.arrival_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
            else
            {
                q5.push(x);
            }
        }

    }
    turn_mean = turn_mean/(float)n;
    Nturn_mean = Nturn_mean/ (float)n;

    for(i=0; i<n; i++)
    {
        if(finish.empty())
        {
        }
        else
        {
            process x =finish.front();
            processes[i]=x;
            finish.pop();
        }
    }
    sort(processes.begin(), processes.end(), compareProcesses);
    if(mode=="stats")
    {
        cout<<"FB-1"<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }
    else if(mode=="trace")
    {
        cout<<"FB-1";
        for(int timer=0;timer<lstinst;timer++){
            for(int k=0;k<n;k++){

                if(processes[k].arrival_t<=timer && processes[k].active[timer]!=2 && processes[k].finish_t>timer){
                    processes[k].active[timer]=1;
                }
            }}


        for(int k=0; k<=lstinst; k++)
        {
            int m=k;
            if(m>9)
            {
                m=m%10;
            }
            cout<<" "<<m;
        }
        cout<<"\n------------------------------------------------"<<endl;
        for(int i=0;i<n;i++) {
            cout<<"  \n"<<processes[i].name_process<<"     |";
            for(int j=0;j<lstinst;j++){

                if(processes[i].active[j]==0){
                    cout<<" |";

                }
                else if(processes[i].active[j]==1){
                    cout<<".|";

                }
                else if(processes[i].active[j]==2){
                    cout<<"*|";

                }
            }


        }
        cout<<"\n------------------------------------------------"<<endl;
        cout<<""<<endl;

    }
}


void feedback2(vector<process> processes,int lstinst, string mode)
{
    float turn_mean=0;
    float Nturn_mean=0.0;
    queue<process> finish;
    queue<process>q0;
    queue<process>q1;
    queue<process>q2;
    queue<process>q3;
    queue<process>q4;
    queue<process>q5;
    int quantum=1;
    int i,timer;
    int n = processes.size();
    int completed[n];

    for(i=0; i<n; i++)
    {
        processes[i].remaining=processes[i].service_t;//to keep track of remaining service time
        q0.push(processes[i]);//storing for checking about arrival time

    }
    i=0;
    for(i=0; i<lstinst;)
    {
        for(int j=0; j<n&&!q0.empty(); j++)//check for new arrival
        {
            if(q0.front().arrival_t<=i)
            {   //cout<<q0.front().name_process<<" : "<<" j \n";

                q1.push(q0.front());
                //cout << "at i= " <<i<<" process "<<q0.front().name_process<<" arrived"<<endl;
                q0.pop();
            }
            else
                break;
        }
        if(!q1.empty())
        {
            quantum=1;
            process x;
            x = q1.front();
            //cout<<"process "<<x.name_process<<"in front of q1 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q1.pop();
            x.remaining --;
            //cout<<x.name_process;
            x.active[i]=2;

            if(x.remaining != 0 && q0.front().arrival_t!=i+1 && q2.empty() )
            {
                q1.push(x);
            }
            else if(x.remaining != 0)
            {
                q2.push(x);
            }
            else
            {
                x.finish_t=i;
                //cout<<"process "<<x.name_process<<" dispatched from q1 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
            i=i+1;
        }
        else if(!q2.empty())
        {   quantum=2;

            process x = q2.front();
            // cout<<x.name_process;

            if(x.remaining>=quantum){
                int qr=i+quantum;
                for(int qq=i;qq<qr;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    // cout<<x.name_process;
                }
                i=qr;
            }
            else {
                int r=i+x.remaining;
                for(int qq=i;qq<r;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    // cout<<x.name_process;
                }
                i=r;
            }


            //cout<<"process "<<x.name_process<<"in front of q2 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q2.pop();
            // x.remaining --;
            if(x.remaining != 0)
            {
                q3.push(x);
            }
            else
            {
                x.finish_t=i;
                //cout<<"process "<<x.name_process<<" dispatched from q2 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }

        }
        else if(!q3.empty())
        {   quantum=4;

            process x = q3.front();
            //cout<<x.name_process;
            if(x.remaining>=quantum){
                int qr=i+quantum;
                for(int qq=i;qq<qr;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    // cout<<x.name_process;
                }
                i=qr;
            }
            else {
                int r=i+x.remaining;
                for(int qq=i;qq<r;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    // cout<<x.name_process;
                }
                i=r;
            }
            //cout<<"process "<<x.name_process<<"in front of q3 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q3.pop();
            //x.remaining --;
            if(x.remaining != 0)
            {
                q4.push(x);
            }
            else
            {
                x.finish_t=i;
                //cout<<"process "<<x.name_process<<" dispatched from q3 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
        }
        else if(!q4.empty())
        {   quantum=8;

            process x = q4.front();
            // cout<<x.name_process;
            if(x.remaining>=quantum){
                int qr=i+quantum;
                for(int qq=i;qq<qr;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    //  cout<<x.name_process;
                }
                i=qr;
            }
            else {
                int r=i+x.remaining;
                for(int qq=i;qq<r;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    //   cout<<x.name_process;
                }
                i=r;
            }

            //cout<<"process "<<x.name_process<<"in front of q4 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            q4.pop();
            //x.remaining --;
            if(x.remaining != 0)
            {
                q5.push(x);
            }
            else
            {
                x.finish_t=i;
                //cout<<"process "<<x.name_process<<" dispatched from q4 at i = "<<i<<endl;
                x.turnaround=x.finish_t-x.arrival_t;
                x.wait_t=x.turnaround-x.finish_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
        }
        else if(!q5.empty())
        {
            quantum=16;
            q5.front().remaining--;
            process x = q5.front();
            //  cout<<x.name_process;
            if(x.remaining>=quantum){
                int qr=i+quantum;
                for(int qq=i;qq<qr;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    //  cout<<x.name_process;
                }
                i=qr;
            }
            else {
                int r=i+x.remaining;
                for(int qq=i;qq<r;qq++){
                    x.active[qq]=2;
                    x.remaining--;
                    // cout<<x.name_process;
                }
                i=r;
            }
            q5.pop();
            //cout<<"process "<<x.name_process<<"in front of q5 with remaining time = "<<x.remaining<<" at i= "<<i<<endl;
            if(x.remaining==0)
            {
                //cout<<"process "<< x.name_process<<" dispatched from q5 at i = "<<i<<endl;
                x.finish_t=i;
                x.wait_t=x.finish_t-x.arrival_t-x.service_t;
                x.turnaround=x.finish_t-x.arrival_t;
                x.norm_turnaround=x.turnaround/(float)x.service_t;
                turn_mean += x.turnaround;
                Nturn_mean += x.norm_turnaround;
                finish.push(x);
            }
            else
            {
                q5.push(x);
            }
        }

    }
    turn_mean = turn_mean/(float)n;
    Nturn_mean = Nturn_mean/ (float)n;

    for(i=0; i<n; i++)
    {
        if(finish.empty())
        {
        }
        else
        {
            process x =finish.front();
            processes[i]=x;
            finish.pop();
        }
    }
    sort(processes.begin(), processes.end(), compareProcesses);
    if(mode=="stats")
    {
        cout<<"FB-2i"<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }
    else if(mode=="trace")
    {
        cout<<"FB-2i";
        for(int timer=0;timer<lstinst;timer++){
            for(int k=0;k<n;k++){

                if(processes[k].arrival_t<=timer && processes[k].active[timer]!=2 && processes[k].finish_t>timer+1){
                    processes[k].active[timer]=1;
                }
            }}


        for(int k=0; k<=lstinst; k++)
        {
            int m=k;
            if(m>9)
            {
                m=m%10;
            }
            cout<<" "<<m;
        }
        cout<<"\n------------------------------------------------"<<endl;
        for(int i=0;i<n;i++) {
            cout<<"  \n"<<processes[i].name_process<<"     |";
            for(int j=0;j<lstinst;j++){

                if(processes[i].active[j]==0){
                    cout<<" |";

                }
                else if(processes[i].active[j]==1){
                    cout<<".|";

                }
                else if(processes[i].active[j]==2){
                    cout<<"*|";

                }
            }


        }
        cout<<"\n------------------------------------------------"<<endl;
        cout<<""<<endl;

    }
}


void hrrn_analysis(vector<process> processes, string mode, int lstinst)
{
    int n = processes.size(),i;
    float t;
    float turn_mean=0;
    float Nturn_mean=0.0;
    int complete[n];
    for(i=0; i<n; i++)
    {
        //initialize the complete status array to zeros
        complete[i]=0;
    }
    sort(processes.begin(), processes.end(), compareProcesses);
    for (t = processes[0].arrival_t; t < lstinst;)
    {
        //initialize the formula at first to a very small number
        float hrr = -9999;
        float temp;
        int index;
        for (i = 0; i < n; i++)
        {
            if (processes[i].arrival_t <= t && complete[i]!= 1)
            {
                //calculating the highest response ratio formula
                temp = (processes[i].service_t + (t - processes[i].arrival_t)) / processes[i].service_t;
                //iterate through the processes to find the maximum ratio response
                if (hrr < temp)
                {
                    hrr = temp;
                    // Storing Location
                    index = i;
                }
            }
        }
        t += processes[index].service_t;
        processes[index].wait_t = t - processes[index].arrival_t - processes[index].service_t;
        processes[index].finish_t = t;

        // Calculation of Turn Around Time
        processes[index].turnaround = t - processes[index].arrival_t;
        // Sum Turn Around Time
        turn_mean += processes[index].turnaround;

        // Calculation of Normalized Turn Around Time
        processes[index].norm_turnaround = ((float)processes[index].turnaround / processes[index].service_t);
        // Sum Normalized Turn Around Time
        Nturn_mean += processes[index].norm_turnaround;

        // Updating Completion Status
        complete[index] = 1;
    }
    turn_mean = turn_mean/(float)n;
    Nturn_mean = Nturn_mean/ (float)n;
    if(mode=="stats")
    {
        cout<<"HRRN"<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }
    else if(mode=="trace")
    {
        cout<<"HRRN";


        display_trace(processes);
    }

}

bool compareService(process p1, process p2)
{
    return (p1.service_t < p2.service_t);
}

void Spn_analysis(vector<process> processes, string mode,int lstinst)
{
    int n = processes.size();

    float turn_mean=0;
    float Nturn_mean=0.0;
    processes[0].wait_t = 0;


    int start_time[n];
    start_time[0] = processes[0].arrival_t;
    vector<process> sorted = processes;
    sort(sorted.begin(), sorted.end(), compareService);
    vector<process> proc ;
    sorted[-1].finish_t=0;
    int complete[n];
    int ready[n];


    for(int l=0; l<n; l++)
    {
        complete[l]=0;
        ready[l]=0;
    }
    int j=0;
    for( j; j<lstinst; ) //loop on time
    {
        int index;
        for(int i=0; i<n; i++)
        {
            if(sorted[i].arrival_t<=j && complete[i]==0) //j=0 //i=0 E , i=1 A , C , D , B
            {
                //cout<<sorted[i].name_process;
                index=i; // 1,
                start_time[i]=j;
                index=i;
                sorted[i].finish_t=start_time[i]+sorted[i].service_t;
                sorted[i].wait_t = start_time[i] - sorted[i].arrival_t;

                sorted[i].turnaround=sorted[i].service_t+sorted[i].wait_t;
                sorted[i].norm_turnaround=sorted[i].turnaround/(1.0*sorted[i].service_t);
                turn_mean+=sorted[i].turnaround;
                Nturn_mean+=sorted[i].norm_turnaround;
                //cout<<sorted[i].name_process<<" : "<<start_time[i]<<" : "<<sorted[i].finish_t<<"\n";

                break;
            }



        }
        complete[index]=1; //complete[1]=1

        j=j+sorted[index].service_t; //j=3
    }
    sort(sorted.begin(), sorted.end(), compareProcesses);




    turn_mean=turn_mean/(float)n;
    Nturn_mean=Nturn_mean/(float)n;
    if(mode=="stats")
    {
        cout<<"SPN "<<endl;
        display_stats(sorted, turn_mean, Nturn_mean);
    }
    if(mode=="trace")
    {
        cout<<"SPN ";


        display_trace(sorted);
    }



}

void fcfs_analysis(vector<process> processes, string mode)
{
    int n = processes.size();

    float turn_mean=0;
    float Nturn_mean=0.0;
    processes[0].wait_t = 0;
    int service_time[n];
    service_time[0] = processes[0].arrival_t;
    for (int  i = 1; i < n ; i++ )
    {
        service_time[i] = service_time[i-1] + processes[i-1].service_t;

        processes[i].wait_t = service_time[i] - processes[i].arrival_t;
    }
    for(int i=0; i<n; i++)
    {

        processes[i].turnaround=processes[i].service_t+processes[i].wait_t;
        processes[i].norm_turnaround=processes[i].turnaround/(1.0*processes[i].service_t);
        processes[i].finish_t= processes[i].arrival_t+processes[i].turnaround;
        turn_mean+=processes[i].turnaround;
        Nturn_mean+=processes[i].norm_turnaround;
    }
    turn_mean=turn_mean/(float)n;
    Nturn_mean=Nturn_mean/(float)n;
    if(mode=="stats")
    {
        cout<<"FCFS"<<endl;
        display_stats(processes, turn_mean, Nturn_mean);
    }
    else if(mode=="trace")
    {
        cout<<"FCFS ";


        display_trace(processes);
    }

}

void aging_analysis(vector<process> processes,string mode,int quantum,int lstinst){
    int n=processes.size();
    int priorty[n];
    int original_priorty[n];
    int index=-1;
    int x=lstinst;
//    queue<processes> q;


    vector<process> sorted = processes;
    process max_prio=processes[0];
    max_prio.service_t=-1;
    process current_process;


    for(int i=0;i<n;i++){
        priorty[i]=processes[i].service_t;

    }
    int j=0;
    for ( j;j<lstinst;){

        for(int i=0;i<n;i++){

            if(processes[i].arrival_t<=j && index!=i){

                processes[i].ready=1;
                processes[i].service_t++;
                //  q.push(processes[i]);




            }}


        for(int l=0;l<n;l++){

            if(processes[l].service_t > max_prio.service_t && processes[l].ready==1 && index!=l){
                max_prio=processes[l];
                index=l;


            }

           // if(processes[0].service_t==processes[1].service_t && max_prio.service_t==processes[0].service_t ) {
             //   max_prio = processes[0];
               // index = 0;
            //}
              for(int g=l;g<n-1;g++){
              if(processes[g].service_t==processes[1].service_t && max_prio.service_t==processes[l].service_t){
                    max_prio=processes[l];
                   index=l;
               }
              }
        }
        /* for(int l=0;l<n;l++){
             if(processes[index].service_t==processes[l].service_t && index>l)
             {   index=l;
                 cout<<"equal at : "<<j;
             }
         }*/



        for(int q=j;q<j+quantum;q++){
            processes[index].active[q]=2;

            processes[index].service_t=priorty[index];
            max_prio.service_t=priorty[index];


            for(int k=0;k<n;k++){

                if(processes[k].arrival_t<=q && processes[k].active[q]!=2){
                    processes[k].active[q]=1;
                }

            }
        }


        j=j+quantum;
    }



    cout<<"Aging";



    for(int k=0; k<=lstinst; k++)
    {
        int m=k;
        if(m>9)
        {
            m=m%10;
        }
        cout<<" "<<m;
    }
    cout<<"\n------------------------------------------------";
    for(int i=0;i<n;i++) {
        cout<<"  \n"<<processes[i].name_process<<"     |";
        for(int j=0;j<lstinst;j++){

            if(processes[i].active[j]==0){
                cout<<" |";

            }
            else if(processes[i].active[j]==1){
                cout<<".|";

            }
            else if(processes[i].active[j]==2){
                cout<<"*|";

            }
        }


    }
    cout<<"\n------------------------------------------------";



}


void display_trace(vector<process> processes)
{ int finish_time=0;
    int n=processes.size();
    for(int l=0;l<n;l++){
        finish_time+=processes[l].service_t;
    }

    for(int k=0; k<=finish_time; k++)

    {
        int m=k;
        if(m>9)
        {
            m=m%10;
        }
        cout<<" "<<m;
    }cout<<" "<<endl;
    cout<<"\n------------------------------------------------"<<endl;


    for(int i=0; i<n; i++)
    {



        cout<<"  \n"<<processes[i].name_process<<"     |";

        for(int j=0; j<finish_time; j++)
        {
            if(j<processes[i].finish_t && j>=(processes[i].finish_t-processes[i].service_t))
            {
                cout<<"*|";
            }
            else if(j>=processes[i].arrival_t && j<=(processes[i].finish_t-processes[i].service_t) )
            {
                cout<<".|";
            }
            else
            {
                cout<<" |";
            }

        }
        cout<<" "<<endl;
    }
    cout<<"\n------------------------------------------------"<<endl;
    cout<<""<<endl;

}
void prem_trace(vector<process> processes,int lstinst ){
    int finish_time=0;
    int n=processes.size();
    for(int l=0;l<n;l++){
        finish_time+=processes[l].service_t;
    }

    for(int k=0; k<=lstinst; k++)
    {
        int m=k;
        if(m>9)
        {
            m=m%10;
        }
        cout<<" "<<m;
    }
    cout<<"\n------------------------------------------------";
    for(int i=0;i<n;i++) {
        cout<<"  \n"<<processes[i].name_process<<"    |";
        for(int j=0;j<finish_time;j++){

            if(processes[i].active[j]==0){
                cout<<" |";

            }
            else if(processes[i].active[j]==1){
                cout<<".|";

            }
            else if(processes[i].active[j]==2){
                cout<<"*|";

            }
        }


    }
    cout<<"\n------------------------------------------------"<<endl;
    cout<<""<<endl;



}


void display_stats(vector<process> processes, float tmean, float ntmean)
{
    std::cout << std::setprecision(2) << std::fixed;
    int n = processes.size();
    cout<<"Process    |";
    for(int i=0; i<n; i++){
        cout<<"  "<<processes[i].name_process<<"  |";
    }
    cout<<endl;
    cout<<"Arrival    |";
    for(int i=0; i<n; i++){
        if(processes[i].arrival_t<9)
            cout<<"  "<<processes[i].arrival_t<<"  |";
        else
            cout<<" "<<processes[i].arrival_t<<"  |";
    }
    cout<<endl;
    cout<<"Service    |";
    for(int i=0; i<n; i++){
        if(processes[i].service_t<9)
            cout<<"  "<<processes[i].service_t<<"  |";
        else
            cout<<" "<<processes[i].service_t<<"  |";
    }
    cout<<" Mean|"<<endl;
    cout<<"Finish     |";
    for(int i=0; i<n; i++){
        if(processes[i].finish_t>9)
            cout<<" "<<processes[i].finish_t<<"  |";
        else
            cout<<"  "<<processes[i].finish_t<<"  |";
    }
    cout<<"-----|"<<endl;
    cout<<"Turnaround |";
    for(int i=0; i<n; i++){
        if(processes[i].turnaround>9)
            cout<<" "<<processes[i].turnaround<<"  |";
        else
            cout<<"  "<<processes[i].turnaround<<"  |";
    }
    if(tmean<9)
        cout<<" "<<tmean<<"|"<<endl;
    else
        cout<<""<<tmean<<"|"<<endl;
    cout<<"NormTurn   |";
    for(int i=0; i<n; i++){
        if(processes[i].norm_turnaround<9)
            cout<<" "<<processes[i].norm_turnaround<<"|";
        else
            cout<<""<<processes[i].norm_turnaround<<"|";
    }
    if(ntmean<9)
        cout<<" "<<ntmean<<"|"<<endl;
    else
        cout<<""<<ntmean<<"|"<<endl;
    cout<<""<<endl;

}




int main()
{
    int lstinst,n;
    string mode;
    cin >> mode;
    string algo;
    cin >> algo;

    stringstream ip(algo);
    string temp;
    vector<string> parsed;
    while(getline(ip,temp,',')){
        parsed.push_back(temp);
    }

    int s= parsed.size();
    int algo_id[s],quantum[s];
    for(int i=0;i<s;i++){
        if(parsed[i][1]=='-')
        {
            algo_id[i]=parsed[i][0]-'0';
            quantum[i]=parsed[i][2]-'0';
        }
        else
            algo_id[i]=parsed[i][0]-'0';
    }
    cin >> lstinst;
    cin >> n;
    vector<process> processes;
    string inter;
    for(int i=0; i<n; i++)
    {
        cin>>inter;
        processes.push_back(process());
        string str(1,inter[0]);
        processes[i].name_process=str;

        processes[i].arrival_t=inter[2]-'0';
        processes[i].service_t=inter[4]-'0';
    }
    for(int i=0;i<s;i++){
        if(algo_id[i]==1)
        {
            fcfs_analysis(processes,mode);
        }
        else if(algo_id[i]==2)
        {
            rr_analysis(processes, mode, lstinst,quantum[i]);
        }
        else if(algo_id[i]==3){
            Spn_analysis(processes,mode,lstinst);
        }
        else if(algo_id[i]==5)
        {
            hrrn_analysis(processes,mode,lstinst);
        }
        else if(algo_id[i]==4)
        {
            srn_analysis(processes, mode,lstinst);
        }
        else if(algo_id[i]==6)
        {
            feedback(processes,lstinst, mode);
        }
        else if(algo_id[i]==7){
            feedback2(processes,lstinst,mode);
        }
        else if (algo_id[i]==8){
            aging_analysis(processes,mode,quantum[i],lstinst);
        }
    }
    return 0;
}
