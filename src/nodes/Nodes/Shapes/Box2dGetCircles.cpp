
#include "Box2dGetCircles.h"
#include "../../Internals/Data/ShapeCustomData.h"
#include "../../Internals/Data/BodyCustomData.h"

using namespace System::Collections::Generic;

namespace VVVV 
{
	namespace Nodes 
	{
		Box2dGetCircles::Box2dGetCircles(void)
		{
			this->tempPosition = gcnew gen::List<v4math::Vector2D>(64);
			this->tempRadius = gcnew gen::List<double>(64);
			this->tempIds = gcnew gen::List<int>(64);
			this->tempCustom = gcnew gen::List<System::String^>(64);
			this->tempBodyId = gcnew gen::List<double>(64);
			this->tempLifetime = gcnew gen::List<double>(64);
			this->tempSensor = gcnew gen::List<double>(64);
		}

		void Box2dGetCircles::SetPluginHost(v4::IPluginHost^ Host) 
		{
			this->FHost = Host;

			this->FHost->CreateNodeInput("Shapes",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInShapes);
			this->vInShapes->SetSubType(VVVV::Utils::ArrayUtils::SingleGuidArray(v4b2d::ShapeDataType::GUID), v4b2d::ShapeDataType::FriendlyName);

			this->FHost->CreateValueInput("Local Coordinates",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vInLocal);
			this->vInLocal->SetSubType(0,1,1,1,false,true,false);

			this->FHost->CreateValueOutput("Position",2,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutPosition);
			this->vOutPosition->SetSubType2D(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,0.0,false,false,false);

			this->FHost->CreateValueOutput("Radius",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutRadius);
			this->vOutRadius->SetSubType(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,false,false,false);

			this->FHost->CreateValueOutput("Is Sensor",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutIsSensor);
			this->vOutIsSensor->SetSubType(System::Double::MinValue,System::Double::MaxValue,0.01,0.0,false,true,false);

			this->FHost->CreateStringOutput("Custom",v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutCustom);
			this->vOutCustom->SetSubType("",false);

			this->FHost->CreateValueOutput("Shape Id",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutId);
			this->vOutId->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,true);

			this->FHost->CreateValueOutput("Body Id",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutBodyId);
			this->vOutBodyId->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,true);

			this->FHost->CreateValueOutput("LifeTime",1,nullptr,v4::TSliceMode::Dynamic,v4::TPinVisibility::True,this->vOutLifeTime);
			this->vOutLifeTime->SetSubType(System::Double::MinValue,System::Double::MaxValue,1,0.0,false,false,false);
		}


		void Box2dGetCircles::Configurate(v4::IPluginConfig^ Input)
		{

		}

		
		void Box2dGetCircles::Evaluate(int SpreadMax)
		{
			this->vInShapes->PinIsChanged;
			
			if (this->vInLocal->PinIsChanged)
			{
				double dbllocal;
				this->vInLocal->GetValue(0,dbllocal);
				this->m_local = dbllocal >= 0.5;
			}

			if (this->vInShapes->IsConnected) 
			{
				this->tempPosition->Clear();
				this->tempRadius->Clear();
				this->tempIds->Clear();
				this->tempCustom->Clear();
				this->tempLifetime->Clear();
				this->tempBodyId->Clear();
				this->tempSensor->Clear();

				int cnt = 0;
				for (int i = 0; i < this->vInShapes->SliceCount ; i++) 
				{
					int realslice;
					this->vInShapes->GetUpsreamSlice(i,realslice);
					b2Shape* shape = this->m_circles->GetSlice(realslice);
					if (shape->GetType() == e_circleShape) 
					{
						b2CircleShape* circle = (b2CircleShape*)shape;

						b2Vec2 local = circle->GetLocalPosition();
						if (this->m_local)
						{
							v4math::Vector2D vec = v4math::Vector2D(local.x,local.y);
							this->tempPosition->Add(vec);
						}
						else
						{
							b2Vec2 world = circle->GetBody()->GetWorldPoint(local);
							v4math::Vector2D vec = v4math::Vector2D(world.x,world.y);
							this->tempPosition->Add(vec);
						}

						tempRadius->Add(circle->GetRadius());
						

						ShapeCustomData* sdata = (ShapeCustomData*)shape->GetUserData();
						tempIds->Add(sdata->Id);
						tempLifetime->Add(sdata->LifeTime);
						tempSensor->Add(shape->IsSensor() ? 1.0 : 0.0);
						System::String^ str = gcnew System::String(sdata->Custom);
						tempCustom->Add(str);

						BodyCustomData* bdata = (BodyCustomData*)shape->GetBody()->GetUserData();
						tempBodyId->Add(bdata->Id);

						cnt++;
					}
				}

				this->vOutPosition->SliceCount = cnt;
				this->vOutRadius->SliceCount = cnt;
				this->vOutId->SliceCount = cnt;
				this->vOutIsSensor->SliceCount = cnt;
				this->vOutBodyId->SliceCount = cnt;
				this->vOutLifeTime->SliceCount = cnt;

				this->vOutCustom->SliceCount = cnt;

				double *positionPtr, *radiusPtr, *idPtr, *sensorPtr, *bodyIdPtr, *lifetimePtr;
				this->vOutPosition->GetValuePointer(positionPtr);
				this->vOutRadius->GetValuePointer(radiusPtr);
				this->vOutId->GetValuePointer(idPtr);
				this->vOutIsSensor->GetValuePointer(sensorPtr);
				this->vOutBodyId->GetValuePointer(bodyIdPtr);
				this->vOutLifeTime->GetValuePointer(lifetimePtr);

				for (int i = 0; i < cnt ; i++) 
				{	
					positionPtr[i * 2] = tempPosition[i].x;
					positionPtr[i * 2 + 1] = tempPosition[i].y;

					radiusPtr[i] = tempRadius[i];
					idPtr[i] = tempIds[i];
					sensorPtr[i] = tempSensor[i];
					bodyIdPtr[i] = tempBodyId[i];
					lifetimePtr[i] = tempLifetime[i];

					this->vOutCustom->SetString(i, tempCustom[i]);
				}
			}
		}

		void Box2dGetCircles::ConnectPin(v4::IPluginIO^ Pin)
		{
			if (Pin == this->vInShapes) 
			{
				v4::INodeIOBase^ usI;
				this->vInShapes->GetUpstreamInterface(usI);
				this->m_circles = (v4b2d::ShapeDataType^)usI;
			}
		}

		void Box2dGetCircles::DisconnectPin(v4::IPluginIO^ Pin)
		{
			if (Pin == this->vInShapes)
        	{
        		this->m_circles = nullptr;
        	}
		}
	}
}
