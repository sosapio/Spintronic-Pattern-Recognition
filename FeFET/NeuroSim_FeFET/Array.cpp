#include "formula.h"
#include "Array.h"
#include <iostream>
#include <fstream>

double Array::ReadCell(int x, int y) {
	if (AnalogNVM *temp = dynamic_cast<AnalogNVM*>(**cell)) {	// Analog eNVM
		double readVoltage = static_cast<eNVM*>(cell[x][y])->readVoltage;
		double totalWireResistance;
		if (static_cast<eNVM*>(cell[x][y])->cmosAccess) {
			if (static_cast<AnalogNVM*>(cell[x][y])->FeFET) {	// FeFET
				totalWireResistance = (x + 1) * wireResistanceRow + (arrayRowSize - y) * wireResistanceCol;
			} else {	// Normal
				totalWireResistance = (x + 1) * wireResistanceRow + (arrayRowSize - y) * wireResistanceCol + static_cast<eNVM*>(cell[x][y])->resistanceAccess;
			}
		} else {
			totalWireResistance = (x + 1) * wireResistanceRow + (arrayRowSize - y) * wireResistanceCol;
		}
		double cellCurrent;
		if (static_cast<eNVM*>(cell[x][y])->nonlinearIV) {
			/* Bisection method to calculate read current with nonlinearity */
			int maxIter = 30;
			double v1 = 0, v2 = readVoltage, v3;
			double wireCurrent;
			for (int iter=0; iter<maxIter; iter++) {
				//printf("iter: %d, %f\t%f\n", iter, v1, v2);
				v3 = (v1 + v2)/2;
				wireCurrent = (readVoltage - v3)/totalWireResistance;
				cellCurrent = static_cast<AnalogNVM*>(cell[x][y])->Read(v3);
				if (wireCurrent > cellCurrent)
					v1 = v3;
				else
					v2 = v3;
			}
		} else {	// No nonlinearity
			if (static_cast<eNVM*>(cell[x][y])->readNoise) {
				extern std::mt19937 gen;
				cellCurrent = readVoltage / (1/static_cast<eNVM*>(cell[x][y])->conductance * (1 + (*static_cast<eNVM*>(cell[x][y])->gaussian_dist)(gen)) + totalWireResistance);
			} else {
				cellCurrent = readVoltage / (1/static_cast<eNVM*>(cell[x][y])->conductance + totalWireResistance);
			}
		}
		return cellCurrent;

	} else {	// digital eNVM
		int weightDigits = 0;
		if (DigitalNVM *temp = dynamic_cast<DigitalNVM*>(**cell)) {	// Digital eNVM
			for (int n=0; n<numCellPerSynapse; n++) {   // n=0 is LSB
				int colIndex = (x+1) * numCellPerSynapse - (n+1);
				double readVoltage = static_cast<eNVM*>(cell[colIndex][y])->readVoltage;
				double totalWireResistance;
				if (static_cast<eNVM*>(cell[colIndex][y])->cmosAccess) {
					totalWireResistance = (colIndex + 1) * wireResistanceRow + (arrayRowSize - y) * wireResistanceCol + static_cast<eNVM*>(cell[colIndex][y])->resistanceAccess;
				} else {
					totalWireResistance = (colIndex + 1) * wireResistanceRow + (arrayRowSize - y) * wireResistanceCol;
				}
				double cellCurrent;
				if (static_cast<eNVM*>(cell[colIndex][y])->nonlinearIV) {
					/* Bisection method to calculate read current with nonlinearity */
					int maxIter = 30;
					double v1 = 0, v2 = readVoltage, v3;
					double wireCurrent;
					for (int iter=0; iter<maxIter; iter++) {
						//printf("iter: %d, %f\t%f\n", iter, v1, v2);
						v3 = (v1 + v2)/2;
						wireCurrent = (readVoltage - v3)/totalWireResistance;
						cellCurrent = static_cast<DigitalNVM*>(cell[colIndex][y])->Read(v3);
						if (wireCurrent > cellCurrent)
							v1 = v3;
						else
							v2 = v3;
					}
				} else {    // No nonlinearity
					if (static_cast<eNVM*>(cell[colIndex][y])->readNoise) {
						extern std::mt19937 gen;
						cellCurrent = readVoltage / (1/static_cast<eNVM*>(cell[colIndex][y])->conductance * (1 + (*static_cast<eNVM*>(cell[colIndex][y])->gaussian_dist)(gen)) + totalWireResistance);
					} else {
						cellCurrent = readVoltage / (1/static_cast<eNVM*>(cell[colIndex][y])->conductance + totalWireResistance);
					}
				}
				// Current sensing
				int bit;
				if (cellCurrent >= static_cast<DigitalNVM*>(cell[colIndex][y])->refCurrent) {
					bit = 1;
				} else {
					bit = 0;
				}
				weightDigits += bit * pow(2, n);	// If the rightmost is LSB
			}
		} 
        else { 
            std::cerr << "Error: The memory cell is not valid." << std::endl;
        }
		return weightDigits;
	}
}

