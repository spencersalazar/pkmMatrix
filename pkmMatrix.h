/*
 *  pkmMatrix.h
 *  
 
 row-major floating point matrix utility class
 utilizes Apple Accelerate's vDSP functions for SSE optimizations
 
 Copyright (C) 2011 Parag K. Mital
 
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
 
 int matrix_invert(int N, double *matrix) {
 
 int error=0;
 int *pivot = malloc(N*N*sizeof(int));
 double *workspace = malloc(N*sizeof(double));
 
 dgetrf_(&N, &N, matrix, &N, pivot, &error);
 
 if (error != 0) {
 NSLog(@"Error 1");
 return error;
 }
 
 dgetri_(&N, matrix, &N, pivot, workspace, &N, &error);
 
 if (error != 0) {
 NSLog(@"Error 2");
 return error;
 }
 
 free(pivot);
 free(workspace);
 return error;
 }
 
 
 *
 */

#pragma once

#include <iostream>
#include <assert.h>
#include <Accelerate/Accelerate.h>
#include <vector>

#ifdef OPENCV_ERROR
#define HAVE_OPENCV
#endif

//#define DEBUG
#define HAVE_OPENCV

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#endif

using namespace std;


#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

// uncomment next line for vecLib optimizations
//#define MULTIPLE_OF_4(x) ((x | 0x03) + 1)
#define MULTIPLE_OF_4(x) x

template <typename T> int signum(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace pkm
{
	// row-major floating point matrix
	class Mat
	{
		/////////////////////////////////////////
	public:
		// default constructor
		Mat();
		
		// destructor
		~Mat();
		
        Mat(vector<float> m);
        
        Mat(vector<vector<float> > m);
#ifdef HAVE_OPENCV
        Mat(cv::Mat m);
#endif
		// allocate data
		Mat(int r, int c, bool clear = false);
		
		// pass in existing data
		// non-destructive by default
		Mat(int r, int c, float *existing_buffer, bool withCopy);
        
        Mat(int r, int c, const float *existing_buffer);
		
		// set every element to a value
		Mat(int r, int c, float val);
		
		// copy-constructor, called during:
		//		pkm::Mat a(rhs);
		Mat(const Mat &rhs);
		Mat & operator=(const Mat &rhs);
		Mat & operator=(const vector<float> &rhs);
		Mat & operator=(const vector<vector<float> > &rhs);
#ifdef HAVE_OPENCV
        Mat & operator=(const cv::Mat &rhs);
        cv::Mat cvMat() const;
#endif
		
		inline Mat operator+(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows &&
				   cols == rhs.cols);
#endif			
			Mat newMat(rows, cols);
			vDSP_vadd(data, 1, rhs.data, 1, newMat.data, 1, rows*cols);
			return newMat;
		}
		
		
		inline Mat operator+(float rhs) const
		{	
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat newMat(rows, cols);
			vDSP_vsadd(data, 1, &rhs, newMat.data, 1, rows*cols);
			return newMat;
		}
		
		inline Mat operator-(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows &&
				   cols == rhs.cols);
#endif			
			Mat newMat(rows, cols);
			vDSP_vsub(data, 1, rhs.data, 1, newMat.data, 1, rows*cols);
			return newMat;
		}
		

		
		inline Mat operator-(const float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat newMat(rows, cols);
			float rhs = -scalar;
			vDSP_vsadd(data, 1, &rhs, newMat.data, 1, rows*cols);
			return newMat;
		}
		
		
		inline Mat operator*(const pkm::Mat &rhs) const
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(cols == rhs.rows);
#endif
			
			Mat gemmResult(rows, rhs.cols);
			//ldb must be >= MAX(N,1): ldb=30 N=3533Parameter 11 to routine cblas_sgemm was incorrect
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, gemmResult.rows, gemmResult.cols, cols, 1.0f, data, cols, rhs.data, rhs.cols, 0.0f, gemmResult.data, gemmResult.cols);
			//vDSP_mmul(data, 1, rhs.data, 1, gemmResult.data, 1, gemmResult.rows, gemmResult.cols, cols);
			return gemmResult;
		}
		

		
		inline Mat operator*(float scalar) const
		{
#ifdef DEBUG
			assert(data != NULL);
#endif
			
			Mat gemmResult(rows, cols);
			vDSP_vsmul(data, 1, &scalar, gemmResult.data, 1, rows*cols);
			
			return gemmResult;
		}
		
		
		inline Mat operator/(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			vDSP_vdiv(rhs.data, 1, data, 1, result.data, 1, rows*cols);
			return result;
		}
		
		inline Mat operator/(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			vDSP_vsdiv(data, 1, &scalar, result.data, 1, rows*cols);
			return result;
			
		}
		
		inline Mat operator>(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] > rhs.data[i];
			return result;
		}
		
		inline Mat operator>(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] > scalar;
			return result;
		}
		
		inline Mat operator>=(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] >= rhs.data[i];
			return result;
		}
		
		inline Mat operator>=(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] >= scalar;
			return result;
		}
		
		inline Mat operator<(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] < rhs.data[i];
			return result;
		}
		
		inline Mat operator<(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] < scalar;
			return result;
		}
		
		inline Mat operator<=(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] <= rhs.data[i];
			return result;
		}
		
		inline Mat operator<=(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] <= scalar;
			return result;
		}
		
		inline Mat operator==(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] == rhs.data[i];
			return result;
		}
		
		inline Mat operator==(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] == scalar;
			return result;
		}
		
		inline Mat operator!=(const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] != rhs.data[i];
			return result;
		}
		
		inline Mat operator!=(float scalar) const
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			Mat result(rows, cols);
			for(int i = 0; i < rows*cols; i++)
				result.data[i] = data[i] != scalar;
			return result;
		}
		
		inline float & operator[](int idx) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rows*cols >= idx);
