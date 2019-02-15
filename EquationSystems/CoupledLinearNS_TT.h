///////////////////////////////////////////////////////////////////////////////
//
// File CoupledLinearNS.h
//
// For more information, please see: http://www.nektar.info
//
// The MIT License
//
// Copyright (c) 2006 Division of Applied Mathematics, Brown University (USA),
// Department of Aeronautics, Imperial College London (UK), and Scientific
// Computing and Imaging Institute, University of Utah (USA).
//
// License for the specific language governing rights and limitations under
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
// Description: Coupled Stokes solver scheme header
//
///////////////////////////////////////////////////////////////////////////////


#include "./CoupledLocalToGlobalC0ContMap.h"
#include "./IncNavierStokes.h"
#include "./CoupledLinearNS.h"
#include <MultiRegions/GlobalLinSys.h>
#include <MultiRegions/ExpList3DHomogeneous1D.h>
#include <MultiRegions/ExpList2D.h>
#include <boost/shared_ptr.hpp>
#include "../Eigen/Dense"
#include <LibUtilities/LinearAlgebra/NekTypeDefs.hpp>
//#include <MultiRegions/GlobalLinSysDirectStaticCond.h>

namespace Nektar
{     
    
      
    class CoupledLinearNS_TT: public CoupledLinearNS
    {
    public:
        friend class MemoryManager<CoupledLinearNS_TT>;
        
        /// Creates an instance of this class
        static SolverUtils::EquationSystemSharedPtr create(
            const LibUtilities::SessionReaderSharedPtr& pSession)
        {
            SolverUtils::EquationSystemSharedPtr p = MemoryManager<CoupledLinearNS_TT>::AllocateSharedPtr(pSession);
            p->InitObject();
            return p;
        }
        /// Name of class
        static std::string className;      

        void Solve(void);

        void SolveLinearNS(const Array<OneD, Array<OneD, NekDouble> > &forcing);
        
        void SolveLinearNS(const Array<OneD, Array<OneD, NekDouble> > &forcing,
                           Array<OneD, MultiRegions::ExpListSharedPtr> &fields,
                           MultiRegions::ExpListSharedPtr &pressure,
                           const int HomogeneousMode = 0);

        void SetUpCoupledMatrix(const NekDouble lambda = 0.0, 
                                const Array< OneD, Array<OneD, NekDouble>  > &Advfield = NullNekDoubleArrayofArray, 
                                bool IsLinearNSEquation = true);

        Array<OneD, CoupledLocalToGlobalC0ContMapSharedPtr> m_locToGloMap;

	void DefineRBspace(Eigen::MatrixXd RB_via_POD);

	Eigen::VectorXd trafoSnapshot(Eigen::VectorXd RB_via_POD, double kInvis);

	void trafoSnapshot_simple(Eigen::MatrixXd RB_via_POD);

        Eigen::MatrixXd DoTrafo(Array<OneD, Array<OneD, NekDouble> > snapshot_x_collection, Array<OneD, Array<OneD, NekDouble> > snapshot_y_collection, Array<OneD, NekDouble> param_vector);

	void load_snapshots(int number_of_snapshots);
        
        Array<OneD, Array<OneD, NekDouble> > m_ForcingTerm;
        Array<OneD, Array<OneD, NekDouble> > m_ForcingTerm_Coeffs;

        Array<OneD, Array<OneD, NekDouble> > PODmodes;
        Eigen::MatrixXd RB;
	Eigen::MatrixXd Get_no_advection_matrix(void);
	Eigen::MatrixXd Get_no_advection_matrix_ABCD(void);
	Eigen::MatrixXd Get_no_advection_matrix_pressure(void);
	Eigen::MatrixXd Get_advection_matrix(void);
	Eigen::MatrixXd Get_complete_matrix(void);

        Eigen::MatrixXd MtM;
        Eigen::MatrixXd RB_A;
        Eigen::MatrixXd RB_A_adv;
        Eigen::MatrixXd RB_A_no_adv;
        Eigen::MatrixXd RB_B;
        Eigen::MatrixXd RB_B_adv;
        Eigen::MatrixXd RB_B_no_adv;
        Eigen::MatrixXd RB_C;
        Eigen::MatrixXd RB_C_adv;
        Eigen::MatrixXd RB_C_no_adv;
        Eigen::MatrixXd RB_D;
        Eigen::MatrixXd RB_D_adv;
        Eigen::MatrixXd RB_D_no_adv;
        Eigen::MatrixXd RB_Dbnd;
        Eigen::MatrixXd RB_Dint;

	int no_dbc_in_loc;
	int no_not_dbc_in_loc;
	std::set<int> elem_loc_dbc;
	std::set<int> elem_not_loc_dbc;

	Eigen::VectorXd curr_f_bnd;
	Eigen::VectorXd curr_f_p;
	Eigen::VectorXd curr_f_int;

	Array<OneD, Array<OneD, NekDouble> > snapshot_x_collection;
	Array<OneD, Array<OneD, NekDouble> > snapshot_y_collection;
        
	CoupledLinearNS_TT(const LibUtilities::SessionReaderSharedPtr &pSesssion);

        void setDBC(Eigen::MatrixXd collect_f_all);
        void set_MtM();
        void DoInitialiseAdv(Array<OneD, NekDouble> myAdvField_x, Array<OneD, NekDouble> myAdvField_y);
        Eigen::MatrixXd remove_cols_and_rows(Eigen::MatrixXd the_matrix, std::set<int> elements_to_be_removed);
        Eigen::VectorXd remove_rows(Eigen::VectorXd the_vector, std::set<int> elements_to_be_removed);

	NekDouble Get_m_kinvis(void);
	void Set_m_kinvis(NekDouble);

    protected:
        

        virtual void v_InitObject();

    private:
        /// Id to identify when single mode is mean mode (i.e. beta=0);
        bool m_zeroMode;
        
        int m_counter;
        bool m_initialStep;
        NekDouble   m_tol;        // Tolerence
        int m_maxIt;           // Max number of iteration
        int m_Restart;    // 0=Stokes solution as init guess; 1=Restart.cont as init guess
        int m_MatrixSetUpStep; 
        NekDouble m_kinvisMin;
        NekDouble m_kinvisStep;
        NekDouble m_KinvisPercentage;

        Array<OneD, CoupledSolverMatrices> m_mat;

        void SetUpCoupledMatrix(const NekDouble lambda, 
                                const Array< OneD, Array<OneD, NekDouble> > &Advfield, 
                                bool       IsLinearNSEquation,
                                const int  HomogeneousMode,
                                CoupledSolverMatrices &mat,
                                CoupledLocalToGlobalC0ContMapSharedPtr &locToGloMap,
                                const NekDouble lambda_imag = NekConstants::kNekUnsetDouble);

        virtual void v_DoSolve(void);        

        virtual void v_DoInitialise(void);

        virtual void v_Output(void);

    };
    
    
    
} //end of namespace


/**
 * $Log:$
 *
 **/

