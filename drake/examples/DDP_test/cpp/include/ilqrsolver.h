#ifndef ILQRSOLVER_H
#define ILQRSOLVER_H

#include "config.h"
#include "matrixUtil.h"
#include "cart_pole.h"
#include "cost_function_cart_pole.h"

#include "dynamicmodel.h"
#include "costfunction.h"
#include <Eigen/Dense>
#include <Eigen/StdVector>
#include <qpOASES.hpp>
#include <qpOASES/QProblemB.hpp>

#define ENABLE_QPBOX 1
#define DISABLE_QPBOX 0
#define ENABLE_FULLDDP 1
#define DISABLE_FULLDDP 0

#ifndef DEBUG_ILQR
#define DEBUG_ILQR 1
#else
    #if PREFIX1(DEBUG_ILQR)==1
    #define DEBUG_ILQR 1
    #endif
#endif

#define TRACE(x) do { if (DEBUG_ILQR) printf(x);} while (0)

#define INIT_OPTSET {0, 0, NULL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, NULL, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0, 0, 0, NULL, NULL, NULL, {0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0} // NULL, NULL

//double default_alpha[8]= {1.0, 0.3727594, 0.1389495, 0.0517947, 0.0193070, 0.0071969, 0.0026827, 0.0010000};

// typedef struct optSet {
//             int n_hor;
//             int debug_level;
//             stateVec_t xInit;
//             double new_cost, cost, dcost, lambda, g_norm, expected;
//             double **p;
//             const double *alpha;
//             int n_alpha;
//             double lambdaMax;
//             double lambdaMin;
//             double lambdaInit;
//             double dlambdaInit;
//             double lambdaFactor;
//             int max_iter;
//             double tolGrad;
//             double tolFun;
//             double tolConstraint;
//             double zMin;
//             int regType;
//             int iterations;
//             int *log_linesearch;
//             double *log_z;
//             double *log_cost;
//             double dV[2];
            
//             double w_pen_l;
//             double w_pen_f;
//             double w_pen_max_l;
//             double w_pen_max_f;
//             double w_pen_init_l;
//             double w_pen_init_f;
//             double w_pen_fact1;
//             double w_pen_fact2;
            
//             int print;
//             // traj_t *nominal;
//             // traj_t *candidates[NUMBER_OF_THREADS]; 
            
//             // traj_t trajectories[NUMBER_OF_THREADS+1];
            
//             // multipliers_t multipliers;
// }tOptSet;

using namespace Eigen;
USING_NAMESPACE_QPOASES

class ILQRSolver
{
public:
    struct traj
    {
        stateVecTab_t xList;
        commandVecTab_t uList;
        unsigned int iter;
    };

    struct tOptSet {
        int n_hor;
        int debug_level;
        stateVec_t xInit;
        double new_cost, cost, dcost, lambda, g_norm, expected;
        double **p;
        const double *alpha;
        int n_alpha;
        double lambdaMax;
        double lambdaMin;
        double lambdaInit;
        double dlambdaInit;
        double lambdaFactor;
        int max_iter;
        double tolGrad;
        double tolFun;
        double tolConstraint;
        double zMin;
        int regType;
        int iterations;
        int *log_linesearch;
        double *log_z;
        double *log_cost;
        double dV[2];
        
        double w_pen_l;
        double w_pen_f;
        double w_pen_max_l;
        double w_pen_max_f;
        double w_pen_init_l;
        double w_pen_init_f;
        double w_pen_fact1;
        double w_pen_fact2;
        
        int print;
        // traj_t *nominal;
        // traj_t *candidates[NUMBER_OF_THREADS]; 
        
        // traj_t trajectories[NUMBER_OF_THREADS+1];
        
        // multipliers_t multipliers;
    };

public:
    ILQRSolver(DynamicModel& myDynamicModel, CostFunction& myCostFunction,bool fullDDP=0,bool QPBox=0);
private:
protected:
    // attributes //
public:
private:
    DynamicModel* dynamicModel;
    CostFunction* costFunction;
    unsigned int stateNb;
    unsigned int commandNb;
    stateVec_t x;
    commandVec_t u;
    stateVec_t xInit;
    stateVec_t xgoal;
    unsigned int T;
    unsigned int iter;
    double dt;
    double stopCrit;
    double changeAmount;

    stateVecTab_t xList;
    commandVecTab_t uList;
    stateVecTab_t updatedxList;
    commandVecTab_t updateduList;
    stateVecTab_t tmpxPtr;
    commandVecTab_t tmpuPtr;
    struct traj lastTraj;

    stateVec_t nextVx;
    stateMat_t nextVxx;
    stateVec_t Qx;
    stateMat_t Qxx;
    commandVec_t Qu;
    commandMat_t Quu;
    commandMat_t QuuInv;
    commandR_stateC_t Qux;
    commandVec_t k;
    commandR_stateC_t K;
    commandVecTab_t kList;
    commandR_stateC_tab_t KList;
    double alphaList[11];
    double alpha;

    double mu;
    stateMat_t muEye;
    unsigned char completeBackwardFlag;

    /* QP variables */
    QProblemB* qp;
    bool enableQPBox;
    bool enableFullDDP;
    commandMat_t H;
    commandVec_t g;
    commandVec_t lowerCommandBounds;
    commandVec_t upperCommandBounds;
    commandVec_t lb;
    commandVec_t ub;
    int nWSR;
    real_t* xOpt;

    tOptSet Op;
    //Eigen::VectorXd default_alpha;
    int verbosity;
    stateVecTab_t FList;
    stateMatTab_t fx_new;
    stateR_commandC_tab_t fu_new;
    stateVecTab_t cx_new; 
    commandVecTab_t cu_new; 
    stateMatTab_t cxx_new; 
    stateR_commandC_tab_t cxu_new; 
    commandMatTab_t cuu_new;
    double c;

protected:
    // methods //
public:
    void FirstInitSolver(stateVec_t& myxInit, stateVec_t& myxDes, unsigned int& myT,
                    double& mydt, unsigned int& mymax_iter,double& mystopCrit, double& mytolFun, double& mytolGrad);
    void solveTrajectory();
    void initializeTraj(tOptSet *Op);
    void standard_parameters(tOptSet *o);
    struct traj getLastSolvedTrajectory();
//private:
    void initTrajectory(tOptSet *Op);
    void backwardLoop();
    void forwardLoop(tOptSet *Op);
    bool isQuudefinitePositive(const commandMat_t & Quu); 
protected:

};

#endif // ILQRSOLVER_H