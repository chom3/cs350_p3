#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

//Declaration of the thread that will be used. 
void *multiplyIndices(void *arg);

/**
 * Create a struct that will help us with making a pointer to a matrix
 */
 struct invals
 {
     int m;
     int n;
     int p;
     int ***passArray;
 };
 
//Array initalization. The 3d array will be used to calculate the additions
int **array1;
int **array2;
int **finalArray;
int ***multAns;
//Initiate rows/columns for both matrices A and B
int rowsA = 0;
int colsA = 0;
int rowsB = 0;
int colsB = 0;
/**
* Helper function will push the values into the matrix
* Will give the size of the columns.
*/
int ReadNumbers(const string &s, vector<int> &v) {
	istringstream is(s);
	int n;
	while (is >> n)
	{
		v.push_back(n);
	}
	return v.size();
}

/**Want to compute product of two matrices.
At least 3 by 3
Allocate space for input matrices
Allocate space for the threads to insert the values for the resulting matrix in dynamic storage -- output matrix
Waits for all 'n' threads to complete.
Print out resulting matrix.
in the example of input.txt the answer is :
	38	32	14
	67	58	16
	122	104	38
	160	138	41
*/

int main(int argc, char *argv[])
{
	if (argc != 3)
		cout << "usage: " << argv[0] << " <filename>\n";
	else {
		// We assume argv[1] is a filename to open
		ifstream input(argv[1]);
		ofstream output(argv[2]);
		// Always check to see if file opening succeeded
		if (!input.is_open())
		{
			cout << "Could not open file\n";
		}
		//If the input is open, assume that it is correct for matrix input.
		else
		{
			//iterators
			int a;
            int b;

			//Start on Matrix A
			string line;
			getline(input, line);
			vector <int> v;

			//Get the columns of Matrix A
			//cout << "Matrix A" << endl;
			colsA = ReadNumbers(line, v);
			//cout << "cols:" << colsA << endl;
			//Detect for 10 asterisks, if found, that's the end of Matrix A
			for (a = 1; a < 32767; a++){
				getline(input, line);
				if (line.find("***") != string::npos) {
					break;
				}
				ReadNumbers(line, v);
			}
			//How many lines there were = number of rows
			rowsA = a;
			//cout << "rows:" << rowsA << endl;
			//want to copy the vector into a 2d array for easy accessibility
			array1 = new int*[rowsA];
			for (int i = 0; i < rowsA; ++i)
			{
    			array1[i] = new int[colsA];
			}
			for (int i = 0; i < rowsA; i++)
			{
				for (int j = 0; j < colsA; j++)
				{
					array1[i][j] = v[i*colsA + j];
				}
			}

			//Start on Matrix B
			//cout << "Matrix B" << endl;
			//Use a different vector
			vector <int> v2;
			string line2;

			//Find # of columns 
			getline(input, line2);
			colsB = ReadNumbers(line2, v2);
			//cout << "cols:" << colsB << endl;

			//Instead we search for the last line
			b = 1;
			while (getline(input, line2))
			{
				b++;
				ReadNumbers(line2, v2);
			}

			//# of lines = # of rows
			rowsB = b;
			//cout << "rows:" << rowsB << endl;

			array2 = new int*[rowsB];
			for (int i = 0; i < rowsB; ++i)
			{
    			array2[i] = new int[colsB];
			}
			for (int i = 0; i < rowsB; i++)
			{
				for (int j = 0; j < colsB; j++)
				{
					array2[i][j] = v2[i*colsB + j];
				}
			}
			//Check matrix A
			// for (int i = 0; i < rowsA; i++)
			// {
			// 	for (int j = 0; j < colsA; j++)
			// 	{
			// 		cout << v[i*colsA + j] << "\t";
			// 	}
			// 	cout << endl;
			// } 
			// cout << "***********" << endl;
			//check new matrixA
			// for (int i = 0; i< rowsA; i++)
			// {
			// 	for (int j = 0; j < colsA; j++)
			// 	{
			// 		cout << array1[i][j] << "\t";
			// 	}
			// 	cout << endl;
			// }
			//Check matrix B
			// for (int i = 0; i < rowsB; i++)
			// {
			// 	for (int j = 0; j < colsB; j++)
			// 	{
			// 		cout << v2[i*colsB + j] << "\t";
			// 	}
			// 	cout << endl;
			// }
			// cout << "**********" << endl;
			// Check new array matrix B
			// for (int i = 0; i < rowsB; i++)
			// {
			// 	for (int j = 0; j < colsB; j++)
			// 	{
			// 		cout << array2[i][j] << "\t";
			// 	}
			// 	cout << endl;
			// }
			//Is the matrix multipliable?
			if (colsA == rowsB)
			{
			    //cout << "Multipliable" << endl;
                //number of multiplications needed in a m x l and l x n matrix is lmn
                int threadsNeeded = rowsA * rowsB * colsB;
				vector<pthread_t> tid(threadsNeeded);
			   	multAns = new int**[rowsA];
				for (int i = 0; i < rowsA; i++)
				{
	    			multAns[i] = new int*[colsA];
	    			for (int j = 0; j < colsA; j++)
	    			{
	    				multAns[i][j] = new int[colsB];
	    			}
				}
			   	int count = 0;
				for (int i = 0; i < rowsA; i++)
				{
					for (int j = 0; j < colsA; j++)
					{
						for (int k = 0; k < colsB; k++)
						{
							invals *result = new invals;
							// cout << i << endl;
							// cout << j << endl;
							// cout << k << endl;
							result->m = i;
							result->n = j;
							result->p = k;
							result->passArray = multAns;
							pthread_create(&tid[count], NULL, multiplyIndices, (void *)result);
							count++;
						}
					}
				}
			    //join the threads when all the threads are finished
			    for (int t = 0; t < threadsNeeded; t++)
			    {
					pthread_join(tid[t], NULL);
			    }
			    //Check the multiply array
			    // for (int i = 0; i < (rowsA); i++)
			    // {
			    // 	for (int j = 0; j < colsA; j++)
			    // 	{
			    // 		for (int k = 0; k < colsB; k++)
			    // 		{
			    // 			cout << multAns[i][j][k] << "\t";
			    // 		}
			    // 		cout << endl;
			    // 	}
			    // 	cout << endl;
			    // }
				//mxn * nxp = mxp
				finalArray = new int*[rowsA];
				for (int i = 0; i < rowsA; ++i)
				{
	    			finalArray[i] = new int[colsB];
				}
				//Add up the columns of the answer with multiplications into the final array
				for (int i = 0; i < rowsA; i++)
				{
					for (int j = 0; j < colsA; j++)
					{
						finalArray[i][j] = 0;
						for (int k = 0; k < rowsB; k++)
						{
							finalArray[i][j] += multAns[i][k][j];
						}
					}
				}
				//display the final matrix to console
				//write the final matrix to a file of your choosing
				for (int i = 0; i < rowsA; i++)
				{
					for (int j = 0; j < colsB; j++)
					{
						cout << finalArray[i][j] << "\t";
						output << finalArray[i][j] << "\t";
					}
					cout << endl;
					if (i < rowsA - 1)
					{
						output << endl;
					}
				}
				char anyChar;
				cout << "Type any key to continue..." << endl;
				cin >> anyChar;
			}
			else
			{
			    cout << "Input matrices are not multipliable." << endl;
			    exit(0);
			}
			//Delete all the heap allocations, free all blocks.
			for (int i = 0; i < rowsA; i++)
			{
    			delete [] array1[i];
			}
			for (int i = 0; i < rowsB; i++)
			{
    			delete [] array2[i];
			}
			for (int i = 0; i < (rowsA); i++)
			{
    			for (int j = 0; j < colsA; j++)
    			{
    				delete [] multAns[i][j];
    			}
			}
			for (int i = 0; i < rowsA; i++)
			{
				delete [] multAns[i];
			}
			for (int i = 0; i < rowsA; i++)
			{
				delete [] finalArray[i];
			}
			delete [] finalArray;
			delete [] multAns;
			delete [] array1;
			delete [] array2;
		}
		input.close();
		output.close();
	}
}

/**
 * Will carry out one multiplication and store it in an array.
 * 
 */
void *multiplyIndices(void *arg)
{
    invals* result = (invals*) arg;
    // cout << result->m << endl;
    // cout << result->n << endl;
    // cout << result->p << endl;
    result->passArray[result->m][result->n][result->p] = array1[result->m][result->n] * array2[result->n][result->p];
    delete result;
	return 0;
}