#endif	
			return data[idx];
		}
		
		// return a vector composed on non-zero indices of logicalMat
		inline Mat operator[](const Mat &rhs) const
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif	
			std::vector<float> newMat;
			for(int i = 0; i < rows*cols; i++)
			{
				if (rhs.data[i] > 0) {
					newMat.push_back(data[i]);
				}
			}
			if (newMat.size() > 0) {
				Mat result(1,newMat.size());
				for(int i = 0; i < newMat.size(); i++)
				{
					result.data[i] = newMat[i];
				}
				return result;
			}
			else {
				Mat empty;
				return empty;
			}
		}
		

		
		friend Mat operator-(float lhs, const Mat &rhs)
		{
#ifdef DEBUG			
			assert(rhs.data != NULL);
#endif			
			Mat newMat(rhs.rows, rhs.cols);
			float scalar = -lhs;
			vDSP_vsadd(rhs.data, 1, &scalar, newMat.data, 1, rhs.rows*rhs.cols);
			return newMat;
		}
		
		friend Mat operator*(float lhs, const Mat &rhs)
		{
#ifdef DEBUG
			assert(rhs.data != NULL);
#endif
			
			Mat gemmResult(rhs.rows, rhs.cols);
			vDSP_vsmul(rhs.data, 1, &lhs, gemmResult.data, 1, rhs.rows*rhs.cols);
			
			return gemmResult;
		}
		friend Mat operator+(float lhs, const Mat &rhs)
		{
#ifdef DEBUG			
			assert(rhs.data != NULL);
#endif			
			Mat newMat(rhs.rows, rhs.cols);
			vDSP_vsadd(rhs.data, 1, &lhs, newMat.data, 1, rhs.rows*rhs.cols);
			return newMat;
		}
        
        bool isNaN()
        {
            for(int i = 0; i < rows*cols; i++)
            {
                if (isnan(data[i])) {
                    return true;
                }
            }
            return false;
        }
        
        void setNaNsTo(float f)
        {
            for(int i = 0; i < rows*cols; i++)
            {
                if (isnan(data[i]) || isinf(data[i])) {
                    data[i] = f;
                }
            }
        }
        
        void reshape(int r, int c)
        {
            if ((r * c) == (rows * cols))
            {
                rows = r;
                cols = c;
            }
        }
		
        void resize(int r, int c, bool clear = false)
        {
#ifdef DEBUG
            if (bUserData) {
                cout << "[WARNING]: Pointer to user data will be lost/leaked.  Up to user to free this memory!" << endl;
            }
#endif
            if (bAllocated)
            {
            
                if (r >= rows && c >= cols) {
                    
                    if (bUserData) {
                        data = (float *)malloc(MULTIPLE_OF_4(r * c) * sizeof(float));
                    }
                    else
                    {
                        float *temp_data = (float *)malloc(sizeof(float)*MULTIPLE_OF_4(rows*cols));
                        cblas_scopy(rows*cols, data, 1, temp_data, 1);
                        
                        data = (float *)realloc(data, MULTIPLE_OF_4(r * c) * sizeof(float));
                        cblas_scopy(rows*cols, temp_data, 1, data, 1);
                        
                        free(temp_data);
                        temp_data = NULL;
                    }
                    
                    if(clear)
                    {
                        vDSP_vclr(data + rows*cols, 1, r*c - rows*cols);
                    }
                    
                    rows = r;
                    cols = c;
                    
                    bAllocated = true;
                    bUserData = false;
                }
                else if(r * c == rows * cols)
                {
                    rows = r;
                    cols = c;
                }
                else {
                    printf("[ERROR: pkmMatrix::resize()] Cannot resize to a smaller matrix (yet).\n");
                }

            
            }
            else
            {
                data = (float *)malloc(MULTIPLE_OF_4(r * c) * sizeof(float));
                rows = r;
                cols = c;
                
                if(clear)
                {
                    vDSP_vclr(data + rows*cols, 1, r*c - rows*cols);
                }
                
                bAllocated = true;
                bUserData = false;
            }
            return;
        }
		
		// can be used to create an already declared matrix without a copy constructor
		void reset(int r, int c, bool clear = false)
		{
//            if (!(r == rows && c == cols && !bUserData)) {
            
                rows = r;
                cols = c;
                current_row = 0;
                bCircularInsertionFull = false;
                
                releaseMemory();
                
                data = (float *)malloc(MULTIPLE_OF_4(rows * cols) * sizeof(float));
            
                bAllocated = true;
                bUserData = false;
//            }
			
			// set every element to 0
			if(clear)
			{
				vDSP_vclr(data, 1, MULTIPLE_OF_4(rows*cols));
			}
		}
        
        // can be used to create an already declared matrix without a copy constructor
		void reset(int r, int c, float val)
		{
//            if (!bAllocated || r != rows || c != cols || bUserData) {
            
                rows = r;
                cols = c;
                current_row = 0;
                bCircularInsertionFull = false;
                
                releaseMemory();
                
                data = (float *)malloc(MULTIPLE_OF_4(rows * cols) * sizeof(float));
                
                bAllocated = true;
                bUserData = false;
//            }
            
			// set every element to val
            vDSP_vfill(&val, data, 1, rows*cols);
			
		}
		
		// set every element to a value
		inline void setTo(float val)
		{
#ifdef DEBUG
			assert(data != NULL);
#endif	
			vDSP_vfill(&val, data, 1, rows * cols);
		}	
		
		inline void clear()
		{
			if (rows == 0 || cols == 0) {
				return;
			}

			vDSP_vclr(data, 1, rows * cols);
		}
		
		/////////////////////////////////////////
		
		inline float * row(int r)
		{
#ifdef DEBUG
			assert(data != NULL);
#endif			
			return (data + r*cols);
		}
		
		inline void insertRow(const float *buf, int row_idx)
		{
			float * rowData = row(row_idx);
			cblas_scopy(cols, buf, 1, rowData, 1);
		}
		
        void push_back(const Mat &m)
        {
#ifdef DEBUG
            if(bUserData)
            {
                cout << "[WARNING]: Pointer to user data will be resized.  Possible leak!" << endl;
            }
#endif
            if (bAllocated && (rows > 0) && (cols > 0)) {
                if (m.cols != cols) {
                    printf("[ERROR]: pkm::Mat push_back(Mat m) requires same number of columns!\n");
                    return;
                }
                if (m.bAllocated && (m.cols > 0) && (m.rows > 0)) {
                    data = (float *)realloc(data, (rows+m.rows)*cols*sizeof(float));
                    cblas_scopy(m.rows*cols, m.data, 1, data + (rows*cols), 1);
                    rows+=m.rows;
                }
                else {
                    printf("[ERROR]: pkm::Mat push_back(Mat m), matrix m is empty!\n");
                    return;
                }
            }
            else {
                *this = m;
            }

        }
        
        void push_back(float m)
        {
            push_back(&m, 1);
        }
        
        void push_back(const float *m, int size)
        {
#ifdef DEBUG
            if(bUserData)
            {
                cout << "[WARNING]: Pointer to user data will be resized.  Possible leak!" << endl;
            }
#endif
            if(size > 0)
            {
                if (bAllocated && (rows > 0) && (cols > 0)) {
                    if (size != cols) {
                        printf("[ERROR]: pkm::Mat push_back(float *m) requires same number of columns in Mat as length of vector!\n");
                        return;
                    }
                    data = (float *)realloc(data, MULTIPLE_OF_4((rows+1)*cols)*sizeof(float));
                    cblas_scopy(cols, m, 1, data + (rows*cols), 1);
                    rows++;
                }
                else {
                    cols = size;
                    data = (float *)malloc(sizeof(float) * MULTIPLE_OF_4(cols));
                    cblas_scopy(cols, m, 1, data, 1);
                    rows = 1;
                    bAllocated = true;
                }
            }
        }
        
        inline void push_back(vector<float> &m)
        {
#ifdef DEBUG
            if(bUserData)
            {
                cout << "[WARNING]: Pointer to user data will be resized.  Possible leak!" << endl;
            }
#endif
            if (bAllocated && rows > 0 && cols > 0) {
                if (m.size() != cols) {
                    printf("[ERROR]: pkm::Mat push_back(vector<float> m) requires same number of columns in Mat as length of vector!\n");
                    return;
                }
                data = (float *)realloc(data, MULTIPLE_OF_4((rows+1)*cols)*sizeof(float));
                cblas_scopy(cols, &(m[0]), 1, data + (rows*cols), 1);
                rows++;
            }
            else {
                *this = m;
            }
            
        }
        
        inline void push_back(vector<vector<float> > &m)
        {
#ifdef DEBUG
            if(bUserData)
            {
                cout << "[WARNING]: Pointer to user data will be resized.  Possible leak!" << endl;
            }
#endif
            if (rows > 0 && cols > 0) {
                if (m[0].size() != cols) {
                    printf("[ERROR]: pkm::Mat push_back(vector<vector<float> > m) requires same number of cols in Mat as length of each vector!\n");
                    return;
                }
                data = (float *)realloc(data, MULTIPLE_OF_4((rows+m.size())*cols)*sizeof(float));
                for (int i = 0; i < m.size(); i++) {
                    cblas_scopy(cols, &(m[i][0]), 1, data + ((rows+i)*cols), 1);
                }
                rows+=m.size();
            }
            else {
                *this = m;
            }
            
        }
        
		inline void insertRowCircularly(const float *buf)
		{
			insertRow(buf, current_row);
			current_row = (current_row + 1) % rows;
			if (current_row == 0) {
				bCircularInsertionFull = true;
			}
		}
        
        inline void insertRowCircularly(vector<float> m)
        {
            insertRowCircularly(&(m[0]));
        }
        
        
        inline void insertRowCircularly(const pkm::Mat &m)
        {
            insertRowCircularly(m.data);
        }
        
        float * getLastCircularRow()
        {
            unsigned int lastRow;
            if (bCircularInsertionFull) {
                lastRow = (current_row - 1) >= 0 ? current_row - 1 : rows - 1;
            }
            else {
                lastRow = (current_row - 1) >= 0 ? current_row - 1 : 0;
            }
            return row(lastRow);
        }
        
        inline void resetCircularRowCounter()
        {
            current_row = 0;
            bCircularInsertionFull = false;
        }
        
        bool isCircularInsertionFull()
        {
            return bCircularInsertionFull;
        }
        
        void removeRow(int i)
        {
#ifdef DEBUG
            assert(i < rows);
            assert(i >= 0);
#endif
            // are we removing the last row (or only row)?
            if(i == (rows - 1))
            {
                rows--;
                realloc(data, sizeof(float)*MULTIPLE_OF_4(rows*cols));       
            }
            // we have to preserve the memory after the deleted row
            else {
                int numRowsToCopy = rows - i - 1;
                float *temp_data = (float *)malloc(sizeof(float)*numRowsToCopy * cols);
                cblas_scopy(numRowsToCopy * cols, row(i+1), 1, temp_data, 1);
                rows--;
                realloc(data, sizeof(float)*MULTIPLE_OF_4(rows*cols));
                cblas_scopy(cols * numRowsToCopy, temp_data, 1, row(i), 1);
                free(temp_data);
                temp_data = NULL;
            }  
        }
		
		// inclusive of start, exclusive of end
		// can be a copy of the original matrix, or a way of editing the original
		// one by not copying the values (default)
		inline Mat rowRange(int start, int end, bool withCopy = true)
		{
#ifdef DEBUG
			assert(rows >= end);
            assert(end > start);
#endif
			Mat submat(end-start, cols, row(start), withCopy);
			return submat;
		}
		
		
		inline Mat colRange(int start, int end, bool withCopy = true)
		{
#ifdef DEBUG
			assert(cols >= end);
#endif
			setTranspose();
			Mat submat(end-start, cols, row(start), withCopy);
			setTranspose();
			submat.setTranspose();
			return submat;
		}
		
		// copy data into the matrix
		void copy(const Mat rhs)
		{
#ifdef DEBUG
			assert(rhs.rows == rows);
			assert(rhs.cols == cols);
#endif
			cblas_scopy(rows*cols, rhs.data, 1, data, 1);
			
		}
		
		void copy(Mat &rhs, Mat &indx)
		{
#ifdef DEBUG
			assert(indx.rows == rows);
			assert(indx.cols == cols);
#endif
			int idx = 0;
			for(int i = 0; i < rows; i++)
			{
				for(int j = 0; j < cols; j++)
				{
					if (indx.data[i*cols + j]) {
						data[i*cols + j] = rhs[idx];
						idx++;
					}
				}
				
			}
		}
		
		/////////////////////////////////////////
		
		// element-wise multiplication
		inline void multiply(const Mat &rhs, Mat &result) const 
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(result.data != NULL);
			assert(rows == rhs.rows && 
				   rhs.rows == result.rows &&
				   cols == rhs.cols && 
				   rhs.cols == result.cols);
