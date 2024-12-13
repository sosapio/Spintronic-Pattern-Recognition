#ifndef FORMULA_H_
#define FORMULA_H_

#include <vector>

double sigmoid(double x);
double truncate(double x, int numBit, double threshold=0.5);
double round_th(double x, double threshold);
double NonlinearWeight(double xPulse, int maxNumLevel, double A, double B, double minConductance);
double InvNonlinearWeight(double conductance, int maxNumLevel, double A, double B, double minConductance);
double MeasuredLTP(double xPulse, int maxNumLevel, std::vector<double>& dataConductanceLTP);
double MeasuredLTD(double xPulse, int maxNumLevel, std::vector<double>& dataConductanceLTD);
double InvMeasuredLTP(double conductance, int maxNumLevel, std::vector<double>& dataConductanceLTP);
double InvMeasuredLTD(double conductance, int maxNumLevel, std::vector<double>& dataConductanceLTD);
double getParamA(double NL);
double NonlinearConductance(double C, double NL, double Vw, double Vr, double V);

#endif