void Array::WriteCell(int x, int y, double deltaWeight, double maxWeight, double minWeight, 
						bool regular /* False: ideal write, True: regular write considering device properties */) {
	// TODO: include wire resistance
	double deltaWeightNormalized = deltaWeight / (maxWeight - minWeight);
	if (AnalogNVM *temp = dynamic_cast<AnalogNVM*>(**cell)) { // Analog eNVM
		if (regular) {	// Regular write
			static_cast<AnalogNVM*>(cell[x][y])->Write(deltaWeightNormalized);
		} else {	// Preparation stage (ideal write)
			double conductance = static_cast<eNVM*>(cell[x][y])->conductance;
			double maxConductance = static_cast<eNVM*>(cell[x][y])->maxConductance;
			double minConductance = static_cast<eNVM*>(cell[x][y])->minConductance;
			conductance += deltaWeightNormalized * (maxConductance - minConductance);
			if (conductance > maxConductance) {
				conductance = maxConductance;
			} else if (conductance < minConductance) {
				conductance = minConductance;
			}
			static_cast<eNVM*>(cell[x][y])->conductance = conductance;
		}
	} else {    //digital eNVM
		int numLevel = pow(2, numCellPerSynapse);
		deltaWeightNormalized = truncate(deltaWeightNormalized, numLevel - 1);
		weightChange[x][y] = (deltaWeightNormalized != 0)? true : false;
		int maxWeightDigits = pow(2, numCellPerSynapse) - 1;
		/* Get original weight */
		int weightDigits = (int)(this->ReadCell(x, y));

		/* Calculate target weight */
		int targetWeightDigits = weightDigits + deltaWeightNormalized * maxWeightDigits;
		if (targetWeightDigits > maxWeightDigits) {
			targetWeightDigits = maxWeightDigits;
		} else if (targetWeightDigits < 0) {
			targetWeightDigits = 0;
		}

		/* Write new weight and calculate write energy */
		if (DigitalNVM *temp = dynamic_cast<DigitalNVM*>(**cell)) { // Digital eNVM
			for (int n=0; n<numCellPerSynapse; n++) {	// n=0 is LSB
				int bitNew = ((targetWeightDigits >> n) & 1);
				/* Write new weight */
				if (static_cast<eNVM*>(cell[x][y])->cmosAccess) {  // 1T1R
					static_cast<DigitalNVM*>(cell[(x+1) * numCellPerSynapse - (n+1)][y])->Write(bitNew, wireCapBLCol);
				} else {	// Cross-point
					static_cast<DigitalNVM*>(cell[(x+1) * numCellPerSynapse - (n+1)][y])->Write(bitNew, wireCapCol);
				}
			}
		} 
	}
}

double Array::GetMaxCellReadCurrent(int x, int y) {
	return static_cast<AnalogNVM*>(cell[x][y])->GetMaxReadCurrent();
}

double Array::ConductanceToWeight(int x, int y, double maxWeight, double minWeight) {
	if (AnalogNVM *temp = dynamic_cast<AnalogNVM*>(**cell)) {	// Analog eNVM
		/* Measure current */
		double I = this->ReadCell(x, y);
		/* Convert current to weight */
		double Imax = static_cast<AnalogNVM*>(cell[x][y])->GetMaxReadCurrent();
		double Imin = static_cast<AnalogNVM*>(cell[x][y])->GetMinReadCurrent();
		if (I<Imin)
			I = Imin;
		else if (I>Imax)
			I = Imax;
		return (I-Imin) / (Imax-Imin) * (maxWeight-minWeight) + minWeight;
	} else {	// SRAM or digital eNVM
		double weightDigits = this->ReadCell(x, y);
		int weightDigitsMax = pow(2, numCellPerSynapse) - 1;
		return (weightDigits / weightDigitsMax) * (maxWeight - minWeight) + minWeight;
	}
}


//EDITED
// Function to write conductance values to a file. 
//vulnerability: path-injection- Unvalidated input in path value creation risks unintended file/directory access
void Array::WriteConductanceToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    for (int x = 0; x < arrayRowSize; ++x) {
        for (int y = 0; y < arrayColSize; ++y) {
            if (AnalogNVM *temp = dynamic_cast<AnalogNVM*>(**cell)) {
                file << static_cast<eNVM*>(cell[x][y])->conductance << " ";
            } else {
                file << 0 << " "; //default value for non-AnalogNVM cells
            }
        }
        file << std::endl;
    }

    file.close();
}