#endif
			vDSP_vmul(data, 1, rhs.data, 1, result.data, 1, rows*cols);
			
		}		
		// element-wise multiplication
		// result stored in newly created matrix
		inline Mat multiply(const Mat &rhs)
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows && 
				   cols == rhs.cols);
#endif			
			Mat multiplied_matrix(rows, cols);
			
			vDSP_vmul(data, 1, rhs.data, 1, multiplied_matrix.data, 1, rows*cols);
			return multiplied_matrix;
		}		
		
		inline void multiply(float scalar, Mat &result) const 
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(result.data != NULL);
			assert(rows == result.rows &&
				   cols == result.cols);
#endif			
			vDSP_vsmul(data, 1, &scalar, result.data, 1, rows*cols);
			
		}
		
		inline void multiply(float scalar)
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif
			vDSP_vsmul(data, 1, &scalar, data, 1, rows*cols);
		}
		

		
		
		inline void divide(const Mat &rhs, Mat &result) const 
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(result.data != NULL);
			assert(rows == rhs.rows && 
				   rhs.rows == result.rows &&
				   cols == rhs.cols && 
				   rhs.cols == result.cols);
#endif			
			vDSP_vdiv(rhs.data, 1, data, 1, result.data, 1, rows*cols);
			
		}
		
		inline void divide(const Mat &rhs)
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows &&
				   cols == rhs.cols);
