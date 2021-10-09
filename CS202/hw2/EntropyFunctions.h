#pragma once

/**
* Author : RüzgarAyan
* ID: 21801984
* Section : 2
* Assignment : 2
*/

double calculateEntropy(const int* classCounts, const int numClasses);
double calculateInformationGain(const bool** data, const int* labels,
		const int numSamples, const int numFeatures, const bool* usedSamples,
		const int featureId);

