#include "ReadInput.hpp"
#include "CNFDecomp.hpp"
#include "Algorithm.hpp"
#include "Printing.hpp"
#include "Verifier.hpp"

#include <chrono>
#include <iostream>




using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    cout << "Expected format: decomp <input-file>" << endl;
  }
  else
  {
    try
    {
        
#if MYDEBUG
      cout <<"*** debug mode *****"<<endl;  
#endif
      /* Path to the input file */
      string inputPath(argv[1]);

      /* Parses the file into a CNF specification */
      CNFSpec f = loadDIMACS(inputPath);

      cout << "=== CNF  Overall Specification ===" << endl;
      cout << "Input variables: ";
      print(f.inputVars(), "x");
      cout << endl;
      cout << "Output variables: ";
      print(f.outputVars(), "y");
      cout << endl;
      cout << "CNF formula:" << endl;
      print(f, "x", "y");

      /* Decompose specification into (F1, F2) */
      CNFChain cnfChain = cnfDecomp(f);

      cout << "=== F1 ===" << endl;
      print(cnfChain.first, "x", "z");
      cout << "=== F2 ===" << endl;
      print(cnfChain.second, "z", "y");

      auto start = system_clock::now(); /*< start timing */

      //********************************   This is the main method of the algorithm ********************************
      /* Call the synthesis algorithm */
      Vector<Set<BVar>> mssList =
	//BAFAlgorithm(cnfChain.first, cnfChain.second); //        This is the non Decomposable version
	BAFConnectedComponents(cnfChain.first, cnfChain.second);  //This is the decomposable version
        
      //************************************************************************************************************  

      auto time = duration_cast<milliseconds>(system_clock::now() - start); /*< stop timing */

      
      printf("**************************************************************************************\n");
      cout << "=== Computed Overall MSS ===" << endl;
      
      for (const Set<BVar>& mss : mssList)
      {
	cout << "MSS: ";

	Set<BVar> indicatorAssignment = setDifference(mss, f.outputVars());
	print(indicatorAssignment, "z");

	cout << " |-> Output assignment: ";

	Set<BVar> outputAssignment = setDifference(mss, indicatorAssignment);
	print(outputAssignment, "y");

	cout << endl;
      }

      cout << "=== Stats ===" << endl;

      cout << "MSS count: " << mssList.size() << " sets" << endl;
      cout << "Running time: " << time.count() << "ms" << endl;
      
      
    /* DF: 4/4/2018 Adding a verifier object that is generated by the MSS list and perform various verification on the list.*/
      
    cout << "=== Verifying ===" << endl;
    
      Verifier MyVerifier(mssList,f);
    MyVerifier.VerifyList();
      
      
    
    
    
    //DF: a small checking code for the eval function.
      /*
       * cout<<"====Verifying my code ====" <<endl;
      CNFClause tmpClause(5);
       tmpClause|=4;
      tmpClause|=3;
     tmpClause|=-6;
     
     cout<<"c1:";
      print(tmpClause,"x");
      cout<<endl;
      
      
      
      
      CNFClause tmpClause2(7);
      tmpClause2|=4;
      tmpClause2|=-3;
      
      cout<<"c2:";
      print(tmpClause2,"x");
      cout<<endl;
      
      Set<BVar> tmpAss;
      tmpAss.insert(7);
      tmpAss.insert(2);      
      tmpAss.insert(3);           
      

   
      
      print(tmpAss,"g");   
      cout<<endl;
      
      CNFFormula tmpFormula;
      tmpFormula&=tmpClause;
      tmpFormula&=tmpClause2;
       
      bool resa = tmpClause.eval(tmpAss);
      cout<<"result c1:"<<resa<<endl;
      
      bool resb = tmpClause2.eval(tmpAss);
      cout<<"result c2:"<<resb<<endl;
      
      bool resTotal = tmpFormula.eval(tmpAss);
      cout<<"result Total:"<<resTotal<<endl;
      
      */
    
}
    
    
    catch (const runtime_error& e)
    {
      cout << e.what() << endl;
    }
  }
}
