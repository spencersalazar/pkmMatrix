// Parag K. Mital
// Nov. 2008
// This library is for a 2D model.

/*
 CARPE, The Software" © Parag K Mital, parag@pkmital.com
 
 The Software is and remains the property of Parag K Mital
 ("pkmital") The Licensee will ensure that the Copyright Notice set
 out above appears prominently wherever the Software is used.
 
 The Software is distributed under this Licence: 
 
 - on a non-exclusive basis, 
 
 - solely for non-commercial use in the hope that it will be useful, 
 
 - "AS-IS" and in order for the benefit of its educational and research
 purposes, pkmital makes clear that no condition is made or to be
 implied, nor is any representation or warranty given or to be
 implied, as to (i) the quality, accuracy or reliability of the
 Software; (ii) the suitability of the Software for any particular
 use or for use under any specific conditions; and (iii) whether use
 of the Software will infringe third-party rights.
 
 pkmital disclaims: 
 
 - all responsibility for the use which is made of the Software; and
 
 - any liability for the outcomes arising from using the Software.
 
 The Licensee may make public, results or data obtained from, dependent
 on or arising out of the use of the Software provided that any such
 publication includes a prominent statement identifying the Software as
 the source of the results or the data, including the Copyright Notice
 and stating that the Software has been made available for use by the
 Licensee under licence from pkmital and the Licensee provides a copy of
 any such publication to pkmital.
 
 The Licensee agrees to indemnify pkmital and hold them
 harmless from and against any and all claims, damages and liabilities
 asserted by third parties (including claims for negligence) which
 arise directly or indirectly from the use of the Software or any
 derivative of it or the sale of any products based on the
 Software. The Licensee undertakes to make no liability claim against
 any employee, student, agent or appointee of pkmital, in connection 
 with this Licence or the Software.
 
 
 No part of the Software may be reproduced, modified, transmitted or
 transferred in any form or by any means, electronic or mechanical,
 without the express permission of pkmital. pkmital's permission is not
 required if the said reproduction, modification, transmission or
 transference is done without financial return, the conditions of this
 Licence are imposed upon the receiver of the product, and all original
 and amended source code is included in any transmitted product. You
 may be held legally responsible for any copyright infringement that is
 caused or encouraged by your failure to abide by these terms and
 conditions.
 
 You are not permitted under this Licence to use this Software
 commercially. Use for which any financial return is received shall be
 defined as commercial use, and includes (1) integration of all or part
 of the source code or the Software into a product for sale or license
 by or on behalf of Licensee to third parties or (2) use of the
 Software or any derivative of it for research with the final aim of
 developing software products for sale or license to a third party or
 (3) use of the Software or any derivative of it for research with the
 final aim of developing non-software products for sale or license to a
 third party, or (4) use of the Software to provide any service to an
 external organisation for which payment is received. If you are
 interested in using the Software commercially, please contact pkmital to
 negotiate a licence. Contact details are: parag@pkmital.com
 
 
 *
 *
 */

#ifndef __pkmGaussianMixtureModel
#define __pkmGaussianMixtureModel

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

class pkmGaussianMixtureModel
{
enum {COV_SPHERICAL, COV_DIAGONAL, COV_GENERIC};
public:
	// setup the mixture model (variables has to be 2)
	pkmGaussianMixtureModel(double *inputData, int observations, int variables, int map_scalar = 1, int cov_type = COV_SPHERICAL);

	// release CvMats stuff...
	~pkmGaussianMixtureModel();

	// the actual modeling step takes the min and max number of kernels,
	// a regularizing factor for the covariance matrix (necessary for small data)
	// and the stopping threshold for the increase in likelihood
	void modelData(int minComponents, int maxComponents, double regularizingFactor,
			double stoppingThreshold);

	void getLikelihoodMap(int rows, int cols, unsigned char *map, std::ofstream &filePtr, int widthStep = 0);

	double multinormalDistribution(const CvMat *pts, const CvMat *mean, const CvMat *covar);

	// Accessor functions as simple dynamic arrays
	int		getNumberOfClusters	();
	float*	getClusterMean		(int clusterNum);
	float	getClusterWeight	(int clusterNum);
	float** getClusterCov		(int clusterNum);
    int     getBestCluster      () { return bestCluster; }
    
	// write the best model's data to a give file stream
	int		writeToFile(std::ofstream &fileStream, bool writeClusterNums = true, 
						bool writeWeights = true, bool writeMeans = true, 
						bool writeCovs = true, bool verbose = false);


private:
	// number of parameters for the free covariance matrix
	int			m_nPars;
	int			m_nParsOver2;

	// ptr to inputData
	double		*m_pData;

	// CvEM Model array
	CvEM		*emModel;

	// OpenCvMat's for Input Data, Covariance, and Means
	CvMat		*m_pCvData;

	// Labels of each sample to the most probable mixture
	CvMat		*m_pCvLabels;

	// Mixing Probabilities
	//CvMat		*m_pCvMixProb;

	// Dimensions of input data
	int		m_nObservations;
	int		m_nVariables;
	int		m_nScale;
    
    // best cluster index (using weight)
    int     bestCluster;

	double	m_Likelihood;
	double	m_BIC;

	// best number of kernels based on MLE
	int		bestModel;

	// Number of kernels
	int		m_nKernels;

	// type of covariance matrix
	int		m_covType;
	/*
	int m_nKernelCount;
	int m_nAttribute;
	double* m_pArrMeanVarWeight;
	double* m_pCatLikelihoods;
	double* m_pTemp;
	//GData* m_pData;
	GNormalDistribution m_dist;
	double m_dMinVariance;
	*/
    
    bool bModeled;
};

#endif