#endif			
			vDSP_vdiv(rhs.data, 1, data, 1, data, 1, rows*cols);
		}
		
		inline void divide(float scalar, Mat &result) const 
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(result.data != NULL);
			assert(rows == result.rows &&
				   cols == result.cols);
#endif	
			
			vDSP_vsdiv(data, 1, &scalar, result.data, 1, rows*cols);
		}
		
		inline void divide(float scalar)
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif
			vDSP_vsdiv(data, 1, &scalar, data, 1, rows*cols);
		}
        
        inline void divideUnder(float scalar, Mat &result) const 
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(result.data != NULL);
			assert(rows == result.rows &&
				   cols == result.cols);
#endif	
			
			vDSP_svdiv(&scalar, data, 1, result.data, 1, rows*cols);
		}
		
		inline void divideUnder(float scalar)
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif
			vDSP_svdiv(&scalar, data, 1, data, 1, rows*cols);
		}
		
		inline void add(const Mat &rhs, Mat &result) const 
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(result.data != NULL);
			assert(rows == rhs.rows && 
				   rhs.rows == result.rows &&
				   cols == rhs.cols && 
				   rhs.cols == result.cols);
#endif			
			vDSP_vadd(data, 1, rhs.data, 1, result.data, 1, rows*cols);
		}
		
		inline void add(const Mat &rhs)
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows &&
				   cols == rhs.cols);
#endif			
			vDSP_vadd(data, 1, rhs.data, 1, data, 1, rows*cols);
		}
		
		inline void add(float scalar)
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			vDSP_vsadd(data, 1, &scalar, data, 1, rows*cols);
		}
		
		inline void subtract(const Mat &rhs, Mat &result) const 
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(result.data != NULL);
			assert(rows == rhs.rows && 
				   rhs.rows == result.rows &&
				   cols == rhs.cols && 
				   rhs.cols == result.cols);
#endif			
			vDSP_vsub(rhs.data, 1, data, 1, result.data, 1, rows*cols);
			
		}
		
		inline void subtract(const Mat &rhs)
		{
#ifdef DEBUG			
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(rows == rhs.rows &&
				   cols == rhs.cols);
#endif			
			vDSP_vsub(rhs.data, 1, data, 1, data, 1, rows*cols);
		}
		
		inline void subtract(float scalar)
		{
#ifdef DEBUG			
			assert(data != NULL);
#endif			
			float rhs = -scalar;
			vDSP_vsadd(data, 1, &rhs, data, 1, rows*cols);
		}
			
		
		inline void GEMM(const Mat &rhs, Mat &result) const 
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(result.data != NULL);
			assert(rows == result.rows &&
				   rhs.cols == result.cols &&
				   cols == rhs.rows);
