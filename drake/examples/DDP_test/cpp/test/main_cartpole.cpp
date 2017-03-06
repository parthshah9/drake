#include <iostream>
#include <fstream>

#include "config.h"
#include "matrixUtil.h"

#include "ilqrsolver.h"
#include "cart_pole.h"
#include "cost_function_cart_pole.h"

#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace Eigen;

#define pi M_PI

int main()
{
    struct timeval tbegin,tend;
    double texec = 0.0;
    stateVec_t xinit,xgoal;

    xinit << 0.0,0.0,0.0,0.0;
    xgoal << 0.0,pi,0.0,0.0;

    double T = TimeHorizon;
    double dt = TimeStep;
    unsigned int N = (int)T/dt;
    double tolFun = 1e-5;//[relaxing default value: 1e-10];
    double tolGrad = 1e-5;//[relaxing default value: 1e-10];
    unsigned int iterMax = 150;

    stateVecTab_t xList;
    commandVecTab_t uList;
    ILQRSolver::traj lastTraj;
    
    CartPole cartPoleModel(dt, N);
    CostFunctionCartPole costCartPole;
    ILQRSolver testSolverCartPole(cartPoleModel,costCartPole,ENABLE_FULLDDP,ENABLE_QPBOX);
    testSolverCartPole.FirstInitSolver(xinit, xgoal, N, dt, iterMax, tolFun, tolGrad);

    // run one or multiple times and then average
    int Num_run = 1;
    gettimeofday(&tbegin,NULL);
    for(int i=0;i<Num_run;i++) testSolverCartPole.solveTrajectory();
    gettimeofday(&tend,NULL);

    lastTraj = testSolverCartPole.getLastSolvedTrajectory();
    xList = lastTraj.xList;
    uList = lastTraj.uList;
    unsigned int iter = lastTraj.iter;

    texec=((double)(1000*(tend.tv_sec-tbegin.tv_sec)+((tend.tv_usec-tbegin.tv_usec)/1000)))/1000.;
    texec /= Num_run;

    cout << endl;
    cout << "Number of iterations: " << iter << endl;
    cout << "Final cost: " << lastTraj.finalCost << endl;
    cout << "Final gradient: " << lastTraj.finalGrad << endl;
    cout << "Final lambda: " << lastTraj.finalLambda << endl;
    cout << "Execution time by time step (second): " << texec/N << endl;
    cout << "Execution time per iteration (second): " << texec/iter << endl;
    cout << "Total execution time of the solver (second): " << texec << endl;
    cout << "\tTime of derivative (second): " << lastTraj.time_derivative.sum() << " (" << 100.0*lastTraj.time_derivative.sum()/texec << "%)" << endl;
    cout << "\tTime of backward pass (second): " << lastTraj.time_backward.sum() << " (" << 100.0*lastTraj.time_backward.sum()/texec << "%)" << endl;
    cout << "\tTime of forward pass (second): " << lastTraj.time_forward.sum() << " (" << 100.0*lastTraj.time_forward.sum()/texec << "%)" << endl;
    
    ofstream file("results.csv",ios::out | ios::trunc);

    if(file)
    {
        file << "x,theta,xDot,thetaDot,u" << endl;
        for(int i=0;i<N;i++) file << xList[i](0,0) << "," << xList[i](1,0) << "," << xList[i](2,0) << "," << xList[i](3,0) << "," << uList[i](0,0) << endl;
        file << xList[N](0,0) << "," << xList[N](1,0) << "," << xList[N](2,0) << "," << xList[N](3,0) << "," << 0.0 << endl;
        file.close();
    }
    else
        cerr << "error in open file" << endl;

    return 0;

}
