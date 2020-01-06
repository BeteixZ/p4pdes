static char help[] =
"ODE system solver example using TS.  Solves N-dimensional system\n"
"    dy/dt = G(t,y)\n"
"with y(t0) = y0 to compute y(tf).  Serial only.\n"
"Sets TS type to Runge-Kutta.  The implemented example has\n"
"G_0 = y_1, G_1 = - y_0 + t, y_0(0) = 0, y_1(0) = 0.  The exact solution is\n"
"y_0(t) = t - sin(t), y_1(t) = 1 - cos(t).\n\n";

#include <petsc.h>

extern PetscErrorCode ExactSolution(PetscReal, Vec);
extern PetscErrorCode FormRHSFunction(TS, PetscReal, Vec, Vec, void*);

//STARTMAIN
int main(int argc,char **argv) {
  PetscErrorCode ierr;
  PetscInt   steps;
  PetscReal  t0 = 0.0, tf = 20.0, dt = 0.1, err;
  Vec        y, yexact;
  TS         ts;

  PetscInitialize(&argc,&argv,(char*)0,help);

  ierr = VecCreate(PETSC_COMM_WORLD,&y); CHKERRQ(ierr);
  ierr = VecSetSizes(y,PETSC_DECIDE,2); CHKERRQ(ierr);
  ierr = VecSetFromOptions(y); CHKERRQ(ierr);
  ierr = VecDuplicate(y,&yexact); CHKERRQ(ierr);

  ierr = TSCreate(PETSC_COMM_WORLD,&ts); CHKERRQ(ierr);
  ierr = TSSetProblemType(ts,TS_NONLINEAR); CHKERRQ(ierr);
  ierr = TSSetRHSFunction(ts,NULL,FormRHSFunction,NULL); CHKERRQ(ierr);
  ierr = TSSetType(ts,TSRK); CHKERRQ(ierr);

  // set time axis
  ierr = TSSetTime(ts,t0); CHKERRQ(ierr);
  ierr = TSSetMaxTime(ts,tf); CHKERRQ(ierr);
  ierr = TSSetTimeStep(ts,dt); CHKERRQ(ierr);
  ierr = TSSetExactFinalTime(ts,TS_EXACTFINALTIME_MATCHSTEP); CHKERRQ(ierr);
  ierr = TSSetFromOptions(ts); CHKERRQ(ierr);

  // set initial values and solve
  ierr = TSGetTime(ts,&t0); CHKERRQ(ierr);
  ierr = ExactSolution(t0,y); CHKERRQ(ierr);
  ierr = TSSolve(ts,y); CHKERRQ(ierr);

  // compute error and report
  ierr = TSGetStepNumber(ts,&steps); CHKERRQ(ierr);
  ierr = TSGetTime(ts,&tf); CHKERRQ(ierr);
  ierr = ExactSolution(tf,yexact); CHKERRQ(ierr);
  ierr = VecAXPY(y,-1.0,yexact); CHKERRQ(ierr);    // y <- y - yexact
  ierr = VecNorm(y,NORM_INFINITY,&err); CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,
              "error at tf = %.3f with %d steps:  |y-y_exact|_inf = %g\n",
              tf,steps,err); CHKERRQ(ierr);

  VecDestroy(&y);  VecDestroy(&yexact);  TSDestroy(&ts);
  return PetscFinalize();
}
//ENDMAIN

//STARTCALLBACKS
PetscErrorCode ExactSolution(PetscReal t, Vec y) {
    PetscReal *ay;
    VecGetArray(y,&ay);
    ay[0] = t - PetscSinReal(t);
    ay[1] = 1.0 - PetscCosReal(t);
    VecRestoreArray(y,&ay);
    return 0;
}

PetscErrorCode FormRHSFunction(TS ts, PetscReal t, Vec y, Vec g,
                               void *ptr) {
    const PetscReal *ay;
    PetscReal       *ag;
    VecGetArrayRead(y,&ay);
    VecGetArray(g,&ag);
    ag[0] = ay[1];            // = g_1(t,y)
    ag[1] = - ay[0] + t;      // = g_2(t,y)
    VecRestoreArrayRead(y,&ay);
    VecRestoreArray(g,&ag);
    return 0;
}
//ENDCALLBACKS

