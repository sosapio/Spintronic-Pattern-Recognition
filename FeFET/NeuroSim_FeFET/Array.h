
#ifndef ARRAY_H_
#define ARRAY_H_

#include <cstdlib>
#include "Cell.h"

class Array {
public:
	Cell ***cell;
	int arrayColSize, arrayRowSize, wireWidth;
	double unitLengthWireResistance;
	double wireResistanceRow, wireResistanceCol;
	double wireCapRow;	// Cap of the WL (cross-point) or BL (1T1R)
	double wireCapCol;	// Cap of the BL (cross-point) or SL (1T1R)
	double wireGateCapRow;	// Cap of 1T1R WL cap
	double wireCapBLCol;	// Cap of 1T1R BL cap in digital eNVM
	double readEnergy, writeEnergy;
	int numCellPerSynapse;	// For SRAM to use redundant cells to represent one synapse
	double writeEnergySRAMCell;	// Write energy per SRAM cell (will move this to SRAM cell level in the future)
	bool **weightChange;	// Specify if the weight value will change or not during weight update (for SRAM and digital eNVM)
	
	/* Constructor */
	Array(int arrayColSize, int arrayRowSize, int wireWidth) {
		this->arrayColSize = arrayColSize;
		this->arrayRowSize = arrayRowSize;
		this->wireWidth = wireWidth;
		readEnergy = 0;
		writeEnergy = 0;
		/* Initialize weightChange */
		weightChange = new bool*[arrayColSize];
		for (int col=0; col<arrayColSize; col++) {
			weightChange[col] = new bool[arrayRowSize];
		}
	}

	template <class memoryType>
	void Initialization(int numCellPerSynapse=1) {
		/* Determine number of cells per synapse (SRAM only now) */
		this->numCellPerSynapse = numCellPerSynapse;

		/* Initialize memory cells */
		cell = new Cell**[arrayColSize*numCellPerSynapse];
		for (int col=0; col<arrayColSize*numCellPerSynapse; col++) {
			cell[col] = new Cell*[arrayRowSize];
			for (int row=0; row<arrayRowSize; row++) {
				cell[col][row] = new memoryType(col, row);
			}
		}
		
		/* Initialize interconnect wires */
		double AR;	// Aspect ratio of wire height to wire width
		double Rho;	// Resistivity
		switch(wireWidth) {
			case 200: 	AR = 2.10; Rho = 2.42e-8; break;
			case 100:	AR = 2.30; Rho = 2.73e-8; break;
			case 50:	AR = 2.34; Rho = 3.91e-8; break;
			case 40:	AR = 1.90; Rho = 4.03e-8; break;
			case 32:	AR = 1.90; Rho = 4.51e-8; break;
			case 22:	AR = 2.00; Rho = 5.41e-8; break;
			case 14:	AR = 2.10; Rho = 7.43e-8; break;
			case -1:	break;	// Ignore wire resistance or user define
			default:	exit(-1); puts("Wire width out of range"); 
		}
		double wireLength = wireWidth * 1e-9 * 2;	// 2F
		if (wireWidth == -1) {
			unitLengthWireResistance = 1.0;	// Use a small number to prevent numerical error for NeuroSim
			wireResistanceRow = 0;
			wireResistanceCol = 0;
		} else {
			unitLengthWireResistance =  Rho / ( wireWidth*1e-9 * wireWidth*1e-9 * AR );
			wireResistanceRow = unitLengthWireResistance * wireLength;
			wireResistanceCol = unitLengthWireResistance * wireLength;
		}
		wireCapRow = wireLength * 0.2e-15/1e-6;
		wireCapCol = wireLength * 0.2e-15/1e-6;
		wireGateCapRow = wireLength * 0.2e-15/1e-6;
		
	}

	double ReadCell(int x, int y);	// x (column) and y (row) start from index 0
	void WriteCell(int x, int y, double deltaWeight, double maxWeight, double minWeight, bool regular);
	double GetMaxCellReadCurrent(int x, int y);
	double ConductanceToWeight(int x, int y, double maxWeight, double minWeight);

	//EDITED
	// Function to write conductance values to a file. 
	void WriteConductanceToFile(const std::string& filename);
};

#endif