#endif
			
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, result.rows, result.cols, cols, 1.0f, data, cols, rhs.data, rhs.cols, 0.0f, result.data, result.cols);
			//vDSP_mmul(data, 1, rhs.data, 1, result.data, 1, result.rows, result.cols, cols);
			
		}
		
		inline Mat GEMM(const pkm::Mat &rhs) const
		{
#ifdef DEBUG
			assert(data != NULL);
			assert(rhs.data != NULL);
			assert(cols == rhs.rows);
#endif
			
			Mat gemmResult(rows, rhs.cols);
			
			//printf("lda: %d\nldb: %d\nldc: %d\n", rows, rhs.rows, gemmResult.rows); 
			cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, gemmResult.rows, gemmResult.cols, cols, 1.0f, data, cols, rhs.data, rhs.cols, 0.0f, gemmResult.data, gemmResult.cols);
			//vDSP_mmul(data, 1, rhs.data, 1, gemmResult.data, 1, gemmResult.rows, gemmResult.cols, cols);
			return gemmResult;
			
		}
				
		inline void setTranspose()
		{
#ifdef DEBUG      
			assert(data != NULL);
            if (bUserData) {
                print("[Warning]: Transposing user data!");
            }
#endif
            float *temp_data = (float *)malloc(sizeof(float)*rows*cols);
			vDSP_mtrans(data, 1, temp_data, 1, cols, rows);
			cblas_scopy(rows*cols, temp_data, 1, data, 1);
            free(temp_data);
            temp_data = NULL;
            int tempvar = cols;
            cols = rows;
            rows = tempvar;
		}
		
		Mat getTranspose() const;
		
		
		// diagonalize the vector into a square matrix with 
		// the current data vector along the diagonal
		inline void setDiag()
		{
#ifdef DEBUG
			assert(data != NULL);
            if(bUserData)
            {
                cout << "[WARNING] Pointer to user data will be resized and therefore possibly lost/leaked.  Up to the user to free this memory!" << endl;
            }
#endif	
			if((rows == 1 && cols > 1) || (cols == 1 && rows > 1))
			{
                
				int diagonal_elements = MAX(rows,cols);
				
				// create a square matrix
                float *temp_data = (float *)malloc(diagonal_elements*diagonal_elements*sizeof(float));
				
				// set values to 0
				vDSP_vclr(temp_data, 1, diagonal_elements*diagonal_elements);
				
				// set diagonal elements to the current vector in data
				for (int i = 0; i < diagonal_elements; i++) {
					temp_data[i*diagonal_elements+i] = data[i];
				}
				
				// store in data
				rows = cols = diagonal_elements;
				std::swap(data, temp_data);
                
                if(!bUserData)
                {
                    free(temp_data);
                    temp_data = NULL;
                }
			}
            
		}
		Mat getDiag();
		
        void abs();
        
        // returns a new matrix with each el the abs(el)
        static Mat abs(const Mat &A);
		
        /*
        // returns a new matrix with each el the log(el)
		static Mat log(Mat &A);
		
		// returns a new matrix with each el the exp(el)
		static Mat exp(Mat &A);
		*/
        
		// returns a new diagonalized matrix version of A
		static Mat diag(const Mat &A);
		
		// get a new identity matrix of size dim x dim
		static Mat identity(int dim);
		
		static Mat zeros(int rows, int cols)
		{
			return Mat(rows, cols, true);
		}
		
		// set every element to a random value between low and high
		void setRand(float low = 0.0, float high = 1.0);
		
		// create a random matrix
		static Mat rand(int r, int c, float low = 0.0, float high = 1.0);
		
		// sum across rows or columns creating a vector from a matrix, or a scalar from a vector
		Mat sum(bool across_rows = true);
		
		// repeat a vector for size times
		static Mat repeat(const Mat &m, int size)
		{
			// repeat a column vector across cols
			if(m.rows > 1 && m.cols == 1 && size > 1)
			{
				Mat repeated_matrix(size, m.rows);
				for (int i = 0; i < size; i++) {
					cblas_scopy(m.rows, m.data, 1, repeated_matrix.data + (i*m.rows), 1);
				}
				repeated_matrix.setTranspose();
				return repeated_matrix;
			}
			else if( m.rows == 1 && m.cols > 1 && size > 1)
			{
				Mat repeated_matrix(size, m.cols, 5.0f);
				
				for (int i = 0; i < size; i++) {
					cblas_scopy(m.cols, m.data, 1, repeated_matrix.data + (i*m.cols), 1);
				}
				return repeated_matrix;
			}
			else {
				printf("[ERROR]: repeat requires a vector and a size to repeat on.");
				Mat a;
				return a;
			}

		}
		
		// repeat a vector for size times
		static void repeat(Mat &dst, const Mat &m, int size)
		{
			// repeat a column vector across cols
			if(m.rows > 1 && m.cols == 1 && size > 1)
			{
				dst.reset(size, m.rows);
				for (int i = 0; i < size; i++) {
					cblas_scopy(m.rows, m.data, 1, dst.data + (i*m.rows), 1);
				}
				dst.setTranspose();
			}
			else if( m.rows == 1 && m.cols > 1 && size > 1)
			{
				dst.reset(size, m.cols);
				
				for (int i = 0; i < size; i++) {
					cblas_scopy(m.cols, m.data, 1, dst.data + (i*m.cols), 1);
				}
			}
			else {
				printf("[ERROR]: repeat requires a vector and a size to repeat on.");
				
			}
			
		}
		
		static float meanMagnitude(const float *buf, int size)
		{
			float mean;
			vDSP_meamgv(buf, 1, &mean, size);
			return mean;
		}
        
        
		static float l1norm(const float *buf1, const float *buf2, int size)
		{
			int a = size;
			float diff = 0;
			const float *p1 = buf1, *p2 = buf2;
			while (a) {
				diff += fabs(*p1++ - *p2++);
				a--;
			}
			return diff;///(float)size;
		}
		
		static float sumOfAbsoluteDifferences(const float *buf1, const float *buf2, int size)
		{
			int a = size;
			float diff = 0;
			const float *p1 = buf1, *p2 = buf2;
			while (a) {
				diff += fabs(*p1++ - *p2++);
				a--;
			}
			return diff/(float)size;
		}
		
		static float mean(const float *buf, int size, int stride = 1)
		{
			float val;
			vDSP_meanv(buf, stride, &val, size);
			return val;
		}
        
        static float mean(const Mat &m, int stride = 1)
        {
            float val;
            vDSP_meanv(m.data, stride, &val, m.rows * m.cols);
            return val;
        }
        
		static float var(const float *buf, int size, int stride = 1)
		{
			float m = mean(buf, size, stride);
			float v = 0;
			float sqr = 0;
			const float *p = buf;
			int a = size;
			while (a) {
				sqr = (*p - m);
				p += stride;
				v += sqr*sqr;
				a--;
			}
			return v/(float)size;
		}
        
        static float stddev(const float *buf, int size, int stride = 1)
		{
			float m = mean(buf, size, stride);
			float v = 0;
			float sqr = 0;
			const float *p = buf;
			int a = size;
			while (a) {
				sqr = (*p - m);
				p += stride;
				v += sqr*sqr;
				a--;
			}
			return sqrtf(v/(float)size);
		}
        
        float rms()
        {
            float val;
            vDSP_rmsqv(data, 1, &val, rows*cols);
            return val;
        }
		
		static float rms(const float *buf, int size)
		{
			float val;
			vDSP_rmsqv(buf, 1, &val, size);
			return val;
		}
		
		static float min(const Mat &A)
		{
			float minval;
			vDSP_minv(A.data, 1, &minval, A.rows*A.cols);
			return minval;
		}
        
        static unsigned long minIndex(const Mat &A)
        {
            float minval;
            unsigned long minidx;
            vDSP_minvi(A.data, 1, &minval, &minidx, A.rows*A.cols);
            return minidx;
        }
        
        void min(float &val, unsigned long &idx) const
        {
            vDSP_minvi(data, 1, &val, &idx, rows*cols);
        }
		
		static float max(const Mat &A)
		{
			float maxval;
			vDSP_maxv(A.data, 1, &maxval, A.rows*A.cols);
			return maxval;
		}
        
        static unsigned long maxIndex(const Mat &A)
        {
            float maxval;
            unsigned long maxidx;
            vDSP_maxvi(A.data, 1, &maxval, &maxidx, A.rows*A.cols);
            return maxidx;
        }
        
        void max(float &val, unsigned long &idx)
        {
            vDSP_maxvi(data, 1, &val, &idx, rows*cols);
        }
        
        float sumAll()
        {
			float sumval;
			vDSP_sve(data, 1, &sumval, rows*cols);
			return sumval;
        }
		
		static float sum(const Mat &A)
		{
			float sumval;
			vDSP_sve(A.data, 1, &sumval, A.rows*A.cols);
			return sumval;
		}
		
		Mat var(bool row_major = true) const
		{
#ifdef DEBUG			
            assert(data != NULL);
            assert(rows >0 &&
                   cols >0);
#endif	
            if (row_major) {
                if (rows == 1) {
                    return *this;
                }
                Mat newMat(1, cols);
                
                for(int i = 0; i < cols; i++)
                {
                    newMat.data[i] = var(data + i, rows, cols);
                }
                return newMat;
            }
            else {
                if (cols == 1) {
                    return *this;
                }
                Mat newMat(rows, 1);
                for(int i = 0; i < rows; i++)
                {
                    newMat.data[i] = var(data + i*cols, cols, 1);
                }
                return newMat;
            }	
		}
		
        Mat stddev(bool row_major = true) const
		{
#ifdef DEBUG			
            assert(data != NULL);
            assert(rows >0 &&
                   cols >0);
#endif	
            if (row_major) {
                if (rows == 1) {
                    return *this;
                }
                Mat newMat(1, cols);
                
                for(int i = 0; i < cols; i++)
                {
                    newMat.data[i] = stddev(data + i, rows, cols);
                }
                return newMat;
            }
            else {
                if (cols == 1) {
                    return *this;
                }
                Mat newMat(rows, 1);
                for(int i = 0; i < rows; i++)
                {
                    newMat.data[i] = stddev(data + i*cols, cols, 1);
                }
                return newMat;
            }	
		}
        
        
        Mat mean(bool row_major = true) const
        {
#ifdef DEBUG			
            assert(data != NULL);
            assert(rows >0 &&
                   cols >0);
#endif	
            if (row_major) {
                
                if (rows == 1) {
                    Mat newMat(1, cols, data, true);
                    return newMat;
                }
                Mat newMat(1, cols);
                
                for(int i = 0; i < cols; i++)
                {
                    newMat.data[i] = mean(data + i, rows, cols);
                }
                return newMat;
            }
            else {
                if (cols == 1) {
                    return *this;
                }
                Mat newMat(rows, 1);
                for(int i = 0; i < rows; i++)
                {
                    newMat.data[i] = mean(data + i*cols, cols, 1);
                }
                return newMat;
            }

        }
        
        inline void zNormalize()
        {
            float mean, stddev;
            int size = rows * cols;
            getMeanAndStdDev(mean, stddev);
            
            // subtract mean
			float rhs = -mean;
			vDSP_vsadd(data, 1, &rhs, data, 1, size);
            
            // divide by std dev
            vDSP_vsdiv(data, 1, &stddev, data, 1, size);
        }
        
        inline void zNormalizeEachCol()
        {
            float mean, stddev;
            float sumval, sumsquareval;
            int size = rows;
            if (size > 1) {
                for (int i = 0; i < cols; i++) {
                    vDSP_sve(data + i, cols, &sumval, size);
                    vDSP_svesq(data + i, cols, &sumsquareval, size);
                    mean = sumval / (float) size;
                    stddev = sqrtf( sumsquareval / (float) size - mean * mean);
                    
                    // subtract mean
                    float rhs = -mean;
                    vDSP_vsadd(data + i, cols, &rhs, data + i, cols, size);
                    
                    // divide by std dev
                    vDSP_vsdiv(data + i, cols, &stddev, data + i, cols, size);
                }
            }
        }
        
        inline void centerEachCol()
        {
            float mean;
            float sumval;
            int size = rows;
            if (size > 1) {
                for (int i = 0; i < cols; i++) {
                    vDSP_sve(data + i, cols, &sumval, size);
                    mean = sumval / (float) size;
                    
                    // subtract mean
                    float rhs = -mean;
                    vDSP_vsadd(data + i, cols, &rhs, data + i, cols, size);
                }
            }
        }
        
        
        inline void getMeanAndStdDev(Mat &meanMat, Mat &stddevMat) const
        {
            meanMat.reset(1, cols);
            stddevMat.reset(1, cols);
            
            float mean, stddev;
            float sumval, sumsquareval;
            int size = rows;
            if (size == 1) {
                cblas_scopy(cols, data, 1, meanMat.data, 1);
                stddevMat.setTo(1.0);
            }
            else if (size > 1) {
                for (int i = 0; i < cols; i++) {
                    vDSP_sve(data + i, cols, &sumval, size);
                    vDSP_svesq(data + i, cols, &sumsquareval, size);
                    mean = sumval / (float) size;
                    stddev = sqrtf( sumsquareval / (float) size - mean * mean);
                    
                    meanMat[i] = mean;
                    stddevMat[i] = stddev;
                }
            }
        }
        
        
        inline void getMeanAndStdDev(float &mean, float &stddev) const
        {
            float sumval, sumsquareval;
            int size = rows * cols;
			vDSP_sve(data, 1, &sumval, size);
            vDSP_svesq(data, 1, &sumsquareval, size);
            mean = sumval / (float) size;
            stddev = sqrtf( sumsquareval / (float) size - mean * mean);
        }
		
		// rescale the values in each row to their maximum
		void setNormalize(bool row_major = true);
		
		void normalizeRow(int r)
		{
			float min, max;
			vDSP_minv(&(data[r*cols]), 1, &min, cols);
			vDSP_maxv(&(data[r*cols]), 1, &max, cols);
			float height = max-min;
			min = -min;
			vDSP_vsadd(&(data[r*cols]), 1, &min, &(data[r*cols]), 1, cols);
			if (height != 0) {
				vDSP_vsdiv(&(data[r*cols]), 1, &height, &(data[r*cols]), 1, cols);	
			}
		}
		
		void divideEachVecByMaxVecElement(bool row_major);
		void divideEachVecBySum(bool row_major);
        
        void inv2x2()
        {
#ifdef DEBUG
            assert(rows == 2);
            assert(cols == 2);
#endif
            float det = 1.0 / (data[0]*data[3] - data[2]*data[1]);
            float a = data[0];
            float b = data[1];
            float c = data[2];
            float d = data[3];
            data[0] = d * det;
            data[1] = -b * det;
            data[2] = -c * det;
            data[3] = a * det;
        }
        
        /*
        void inv()
        {
#ifdef DEBUG
            assert(rows == cols); // must be square
#endif
            int error = 0;
            float *workspace = (float *)malloc(MULTIPLE_OF_4(rows)*sizeof(float));
            int *pivot = (int *)malloc(MULTIPLE_OF_4(rows*rows)*sizeof(int));
            
            sgetrf_(&rows, &rows, data, &rows, pivot, &error);
            
#ifdef DEBUG
            if (error != 0) {
                cerr << "[pkmMatrix]: Error 1";
            }
#endif
            sgetri_(&rows, data, &rows, pivot, workspace, &rows, &error);
#ifdef DEBUG
            if (error != 0) {
                cerr << "[pkmMatrix]: Error 2";
            }
#endif
            free(workspace);
            free(pivot);
        }
        */
        
        // input is 1 x d dimensional vector
        // mean is 1 x d dimensional vector
        // sigma is d x d dimensional matrix
        static float gaussianPosterior(const Mat &input, Mat mean, Mat sigma)
        {
#ifdef DEBUG
            assert(input.cols == mean.cols);
            assert(input.cols == sigma.rows);
            assert(input.cols == sigma.cols);
#endif
            float A = 1.0 / (powf(M_PI * 2.0, input.cols / 2.0) * sqrtf(sigma[0]*sigma[3] - sigma[2]*sigma[1]));
            Mat inputCopy = input;
            inputCopy.subtract(mean);
            sigma.inv2x2();
            Mat a = inputCopy.GEMM(sigma);
            Mat l = inputCopy;
            l.setTranspose();
            Mat b = a.GEMM(l);
            return (A * expf(-0.5 * b[0]));
        }
        
        void sqr()
        {
			vDSP_vmul(data, 1, data, 1, data, 1, rows*cols);
        }
        
        static Mat sqr(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
			vDSP_vmul(b.data, 1, b.data, 1, newMat.data, 1, b.rows*b.cols);
            return newMat;
        }
        
        void sqrt()
        {
            int size = rows*cols;
            vvsqrtf(data, data, &size);
        }
        
        static Mat sqrt(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvsqrtf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void sin()
        {
            int size = rows*cols;
            vvsinf(data, data, &size);
        }
        
        static Mat sin(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvsinf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void cos()
        {
            int size = rows*cols;
            vvcosf(data, data, &size);
        }
        
        static Mat cos(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvcosf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void pow(float p)
        {
            int size = rows*cols;
            vvpowf(data, &p, data, &size);
        }
        
        static Mat pow(const Mat &b, float p)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvpowf(newMat.data, &p, b.data, &size);
            return newMat;
        }
        
        void log()
        {
            int size = rows*cols;
            vvlogf(data, data, &size);
        }
        
        static Mat log(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvlogf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void log10()
        {
            int size = rows*cols;
            vvlog10f(data, data, &size);
        }
        
        static Mat log10(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvlog10f(newMat.data, b.data, &size);
            return newMat;
        }
        
        void exp()
        {
            int size = rows*cols;
            vvexpf(data, data, &size);
        }
        
        static Mat exp(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvexpf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void floor()
        {
            int size = rows*cols;
            vvfloorf(data, data, &size);
        }
        
        static Mat floor(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvfloorf(newMat.data, b.data, &size);
            return newMat;
        }
        
        void ceil()
        {
            int size = rows*cols;
            vvceilf(data, data, &size);
        }
        
        static Mat ceil(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            int size = b.rows*b.cols;
            vvceilf(newMat.data, b.data, &size);
            return newMat;
        }
        
        static Mat sgn(const Mat &b)
        {
            Mat newMat(b.rows, b.cols);
            float *p = b.data;
            float *p2 = newMat.data;
            for (int i = 0; i < b.rows*b.cols; i++) {
                *p2++ = signum<float>(*p++);
            }
            return newMat;
        }
        
        static Mat resize(const Mat &a, int newSize)
        {
            int originalSize = a.size();
            Mat b(1, newSize);
            float factor = (float)((newSize - 1) / (float)(originalSize-1));
            for (int i = 0; i < newSize; i++) {
                b[i] = i / factor;
            }
            Mat c(1, newSize);
            vDSP_vlint(a.data, b.data, 1, c.data, 1, newSize, originalSize);
            return c;
        }
        
        inline int size() const
        {
            return rows * cols;
        }
        
        inline float *last()
        {
#ifdef DEBUG
			assert(data != NULL);
#endif	
            return data + (rows * cols - 1);
        }
        
        inline float *first()
        {
#ifdef DEBUG
			assert(data != NULL);
#endif	
            return data;
        }
        
        inline int svd(Mat &U, Mat &S, Mat &V_t)
        {
//            print();
            
            __CLPK_integer m = rows;
            __CLPK_integer n = cols;

            __CLPK_integer lda = m;
            __CLPK_integer ldu = m;
            __CLPK_integer ldv = n;
            
            int nSVs = m > n ? n : m;
            
            U.reset(m, m);
            V_t.reset(n, n);
            S.reset(1, nSVs);
            
            float workSize;
            
            __CLPK_integer lwork = -1;
            __CLPK_integer info = 0;

            // iwork dimension should be at least 8*min(m,n)
            __CLPK_integer iwork[8*nSVs];
            
            //https://groups.google.com/forum/#!topic/julia-dev/mmgO65i6-fA sdd (divide/conquer, better if memory is available, for large matrices) versus svd (qr)
            //http://docs.oracle.com/cd/E19422-01/819-3691/dgesvd.html
            
            // call svd to query optimal work size:
            char job = 'A';
            sgesdd_(&job, &m, &n, data, &lda, S.data, U.data, &ldu, V_t.data, &ldv, &workSize, &lwork, iwork, &info);
            
            lwork = (int)workSize;
            float work[lwork];
            
            // actual svd
            sgesdd_(&job, &m, &n, data, &lda, S.data, U.data, &ldu, V_t.data, &ldv, work, &lwork, iwork, &info);
            
            // Check for convergence
            if( info > 0 ) {
                printf( "[pkm::Mat]::svd(...) sgesvd_() failed to converge.\\n" );
            }
            
            return info;

        }
        
        void copyToDouble(double *ptr) const
        {
            vDSP_vspdp(data, 1, ptr, 1, rows*cols);
        }
        
        
        void copyFromDouble(const double *ptr, size_t rows, size_t cols)
        {
            resize(rows, cols);
            vDSP_vdpsp(ptr, 1, data, 1, size());
        }
        
        bool save(string filename)
        {
            FILE *fp;
            fp = fopen(filename.c_str(), "w");
            if(fp)
            {
                fprintf(fp, "%d %d\n", rows, cols);
                for(int i = 0; i < rows; i++)
                {
                    for(int j = 0; j < cols; j++)
                    {
                        fprintf(fp, "%f, ", data[i*cols + j]);
                    }
                    fprintf(fp,"\n");
                }
                fclose(fp);
                return true;
            }
            else
            {
                return false;
            }
        }
        
        bool saveCSV(string filename)
        {
            FILE *fp;
            fp = fopen(filename.c_str(), "w");
            if(fp)
            {
//                fprintf(fp, "%d %d\n", rows, cols);
                for(int i = 0; i < rows; i++)
                {
                    for(int j = 0; j < cols; j++)
                    {
                        fprintf(fp, "%f, ", data[i*cols + j]);
                    }
                    fprintf(fp,"\n");
                }
                fclose(fp);
                return true;
            }
            else
            {
                return false;
            }
        }
        
        bool load(string filename)
        {
            if (bAllocated && !bUserData) {
                free(data); data = NULL;
                rows = cols = 0;
            }
            FILE *fp;
            fp = fopen(filename.c_str(), "r");
            if (fp) {
                fscanf(fp, "%d %d\n", &rows, &cols);
                data = (float *)malloc(sizeof(float) * MULTIPLE_OF_4(rows * cols));
                for(int i = 0; i < rows; i++)
                {
                    for(int j = 0; j < cols; j++)
                    {
                        fscanf(fp, "%f, ", &(data[i*cols + j]));
                    }
                    fscanf(fp, "\n");
                }
                fclose(fp);
                bAllocated = true;
                return true;
            }
            else {
                return false;
            }
        }
        
        bool load(string filename, int r, int c)
        {
            if (bAllocated && !bUserData) {
                free(data); data = NULL;
                rows = cols = 0;
            }
            FILE *fp;
            fp = fopen(filename.c_str(), "r");
            if (fp) {
                rows = r;
                cols = c;
                data = (float *)malloc(sizeof(float) * MULTIPLE_OF_4(rows * cols));
                for(int i = 0; i < rows; i++)
                {
                    for(int j = 0; j < cols; j++)
                    {
                        fscanf(fp, "%f, ", &(data[i*cols + j]));
                    }
                    fscanf(fp, "\n");
                }
                fclose(fp);
                bAllocated = true;
                return true;
            }
            else {
                return false;
            }
        }
		
		// simple print output (be careful with large matrices!)
		void print(bool row_major = true, char delimiter = ',');
		// only prints maximum of 5 rows/cols
		void printAbbrev(bool row_major = true, char delimiter = ',');
		
        
        
		/////////////////////////////////////////
		
		int current_row;	// for circular insertion
		bool bCircularInsertionFull;
		int rows;
		int cols;
		
		float *data;
		
		bool bAllocated;
		bool bUserData;
        
        
    protected:
        void releaseMemory()
        {
            if(bAllocated)
            {
                if (!bUserData) {
                    assert(data != 0);
                    free(data);
                    data = NULL;
                }
            }
        }
        
	};
    
	
};