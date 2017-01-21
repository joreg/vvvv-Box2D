#include "StdAfx.h"
#include "Box2dTensorDampingControllerDefNode.h"

namespace VVVV 
{
	namespace Nodes 
	{
		Box2dTensorDampingControllerDefNode::Box2dTensorDampingControllerDefNode(void)
		{
			this->m_controller = gcnew ControllerDataType();
		}

		void Box2dTensorDampingControllerDefNode::OnEvaluate(int SpreadMax, bool reset)
		{
			if (this->vInModel->PinIsChanged
				|| reset)
			{

				for (int i = 0; i < SpreadMax; i++)
				{
					double x,y,z,w;
					this->vInModel->GetValue4D(i, x,y,z,w);
					//b2Mat22* mat = new b2Mat22(x,y,z,w);
					
					if (reset)
					{
						b2TensorDampingControllerDef ctrldef;
						ctrldef.T.col1.x = (float)x;
						ctrldef.T.col2.x = (float)y;
						ctrldef.T.col1.y = (float)z;
						ctrldef.T.col2.y = (float)w;
						this->ctrl->push_back(this->m_world->GetWorld()->CreateController(&ctrldef));
						this->m_controller->Add(this->ctrl->at(i));
					}
					else
					{
						b2TensorDampingController* gc = (b2TensorDampingController*) this->ctrl->at(i);
						gc->T.col1.x = (float)x;
						gc->T.col2.x = (float)y;
						gc->T.col1.y = (float)z;
						gc->T.col2.y = (float)w;
						//gc->T = *mat;
					}


				}
			}
		}
		
		void Box2dTensorDampingControllerDefNode::OnPluginHostSet()
		{
			this->FHost->CreateValueInput("Model",4,nullptr,TSliceMode::Dynamic,TPinVisibility::True,this->vInModel);
			this->vInModel->SetSubType4D(Double::MinValue,Double::MaxValue,0.01,-1.0,0.0,0.0,-1.0,false,false,false);
		}

	}
}
