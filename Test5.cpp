#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <ctime>

#include "DiffusionGBM.h"

#include "IRProvider.h"
#include "IRProviderConst.h"

#include "MCEngine1D.h"
#include "MCEngine1D.hpp"

#include "VanillaOptions.h"
#include "MCOptionHedger1D.hpp"



using namespace siriusFM;
using namespace std;

namespace {
  inline double Phi(double x){return 0.5*(1.0+erf(x/M_SQRT2));}
  inline double BSMPxCall(double a_S0, double a_K, double a_TTE, double a_rateA, double a_rateB, double a_sigma){assert(a_S0>0&&a_K>0&&a_sigma>0);
			if (a_TTE<=0) {return std::max<double>(a_S0-a_K,0);}
			double xd=a_sigma*sqrt(a_TTE);
			double x1=(log(a_S0/a_K)+(a_rateB-a_rateA+a_sigma*a_sigma/2.0)*a_TTE)/xd;
			double x2=x1-xd;
			double px=a_S0*exp(-a_rateA*a_TTE)*Phi(x1)-a_K*exp(-a_rateB*a_TTE)*Phi(x2);
			return px;
			}
  inline double BSMPxPut(double a_S0, double a_K, double a_TTE, double a_rateA, double a_rateB, double a_sigma){
			double px=BSMPxCall(a_S0,a_K,a_TTE,a_rateA,a_rateB,a_sigma)-a_S0+exp(-a_rateB*a_TTE)*a_K;
			assert(px>0.0);
			return px;
			}
  inline double BSMDeltaCall(double a_S0,double a_K,double a_TTE,double a_rateA, double a_rateB, double a_sigma){
			assert(a_S0>0 && a_K>0 && a_sigma>0);
			if (a_TTE<=0) {return (a_S0<a_K)?0:(a_S0>a_K)?1:0.5;}
			double xd=a_sigma*sqrt(a_TTE);
			double x1=(log(a_S0/a_K)+(a_rateB-a_rateA+a_sigma*a_sigma/2.0)*a_TTE)/xd;
			return Phi(x1);
			}
  inline double BSMDeltaPut(double a_S0,double a_K,double a_TTE,double a_rateA, double a_rateB, double a_sigma){
			return BSMDeltaCall(a_S0,a_K,a_TTE,a_rateA,a_rateB,a_sigma)-1.0;
			}



}

int main(int argc, char** argv)
{
  printf("Hello!\n");
  if (argc!=10){printf("Usage: mu, sigma, S_0, call/put, K, T_days, deltaAcc, tau_min,P\n"); return -1;}
  double mu=atof(argv[1]);
  double sigma=atof(argv[2]);
  double s0=atof(argv[3]);
  char const* optType=argv[4];
  double K=atof(argv[5]);
  long T_days=atof(argv[6]);
  double deltaAcc=atof(argv[7]);
  int tau_min=atoi(argv[8]);
  long P=atoi(argv[9]);
  double t_full;
  if (sigma<=0 || s0<=0 || T_days<=0 || tau_min<=0 || P<=0){printf("Bad values in input!\n"); return -2;}
  
  CcyE ccyA=CcyE::USD;
  CcyE ccyB=CcyE::USD;

  char const* ratesFileA=nullptr;
  char const* ratesFileB=nullptr;
  bool useTimerSeed=true; 

  
  DiffusionGBM diff(mu, sigma,s0);

  MCOptionHedger1D<decltype(diff),IRProvider<IRModeE::Const>,IRProvider<IRModeE::Const>,CcyE,CcyE> hedger(ratesFileA,ratesFileB,&diff, useTimerSeed);

  time_t t0=time(nullptr);
  time_t T=t0+T_days*86400;
  double TTE=YearFrac(T-t0)-YearFrac(0.0);
  double Ty=1970.0 +double(T)/31557600;
  
  OptionFX * option=nullptr;
  decltype(hedger)::DeltaFunc const* deltaFunc=nullptr;
  double C0=0.0;

  double rateA=hedger.getRateA(ccyA,0.0);
  double rateB=hedger.getRateB(ccyB,0.0);

  function<double(double, double)> deltaCall([K,Ty,rateA,rateB,sigma](double a_St,double a_t)->double{double currTTE=Ty-a_t; return BSMDeltaCall(a_St,K,currTTE,rateA,rateB,sigma);});

  function<double(double, double)> deltaPut([K,Ty,rateA,rateB,sigma](double a_St,double a_t)->double{double currTTE=Ty-a_t; return BSMDeltaPut(a_St,K,currTTE,rateA,rateB,sigma);});

  if (strcmp(optType,"call")==0) {option=new EurCallOptionFX(K,T, ccyA, ccyB);C0=BSMPxCall(s0,K,TTE,rateA,rateB,sigma); printf("C0=%f\n",C0);deltaFunc=&deltaCall;}
  if (strcmp(optType,"put")==0) {option=new EurPutOptionFX(K,T, ccyA, ccyB);C0=BSMPxPut(s0,K,TTE,rateA,rateB,sigma);deltaFunc=&deltaPut;}


  t_full=clock();

  // auto res=hedger.SimulateHedging(option,t0,C0,deltaFunc,deltaAcc,tau_min,P);
  auto res = hedger.SimulateHedging(option, t0, tau_min, P, C0, deltaFunc, deltaAcc);
  t_full=(clock()-t_full)/CLOCKS_PER_SEC;

  double EPnL=get<0>(res);
  double StdPnL=get<1>(res);
  double MinPnL=get<2>(res);
  double MaxPnL=get<3>(res);
  
  printf("EPnL=%f\n",EPnL);
  printf("StdPnL=%f\n",StdPnL);
  printf("MinPnL=%f\n",MinPnL);
  printf("MaxPnL=%f\n",MaxPnL);
  printf("t_full=%f\n",t_full);
  


  delete option;
   
  
  printf("Goodbuy!\n");
  return 0;

}