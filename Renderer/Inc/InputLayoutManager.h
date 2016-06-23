#pragma once
/***********************************************
* Filename:
* Date:
* Mod. Date:
* Mod. Initials:
* Author:
* Purpose:
************************************************/

namespace Renderer
{
	
	class CInputLayoutManager
	{


	private:
		//static CInputLayoutManager* instancePtr;
        CInputLayoutManager();
        ~CInputLayoutManager();
	public:
		 enum VertexFormat 
		{
			eVertex_POS = 0,
			eVertex_POSNORUVTAN,
            eVertex_POSUV0UV1,
            ePTVertex_POSVELSIZAGETYP,
			eVetex_MAX,
		};
		static ID3D11InputLayout*	 inputLayouts[eVetex_MAX];

		static CInputLayoutManager& GetRef();
		inline ID3D11InputLayout* InputLayout(VertexFormat _index) { return inputLayouts[_index]; }
		void Initilize();
		static void DeleteInstance();
	};
}
